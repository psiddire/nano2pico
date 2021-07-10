#include "jet_producer.hpp"

#include <algorithm>
#include <iomanip> 
#include <vector>

#include "TLorentzVector.h"

#include "utilities.hpp"

using namespace std;

JetProducer::JetProducer(int year_, float min_jet_pt_, float max_jet_eta_, bool isData_, bool verbose_){
  year = year_;
  isData = isData_;
  verbose = verbose_;
  min_jet_pt = min_jet_pt_;
  max_jet_eta = max_jet_eta_;
}

JetProducer::~JetProducer(){
}

vector<int> JetProducer::WriteJets(nano_tree &nano, pico_tree &pico, 
                                   vector<int> jet_islep_nano_idx, 
                                   vector<int> jet_isvlep_nano_idx,  
                                   vector<int> jet_isphoton_nano_idx,
                                   const vector<float> &btag_wpts, 
                                   const vector<float> &btag_df_wpts, 
                                   bool isFastsim, 
                                   bool isSignal){
  vector<int> sig_jet_nano_idx;
  pico.out_njet() = 0; pico.out_ht() = 0; pico.out_ht5() = 0; 
  pico.out_nbl() = 0; pico.out_nbm() = 0; pico.out_nbt() = 0; 
  pico.out_nbdfl() = 0; pico.out_nbdfm() = 0; pico.out_nbdft() = 0; 

  vector<float> Jet_pt, Jet_mass;
  getJetWithJEC(nano, isFastsim, Jet_pt, Jet_mass);
  float MET_pt, MET_phi;
  getMETWithJEC(nano, year, isFastsim, MET_pt, MET_phi);
  
  // calculate MHT; needed when saving jet info
  TLorentzVector mht_vec;
  for(int ijet(0); ijet<nano.nJet(); ++ijet){
    if (Jet_pt[ijet] > min_jet_pt) {
      TLorentzVector ijet_v4;
      ijet_v4.SetPtEtaPhiM(Jet_pt[ijet], nano.Jet_eta()[ijet], nano.Jet_phi()[ijet], Jet_mass[ijet]);
      mht_vec -= ijet_v4;
    }
  }
  pico.out_mht() = mht_vec.Pt();
  pico.out_mht_phi() = mht_vec.Phi();

  vector<vector<float>> sys_jet_met_dphi;

  if (isSignal) {
    pico.out_sys_njet().resize(4,0);
    pico.out_sys_nbl().resize(4,0);
    pico.out_sys_nbm().resize(4,0);
    pico.out_sys_nbt().resize(4,0);
    pico.out_sys_ht().resize(4,0.0);
    sys_jet_met_dphi.resize(4,vector<float>({}));
    pico.out_sys_low_dphi_met().resize(4,false);
  }

  // saving jet info on all jets passing pt cut, including endcap
  for(int ijet(0); ijet<nano.nJet(); ++ijet){
    if (verbose) cout<<"Jet "<<ijet<<": pt = "<<setw(10)<<Jet_pt[ijet]
                                    <<" eta = "<<setw(10)<<nano.Jet_eta()[ijet]
                                    <<" phi = "<<setw(10)<<nano.Jet_phi()[ijet]
                                    <<" m = "<<setw(10)<<Jet_mass[ijet]
                                    <<endl;


    // check overlap with signal leptons (or photons)
    bool islep = find(jet_islep_nano_idx.begin(), jet_islep_nano_idx.end(), ijet) != jet_islep_nano_idx.end();
    // check overlap with veto leptons
    bool isvlep = find(jet_isvlep_nano_idx.begin(), jet_isvlep_nano_idx.end(), ijet) != jet_isvlep_nano_idx.end();
    // N.B. photon collection is not filled for Higgsino analysis, so there is no overlap removal!
    bool isphoton = find(jet_isphoton_nano_idx.begin(), jet_isphoton_nano_idx.end(), ijet) != jet_isphoton_nano_idx.end();
    // jetid applied to only full sim and data
    bool pass_jetid = true;
    if (!isFastsim) if (nano.Jet_jetId()[ijet] <1) pass_jetid = false;

    bool isgood = !islep && !isphoton && (fabs(nano.Jet_eta()[ijet]) <= max_jet_eta) && pass_jetid;

    //sys_jetvar convention: [0] JER up, [1] JER down, [2] JEC up, [3] JEC down
    //for now, only save sys_ variables
    if (isSignal) {
      if (nano.Jet_pt_jerUp()[ijet] > min_jet_pt) {
        if (isgood) {
          pico.out_sys_njet()[0]++;
          pico.out_sys_ht()[0] += nano.Jet_pt_jerUp()[ijet];
          if (nano.Jet_btagDeepB()[ijet] > btag_wpts[0]) pico.out_sys_nbl()[0]++; 
          if (nano.Jet_btagDeepB()[ijet] > btag_wpts[1]) pico.out_sys_nbm()[0]++; 
          if (nano.Jet_btagDeepB()[ijet] > btag_wpts[2]) pico.out_sys_nbt()[0]++;
          TLorentzVector lv;
          lv.SetPtEtaPhiM(nano.Jet_pt_jerUp()[ijet], nano.Jet_eta()[ijet],
                          nano.Jet_phi()[ijet], nano.Jet_mass_jerUp()[ijet]);
        }
        if (fabs(nano.Jet_eta()[ijet]) < 2.4)
          sys_jet_met_dphi.at(0).push_back(DeltaPhi(nano.Jet_phi()[ijet], pico.out_sys_met_phi()[0]));
      }
      if (nano.Jet_pt_jerDown()[ijet] > min_jet_pt) {
        if (isgood) {
          pico.out_sys_njet()[1]++;
          pico.out_sys_ht()[1] += nano.Jet_pt_jerDown()[ijet];
          if (nano.Jet_btagDeepB()[ijet] > btag_wpts[0]) pico.out_sys_nbl()[1]++; 
          if (nano.Jet_btagDeepB()[ijet] > btag_wpts[1]) pico.out_sys_nbm()[1]++; 
          if (nano.Jet_btagDeepB()[ijet] > btag_wpts[2]) pico.out_sys_nbt()[1]++;
          TLorentzVector lv;
          lv.SetPtEtaPhiM(nano.Jet_pt_jerDown()[ijet], nano.Jet_eta()[ijet],
                          nano.Jet_phi()[ijet], nano.Jet_mass_jerDown()[ijet]);
        }
        if (fabs(nano.Jet_eta()[ijet]) < 2.4)
          sys_jet_met_dphi.at(1).push_back(DeltaPhi(nano.Jet_phi()[ijet], pico.out_sys_met_phi()[1]));
      }
      if (nano.Jet_pt_jesTotalUp()[ijet] > min_jet_pt) {
        if (isgood) {
          pico.out_sys_njet()[2]++;
          pico.out_sys_ht()[2] += nano.Jet_pt_jesTotalUp()[ijet];
          if (nano.Jet_btagDeepB()[ijet] > btag_wpts[0]) pico.out_sys_nbl()[2]++; 
          if (nano.Jet_btagDeepB()[ijet] > btag_wpts[1]) pico.out_sys_nbm()[2]++; 
          if (nano.Jet_btagDeepB()[ijet] > btag_wpts[2]) pico.out_sys_nbt()[2]++;
          TLorentzVector lv;
          lv.SetPtEtaPhiM(nano.Jet_pt_jesTotalUp()[ijet], nano.Jet_eta()[ijet],
                          nano.Jet_phi()[ijet], nano.Jet_mass_jesTotalUp()[ijet]);
        }
        if (fabs(nano.Jet_eta()[ijet]) < 2.4)
          sys_jet_met_dphi.at(2).push_back(DeltaPhi(nano.Jet_phi()[ijet], pico.out_sys_met_phi()[2]));
      }
      if (nano.Jet_pt_jesTotalDown()[ijet] > min_jet_pt) {
        if (isgood) {
          pico.out_sys_njet()[3]++;
          pico.out_sys_ht()[3] += nano.Jet_pt_jesTotalDown()[ijet];
          if (nano.Jet_btagDeepB()[ijet] > btag_wpts[0]) pico.out_sys_nbl()[3]++; 
          if (nano.Jet_btagDeepB()[ijet] > btag_wpts[1]) pico.out_sys_nbm()[3]++; 
          if (nano.Jet_btagDeepB()[ijet] > btag_wpts[2]) pico.out_sys_nbt()[3]++;
          TLorentzVector lv;
          lv.SetPtEtaPhiM(nano.Jet_pt_jesTotalDown()[ijet], nano.Jet_eta()[ijet],
                          nano.Jet_phi()[ijet], nano.Jet_mass_jesTotalDown()[ijet]);
        }
        if (fabs(nano.Jet_eta()[ijet]) < 2.4)
          sys_jet_met_dphi.at(3).push_back(DeltaPhi(nano.Jet_phi()[ijet], pico.out_sys_met_phi()[3]));
      }
    }

    if (Jet_pt[ijet] <= min_jet_pt) continue;

    pico.out_jet_pt().push_back(Jet_pt[ijet]);
    pico.out_jet_eta().push_back(nano.Jet_eta()[ijet]);
    pico.out_jet_phi().push_back(nano.Jet_phi()[ijet]);
    pico.out_jet_m().push_back(Jet_mass[ijet]);
    pico.out_jet_breg_corr().push_back(nano.Jet_bRegCorr()[ijet]);
    pico.out_jet_breg_res().push_back(nano.Jet_bRegRes()[ijet]);
    pico.out_jet_deepcsv().push_back(nano.Jet_btagDeepB()[ijet]);
    pico.out_jet_deepflav().push_back(nano.Jet_btagDeepFlavB()[ijet]);
    pico.out_jet_ne_emef().push_back(nano.Jet_neEmEF()[ijet]);
    pico.out_jet_qgl().push_back(nano.Jet_qgl()[ijet]);
    pico.out_jet_islep().push_back(islep);
    pico.out_jet_isvlep().push_back(isvlep);
    pico.out_jet_isphoton().push_back(isphoton);
    pico.out_jet_isgood().push_back(isgood);
    pico.out_jet_id().push_back(nano.Jet_jetId()[ijet]);
    pico.out_jet_mht_dphi().push_back(DeltaPhi(nano.Jet_phi()[ijet], mht_vec.Phi()));
    pico.out_jet_met_dphi().push_back(DeltaPhi(nano.Jet_phi()[ijet], MET_phi));
    if (!isData) {
      pico.out_jet_hflavor().push_back(nano.Jet_hadronFlavour()[ijet]);
      pico.out_jet_pflavor().push_back(nano.Jet_partonFlavour()[ijet]);
    }
    
    // will be overwritten with the overlapping fat jet index, if such exists, in WriteFatJets
    pico.out_jet_fjet_idx().push_back(-999);

    //the jets for the higgs pair with smallest dm will be set to true in hig_producer
    pico.out_jet_h1d().push_back(false);
    pico.out_jet_h2d().push_back(false);

    if (!islep && !isphoton) pico.out_ht5() += Jet_pt[ijet];

    if (isgood) {
      sig_jet_nano_idx.push_back(ijet);
      pico.out_njet()++;
      pico.out_ht() += Jet_pt[ijet];
      if (nano.Jet_btagDeepB()[ijet] > btag_wpts[0]) pico.out_nbl()++; 
      if (nano.Jet_btagDeepB()[ijet] > btag_wpts[1]) pico.out_nbm()++; 
      if (nano.Jet_btagDeepB()[ijet] > btag_wpts[2]) pico.out_nbt()++;
      if (nano.Jet_btagDeepFlavB()[ijet] > btag_df_wpts[0]) pico.out_nbdfl()++; 
      if (nano.Jet_btagDeepFlavB()[ijet] > btag_df_wpts[1]) pico.out_nbdfm()++; 
      if (nano.Jet_btagDeepFlavB()[ijet] > btag_df_wpts[2]) pico.out_nbdft()++; 
    }
  } // end jet loop

  pico.out_low_dphi_mht_e5() = false;
  pico.out_low_dphi_met_e5() = false;
  for (unsigned ijet(0); ijet<pico.out_jet_mht_dphi().size(); ijet++){
    float cut_ = ijet<=1 ? 0.5 : 0.3;
    if (pico.out_jet_mht_dphi()[ijet]<=cut_) pico.out_low_dphi_mht_e5() = true;
    if (pico.out_jet_met_dphi()[ijet]<=cut_) pico.out_low_dphi_met_e5() = true;
    if (ijet==3) break;
  }

  pico.out_low_dphi_mht() = false;
  pico.out_low_dphi_met() = false;
  for (unsigned ijet(0), ijet_e24(0); ijet<pico.out_jet_mht_dphi().size(); ijet++){
    if (fabs(pico.out_jet_eta()[ijet]) > max_jet_eta) continue;
    float cut_ = ijet_e24<=1 ? 0.5 : 0.3;
    if (pico.out_jet_mht_dphi()[ijet]<=cut_) pico.out_low_dphi_mht() = true;
    if (pico.out_jet_met_dphi()[ijet]<=cut_) pico.out_low_dphi_met() = true;
    if (ijet_e24==3) break;
    ijet_e24++;
  }

  if (isSignal) {
    for (unsigned ijec(0); ijec < 4; ijec++) {
      for (unsigned ijet(0); ijet < sys_jet_met_dphi[ijec].size(); ijet++) {
        float cut_ = ijet<=1 ? 0.5 : 0.3;
        if (sys_jet_met_dphi[ijec][ijet] <= cut_) pico.out_sys_low_dphi_met()[ijec] = true;
        if (ijet==3) break;
      }
    }
  }

  if (verbose) cout<<"Done with AK4 jets"<<endl;
  return sig_jet_nano_idx;
}

