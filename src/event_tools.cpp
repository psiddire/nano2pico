#include "event_tools.hpp"

#include "utilities.hpp"
#include "hig_trig_eff.hpp"
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
  else if(Contains(name, "SingleElectron")) 
    dataset = Dataset::SingleElectron;
  else if(Contains(name, "SingleMuon")) 
    dataset = Dataset::SingleMuon;
  else if(Contains(name, "DoubleEG")) 
    dataset = Dataset::DoubleEG;
  else if(Contains(name, "DoubleMuon")) 
    dataset = Dataset::DoubleMuon;
  else if(Contains(name, "MET")) 
    dataset = Dataset::MET;
  else if(Contains(name, "JetHT")) 
    dataset = Dataset::JetHT;
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
  if (isFastsim) {
    // Fastsim: veto if certain central jets have no matching GenJet as per SUSY recommendation:
    // https://twiki.cern.ch/twiki/bin/view/CMS/SUSRecommendations18#Cleaning_up_of_fastsim_jets_from
    for(int ijet(0); ijet<nano.nJet(); ++ijet){
      if(Jet_pt[ijet] > 20 && fabs(nano.Jet_eta()[ijet])<=2.5 && nano.Jet_chHEF()[ijet] < 0.1) {
        bool found_match = false;
        for(int igenjet(0); igenjet<nano.nGenJet(); ++igenjet){
          if (dR(nano.Jet_eta()[ijet], nano.GenJet_eta()[igenjet], nano.Jet_phi()[ijet], nano.GenJet_phi()[igenjet])<=0.3) {
            found_match = true;
            break;
          }
        }
        if (!found_match) {
          pico.out_pass_jets() = false;
          break;
        }
      }
    }
  } else { // Fullsim: require just loosest possible ID for now (for all jets, not just central!)
    for(int ijet(0); ijet<nano.nJet(); ++ijet){
      if (Jet_pt[ijet] > min_jet_pt && nano.Jet_jetId()[ijet] < 1) 
        pico.out_pass_jets() = false;
    } 
  }

  // RA2b filters  
  pico.out_pass_muon_jet() = true; 
  for (auto &idx: sig_jet_nano_idx){
    // if (abs(nano.Jet_eta()[idx])>2.4) continue; -> already enforced in signal jet selection
    // if is overlapping with lepton -> already enforced in signal jet selection
    if (Jet_pt[idx]<=200.) continue;
    if (nano.Jet_muEF()[idx]<=0.5) continue;
    if (DeltaPhi(nano.Jet_phi()[idx],MET_phi)<(TMath::Pi()-0.4)) continue;
    pico.out_pass_muon_jet() = false;
    break;
  }

  pico.out_pass_low_neutral_jet() = true;
  for(int ijet(0); ijet<nano.nJet(); ++ijet){  
    if (nano.Jet_neEmEF()[ijet] <0.03 && DeltaPhi(nano.Jet_phi()[ijet], pico.out_met_phi())>(TMath::Pi()-0.4))
      pico.out_pass_low_neutral_jet() = false;
    break; //only apply to leading jet
  }

  pico.out_pass_htratio_dphi_tight() = true;
  float htratio = pico.out_ht5()/pico.out_ht();
  for(int ijet(0); ijet<nano.nJet(); ++ijet){  
    if (htratio >= 1.2 && DeltaPhi(nano.Jet_phi()[ijet], pico.out_met_phi()) < (5.3*htratio - 4.78)) 
      pico.out_pass_htratio_dphi_tight() = false;
    break; //only apply to leading jet
  }

  pico.out_pass_ecalnoisejet() = true;
  if (year!=2016) {
    int counter = 0;
    bool goodjet[2] = {true, true};
    double dphi = 0.;
    for (int ijet(0); ijet < nano.nJet(); ijet++) {
      if (counter >= 2) break;
      float jet_pt = nano.Jet_pt()[ijet];
      if (isFastsim) jet_pt = nano.Jet_pt_nom()[ijet];
      if (jet_pt>30 && fabs(nano.Jet_eta()[ijet])>2.4 && fabs(nano.Jet_eta()[ijet])<5.0) {
        dphi = DeltaPhi(nano.Jet_phi()[ijet], pico.out_met_phi());
        if (nano.Jet_pt()[ijet]>250 && (dphi > 2.6 || dphi < 0.1)) goodjet[counter] = false;
        ++counter;
      }
    }
    pico.out_pass_ecalnoisejet() = goodjet[0] && goodjet[1];
  }

  // filters directly from Nano
  pico.out_pass_hbhe() = nano.Flag_HBHENoiseFilter();
  pico.out_pass_hbheiso() = nano.Flag_HBHENoiseIsoFilter();
  pico.out_pass_goodv() = nano.Flag_goodVertices();
  pico.out_pass_cschalo_tight() = nano.Flag_globalSuperTightHalo2016Filter();
  pico.out_pass_eebadsc() = nano.Flag_eeBadScFilter();
  pico.out_pass_ecaldeadcell() = nano.Flag_EcalDeadCellTriggerPrimitiveFilter();
  if (year==2016) {
    pico.out_pass_badcalib() = true;
  } else {
    pico.out_pass_badcalib() = nano.Flag_ecalBadCalibFilterV2();
  }
  pico.out_pass_badchhad() = nano.Flag_BadChargedCandidateFilter();
  pico.out_pass_badpfmu() = nano.Flag_BadPFMuonFilter();
  pico.out_pass_mubadtrk() = nano.Flag_muonBadTrackFilter();

  // Combined pass variable, as recommended here:
  // https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2#Analysis_Recommendations_for_ana
  pico.out_pass() = pico.out_pass_muon_jet() && pico.out_pass_badpfmu() && 
                    pico.out_met()/pico.out_met_calo()<5 &&
                    pico.out_pass_goodv() &&
                    pico.out_pass_hbhe() && pico.out_pass_hbheiso() && 
                    pico.out_pass_ecaldeadcell() && pico.out_pass_badcalib() &&
                    pico.out_pass_jets();

  if (!isFastsim) {
    pico.out_pass() = pico.out_pass() && pico.out_pass_cschalo_tight();
    if (isData) 
      pico.out_pass() = pico.out_pass() && pico.out_pass_eebadsc();
  }
  
  // Combined pass RA2b-like variable
  // https://github.com/rpatelCERN/boostedHiggsPlusMET/blob/Higgsino/src/definitions.cc#L1137
  pico.out_pass_boosted() = pico.out_pass_hbhe() && 
                         pico.out_pass_hbheiso() && 
                         pico.out_pass_eebadsc() && 
                         // pico.out_pass_ecaldeadcell() && 
                         pico.out_pass_goodv() &&
                         pico.out_met()/pico.out_met_calo() < 5. &&
                         pico.out_pass_badpfmu() && 
                         pico.out_pass_cschalo_tight() && 
                         pico.out_pass_low_neutral_jet() && 
                         pico.out_pass_htratio_dphi_tight() && 
                         pico.out_pass_jets();

  return;
}

