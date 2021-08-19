#include "event_tools.hpp"

#include "utilities.hpp"
#include "TMath.h"

using namespace std;

EventTools::EventTools(const string &name_, int year_):
  name(name_),
  year(year_),
  isTTJets_LO_Incl(false),
  isTTJets_LO_MET(false),
  isTTJets_LO_HT(false),
  isWJets_LO(false),
  isDYJets_LO(false),
  dataset(-1){

  if(Contains(name, "TTJets_") && Contains(name, "genMET-") && Contains(name, "madgraphMLM")) 
    isTTJets_LO_MET = true;

  if(Contains(name, "TTJets_") && !Contains(name, "TTJets_HT") && !Contains(name, "genMET-") &&Contains(name, "madgraphMLM")) 
    isTTJets_LO_Incl = true;

  if(Contains(name, "TTJets_HT") && Contains(name, "madgraphMLM")) 
    isTTJets_LO_HT = true;

  if(Contains(name, "WJetsToLNu_Tune")  && Contains(name,"madgraphMLM"))
    isWJets_LO = true;
  
  if(Contains(name, "DYJetsToLL_M-50_Tune")  && Contains(name,"madgraphMLM"))
    isDYJets_LO = true;

  if(Contains(name, "EGamma")) // looks like this replaced SingleElectron and DoubleEG starting in 2018
    dataset = Dataset::EGamma;
  else if(Contains(name, "DoubleEG")) 
    dataset = Dataset::DoubleEG;
  else if(Contains(name, "DoubleMuon")) 
    dataset = Dataset::DoubleMuon;
}

EventTools::~EventTools(){
}

void EventTools::WriteStitch(nano_tree &nano, pico_tree &pico){
  pico.out_stitch_photon() = pico.out_stitch_htmet() = pico.out_stitch() = pico.out_stitch_ht() = true;
  if(isTTJets_LO_Incl) {
    if (nano.LHE_HTIncoming()>600) 
      pico.out_stitch_htmet() = pico.out_stitch_ht() = false;
    if(year==2018 && nano.GenMET_pt()>80)
      pico.out_stitch_htmet() = pico.out_stitch() = false;
    else if (nano.GenMET_pt()>150) 
      pico.out_stitch_htmet() = pico.out_stitch() = false;
  }

  if (isTTJets_LO_MET && nano.LHE_HTIncoming()>600) 
      pico.out_stitch_htmet() = pico.out_stitch_ht() = false;

  if (isTTJets_LO_Incl || isTTJets_LO_MET || isTTJets_LO_HT) {
    //remove events covered by TTG, see AN-17-197 (TOP-18-010)
    //stitch if prompt photon w pt>13 |eta|<3 deltaR(genPart[pt>5])>0.2
    for (unsigned int mc_idx = 0; mc_idx < nano.GenPart_pdgId().size(); mc_idx++) {
      if (nano.GenPart_pdgId().at(mc_idx) == 22) {
	float ph_pt = nano.GenPart_pt().at(mc_idx);
	float ph_eta = nano.GenPart_eta().at(mc_idx);
	float ph_phi = nano.GenPart_phi().at(mc_idx);
        if (ph_pt > 13 && fabs(ph_eta)<3.0 && (nano.GenPart_statusFlags().at(mc_idx) & 0x1) == 1) {
	  //check if another genparticle nearby
	  bool deltar_fail = false;
          for (unsigned int mc_idx_2 = 0; mc_idx_2 < nano.GenPart_pdgId().size(); mc_idx_2++) {
            if (nano.GenPart_pt().at(mc_idx_2)>5 && dR(ph_eta,nano.GenPart_eta().at(mc_idx_2),ph_phi,nano.GenPart_phi().at(mc_idx_2))<0.2) {
              deltar_fail = true;
	      break;
	    }
          }
          if (!deltar_fail) {
            pico.out_stitch_photon() = pico.out_stitch() = false;
          }
	}
      }
    }
  }

  if(isDYJets_LO  && nano.LHE_HT()>70) 
    pico.out_stitch_htmet() = pico.out_stitch_ht() = pico.out_stitch() = false;
  
  if(isWJets_LO  && nano.LHE_HT()>70) 
    pico.out_stitch_htmet() = pico.out_stitch_ht() = pico.out_stitch() = false;
  return;
}