void JetProducer::WriteJetSystemPt(nano_tree &nano, pico_tree &pico, 
                                   vector<int> &sig_jet_nano_idx, const float &btag_wpt, bool isFastsim) {

  vector<float> Jet_pt, Jet_mass;
  getJetWithJEC(nano, isFastsim, Jet_pt, Jet_mass);

  TLorentzVector jetsys_v4, jetsys_nob_v4;
  int njet_nob(0);
  for (auto &idx: sig_jet_nano_idx) {
    TLorentzVector ijet_v4;
    ijet_v4.SetPtEtaPhiM(Jet_pt[idx], nano.Jet_eta()[idx], nano.Jet_phi()[idx], Jet_mass[idx]);
    jetsys_v4 += ijet_v4;

    if (nano.Jet_btagDeepB()[idx] <= btag_wpt){
      njet_nob++;
      jetsys_nob_v4 += ijet_v4;
    }
  }

  if (sig_jet_nano_idx.size()>0) {
    pico.out_jetsys_pt() = jetsys_v4.Pt();
    pico.out_jetsys_eta() = jetsys_v4.Eta();
    pico.out_jetsys_phi() = jetsys_v4.Phi();
    pico.out_jetsys_m() = jetsys_v4.M();
    if (njet_nob>0) {
      pico.out_jetsys_nob_pt() = jetsys_nob_v4.Pt();
      pico.out_jetsys_nob_eta() = jetsys_nob_v4.Eta();
      pico.out_jetsys_nob_phi() = jetsys_nob_v4.Phi();
      pico.out_jetsys_nob_m() = jetsys_nob_v4.M();
    }
  }
  return;
}