bool EventTools::SaveTriggerDecisions(nano_tree& nano, pico_tree& pico, bool isZgamma){

  bool egamma_trigs = nano.HLT_Ele25_WPTight_Gsf() || nano.HLT_Ele27_WPTight_Gsf() || 
                      nano.HLT_Ele28_WPTight_Gsf() || nano.HLT_Ele32_WPTight_Gsf() ||
	              nano.HLT_Ele32_WPTight_Gsf_L1DoubleEG() || nano.HLT_Ele35_WPTight_Gsf() || 
                      nano.HLT_Ele20_WPLoose_Gsf() || nano.HLT_Ele45_WPLoose_Gsf() ||
                      nano.HLT_Ele105_CaloIdVT_GsfTrkIdT() || nano.HLT_Ele115_CaloIdVT_GsfTrkIdT() ||
                      nano.HLT_Ele135_CaloIdVT_GsfTrkIdT() || nano.HLT_Ele145_CaloIdVT_GsfTrkIdT() ||
                      nano.HLT_Ele25_eta2p1_WPTight_Gsf() || nano.HLT_Ele27_eta2p1_WPTight_Gsf() || 
                      nano.HLT_Ele20_eta2p1_WPLoose_Gsf() || nano.HLT_Ele25_eta2p1_WPLoose_Gsf() ||
                      nano.HLT_Ele27_eta2p1_WPLoose_Gsf() || nano.HLT_Ele15_IsoVVVL_PFHT350() ||
		      nano.HLT_Ele15_IsoVVVL_PFHT400() || nano.HLT_Ele15_IsoVVVL_PFHT450() ||
		      nano.HLT_Ele15_IsoVVVL_PFHT600() || nano.HLT_Ele50_IsoVVVL_PFHT450();

  pico.out_HLT_Ele25_WPTight_Gsf() = nano.HLT_Ele25_WPTight_Gsf();
  pico.out_HLT_Ele27_WPTight_Gsf() = nano.HLT_Ele27_WPTight_Gsf();
  pico.out_HLT_Ele28_WPTight_Gsf() = nano.HLT_Ele28_WPTight_Gsf();
  pico.out_HLT_Ele32_WPTight_Gsf() = nano.HLT_Ele32_WPTight_Gsf();
  pico.out_HLT_Ele32_WPTight_Gsf_L1DoubleEG() = nano.HLT_Ele32_WPTight_Gsf_L1DoubleEG();
  pico.out_HLT_Ele35_WPTight_Gsf() = nano.HLT_Ele35_WPTight_Gsf();
  pico.out_HLT_Ele20_WPLoose_Gsf() = nano.HLT_Ele20_WPLoose_Gsf();
  pico.out_HLT_Ele45_WPLoose_Gsf() = nano.HLT_Ele45_WPLoose_Gsf();
  pico.out_HLT_Ele105_CaloIdVT_GsfTrkIdT() = nano.HLT_Ele105_CaloIdVT_GsfTrkIdT();
  pico.out_HLT_Ele115_CaloIdVT_GsfTrkIdT() = nano.HLT_Ele115_CaloIdVT_GsfTrkIdT();
  pico.out_HLT_Ele135_CaloIdVT_GsfTrkIdT() = nano.HLT_Ele135_CaloIdVT_GsfTrkIdT();
  pico.out_HLT_Ele145_CaloIdVT_GsfTrkIdT() = nano.HLT_Ele145_CaloIdVT_GsfTrkIdT();
  pico.out_HLT_Ele25_eta2p1_WPTight_Gsf() = nano.HLT_Ele25_eta2p1_WPTight_Gsf();
  pico.out_HLT_Ele27_eta2p1_WPTight_Gsf() = nano.HLT_Ele27_eta2p1_WPTight_Gsf();
  pico.out_HLT_Ele20_eta2p1_WPLoose_Gsf() = nano.HLT_Ele20_eta2p1_WPLoose_Gsf();
  pico.out_HLT_Ele25_eta2p1_WPLoose_Gsf() = nano.HLT_Ele25_eta2p1_WPLoose_Gsf();
  pico.out_HLT_Ele27_eta2p1_WPLoose_Gsf() = nano.HLT_Ele27_eta2p1_WPLoose_Gsf();
  pico.out_HLT_Ele15_IsoVVVL_PFHT350() = nano.HLT_Ele15_IsoVVVL_PFHT350();
  pico.out_HLT_Ele15_IsoVVVL_PFHT400() = nano.HLT_Ele15_IsoVVVL_PFHT400();
  pico.out_HLT_Ele15_IsoVVVL_PFHT450() = nano.HLT_Ele15_IsoVVVL_PFHT450();
  pico.out_HLT_Ele15_IsoVVVL_PFHT600() = nano.HLT_Ele15_IsoVVVL_PFHT600();
  pico.out_HLT_Ele50_IsoVVVL_PFHT450() = nano.HLT_Ele50_IsoVVVL_PFHT450();

  bool muon_trigs = nano.HLT_IsoMu20() || nano.HLT_IsoMu22() || nano.HLT_IsoMu24() ||
                    nano.HLT_IsoMu27() || nano.HLT_IsoTkMu20() || nano.HLT_IsoTkMu22() ||
                    nano.HLT_IsoTkMu24() || nano.HLT_Mu50() || nano.HLT_Mu55() ||
                    nano.HLT_TkMu50() || nano.HLT_IsoMu22_eta2p1() || nano.HLT_IsoMu24_eta2p1() ||
                    nano.HLT_Mu45_eta2p1() || nano.HLT_Mu15_IsoVVVL_PFHT350() || nano.HLT_Mu15_IsoVVVL_PFHT400() ||
                    nano.HLT_Mu15_IsoVVVL_PFHT450() || nano.HLT_Mu15_IsoVVVL_PFHT600() || nano.HLT_Mu50_IsoVVVL_PFHT400() ||
                    nano.HLT_Mu50_IsoVVVL_PFHT450();

  pico.out_HLT_IsoMu20() = nano.HLT_IsoMu20();
  pico.out_HLT_IsoMu22() = nano.HLT_IsoMu22();
  pico.out_HLT_IsoMu24() = nano.HLT_IsoMu24();
  pico.out_HLT_IsoMu27() = nano.HLT_IsoMu27();
  pico.out_HLT_IsoTkMu20() = nano.HLT_IsoTkMu20();
  pico.out_HLT_IsoTkMu22() = nano.HLT_IsoTkMu22();
  pico.out_HLT_IsoTkMu24() = nano.HLT_IsoTkMu24();
  pico.out_HLT_Mu50() = nano.HLT_Mu50();
  pico.out_HLT_Mu55() = nano.HLT_Mu55();
  pico.out_HLT_TkMu50() = nano.HLT_TkMu50();
  pico.out_HLT_IsoMu22_eta2p1() = nano.HLT_IsoMu22_eta2p1();
  pico.out_HLT_IsoMu24_eta2p1() = nano.HLT_IsoMu24_eta2p1();
  pico.out_HLT_Mu45_eta2p1() = nano.HLT_Mu45_eta2p1();
  pico.out_HLT_Mu15_IsoVVVL_PFHT350() = nano.HLT_Mu15_IsoVVVL_PFHT350();
  pico.out_HLT_Mu15_IsoVVVL_PFHT400() = nano.HLT_Mu15_IsoVVVL_PFHT400();
  pico.out_HLT_Mu15_IsoVVVL_PFHT450() = nano.HLT_Mu15_IsoVVVL_PFHT450();
  pico.out_HLT_Mu15_IsoVVVL_PFHT600() = nano.HLT_Mu15_IsoVVVL_PFHT600();
  pico.out_HLT_Mu50_IsoVVVL_PFHT400() = nano.HLT_Mu50_IsoVVVL_PFHT400();
  pico.out_HLT_Mu50_IsoVVVL_PFHT450() = nano.HLT_Mu50_IsoVVVL_PFHT450();

  // ZGamma triggers
  pico.out_HLT_Mu17_Photon30_IsoCaloId()               = nano.HLT_Mu17_Photon30_IsoCaloId();
  pico.out_HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ() = nano.HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ();
  pico.out_HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL()    = nano.HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL();
  pico.out_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL()          = nano.HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL();
  pico.out_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL()        = nano.HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL();
  pico.out_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ()       = nano.HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ();
  pico.out_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ()     = nano.HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ();
  pico.out_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8() = nano.HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8();
  pico.out_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8()   = nano.HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8();
  pico.out_HLT_Photon175()                             = nano.HLT_Photon175();

  if ((dataset==Dataset::DoubleMuon) && muon_trigs) 
    return true;
  else if (((year==2018 && dataset==Dataset::EGamma) || ((year==2016 || year==2017) && dataset==Dataset::DoubleEG)) && egamma_trigs) 
    return true;
  else 
    return false;

  return false;
}

void EventTools::WriteTriggerEfficiency(pico_tree &pico) {
  // trigger efficiency and uncertainty - @todo, needs to be updated to full Run 2 trig eff. measurement
  pico.out_eff_trig() = hig_trig_eff::eff(pico);
  float effunc = hig_trig_eff::eff_unc(pico);
  pico.out_sys_trig().resize(2,0);
  pico.out_sys_trig()[0] = 1+effunc;
  pico.out_sys_trig()[1] = 1-effunc;

}