void EventTools::WriteDataQualityFilters(nano_tree& nano, pico_tree& pico, vector<int> sig_jet_nano_idx,
                                         float min_jet_pt, bool isData, bool isFastsim){
  float MET_pt, MET_phi;
  getMETWithJEC(nano, year, isFastsim, MET_pt, MET_phi);
  vector<float> Jet_pt, Jet_mass;
  getJetWithJEC(nano, isFastsim, Jet_pt, Jet_mass);

  // jet quality filter
  pico.out_pass_jets() = true;
  for(int ijet(0); ijet<nano.nJet(); ++ijet){
    if (Jet_pt[ijet] > min_jet_pt && nano.Jet_jetId()[ijet] < 1) 
      pico.out_pass_jets() = false;
  } 

  // filters directly from Nano 
  // Check for 2016 MC
  pico.out_pass_goodv() = nano.Flag_goodVertices();
  pico.out_pass_cschalo_tight() = nano.Flag_globalSuperTightHalo2016Filter();
  pico.out_pass_hbhe() = nano.Flag_HBHENoiseFilter();
  pico.out_pass_hbheiso() = nano.Flag_HBHENoiseIsoFilter();
  pico.out_pass_ecaldeadcell() = nano.Flag_EcalDeadCellTriggerPrimitiveFilter();
  pico.out_pass_badpfmu() = nano.Flag_BadPFMuonFilter();
  pico.out_pass_badpfmudz() = nano.Flag_BadPFMuonDzFilter();
  pico.out_pass_eebadsc() = nano.Flag_eeBadScFilter();

  if (year==2016) {
    pico.out_pass_hfnoise() = true;
    pico.out_pass_badcalib() = true;
  } else {
    pico.out_pass_hfnoise() = nano.Flag_hfNoisyHitsFilter();
    pico.out_pass_badcalib() = nano.Flag_ecalBadCalibFilterV2();
  }

  // https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2#Analysis_Recommendations_for_ana
  pico.out_pass() = pico.out_pass_goodv() && pico.out_pass_cschalo_tight() &&
                    pico.out_pass_hbhe() && pico.out_pass_hbheiso() && 
                    pico.out_pass_ecaldeadcell() && pico.out_pass_badpfmu() &&
                    pico.out_pass_badpfmudz() && pico.out_pass_eebadsc();

  return;
}

bool EventTools::SaveTriggerDecisions(nano_tree& nano, pico_tree& pico){

  // ZGamma triggers
  pico.out_HLT_Mu17_Photon30_IsoCaloId()               = nano.HLT_Mu17_Photon30_IsoCaloId();
  pico.out_HLT_Ele23_CaloIdM_TrackIdM_PFJet30()        = nano.HLT_Ele23_CaloIdM_TrackIdM_PFJet30();
  pico.out_HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30()  = nano.HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30();
  pico.out_HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ() = nano.HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ();
  pico.out_HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL()    = nano.HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL();
  pico.out_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL()          = nano.HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL();
  pico.out_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL()        = nano.HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL();
  pico.out_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ()       = nano.HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ();
  pico.out_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ()     = nano.HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ();
  pico.out_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8() = nano.HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8();
  pico.out_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8()   = nano.HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8();

  bool egamma_trigs = nano.HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ() || nano.HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL() || 
                      nano.HLT_Ele23_CaloIdM_TrackIdM_PFJet30() || nano.HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30();

  bool muon_trigs = nano.HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL() || nano.HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL() ||
                    nano.HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ() || nano.HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ() ||
                    nano.HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8() || nano.HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8();

  if ((dataset==Dataset::DoubleMuon) && muon_trigs) 
    return true;
  else if (((year==2018 && dataset==Dataset::EGamma) || ((year==2016 || year==2017) && dataset==Dataset::DoubleEG)) && egamma_trigs) 
    return true;
  else 
    return false;

  return false;
}

