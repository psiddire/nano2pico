#include "jet_producer.hpp"

#include <algorithm>
#include <iomanip> 

#include "TLorentzVector.h"

#include <fastjet/JetDefinition.hh>
#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>

#include "utilities.hpp"

#include <iostream>

using namespace std;

JetProducer::JetProducer(int year_, float min_jet_pt_, float max_jet_eta_, bool verbose_){
  year = year_;
  verbose = verbose_;
  min_jet_pt = min_jet_pt_;
  max_jet_eta = max_jet_eta_;
}

JetProducer::~JetProducer(){
}

vector<int> JetProducer::WriteJets(nano_tree &nano, pico_tree &pico,
                                   vector<int> jet_islep_nano_idx, vector<int> jet_isphoton_nano_idx,
                                   const vector<float> &btag_wpts, const vector<float> &btag_df_wpts){
  vector<int> sig_jet_nano_idx;
  pico.out_njet() = 0; pico.out_ht() = 0; pico.out_ht5() = 0; 
  pico.out_nbl() = 0; pico.out_nbm() = 0; pico.out_nbt() = 0; 
  pico.out_nbdfl() = 0; pico.out_nbdfm() = 0; pico.out_nbdft() = 0; 
  
  // calculate MHT; needed when saving jet info
  TLorentzVector mht_vec;
  for(int ijet(0); ijet<nano.nJet(); ++ijet){
    if (nano.Jet_pt()[ijet] <= min_jet_pt) continue;
    TLorentzVector ijet_v4;
    ijet_v4.SetPtEtaPhiM(nano.Jet_pt()[ijet], nano.Jet_eta()[ijet], nano.Jet_phi()[ijet], nano.Jet_mass()[ijet]);
    mht_vec -= ijet_v4;
  }
  pico.out_mht() = mht_vec.Pt();
  pico.out_mht_phi() = mht_vec.Phi();

  // saving jet info on all jets passing pt cut, including endcap
  for(int ijet(0); ijet<nano.nJet(); ++ijet){
    if (verbose) cout<<"Jet "<<ijet<<": pt = "<<setw(10)<<nano.Jet_pt()[ijet]
                                    <<" eta = "<<setw(10)<<nano.Jet_eta()[ijet]
                                    <<" phi = "<<setw(10)<<nano.Jet_phi()[ijet]
                                    <<" m = "<<setw(10)<<nano.Jet_mass()[ijet]
                                    <<endl;
    if (nano.Jet_pt()[ijet] <= min_jet_pt) continue;

    // check overlap with signal leptons (or photons)
    bool islep = find(jet_islep_nano_idx.begin(), jet_islep_nano_idx.end(), ijet) != jet_islep_nano_idx.end();
    // N.B. photon collection is not filled for Higgsino analysis, so there is no overlap removal!
    bool isphoton = find(jet_isphoton_nano_idx.begin(), jet_isphoton_nano_idx.end(), ijet) != jet_isphoton_nano_idx.end();

    bool isgood = !islep && !isphoton && (fabs(nano.Jet_eta()[ijet]) <= max_jet_eta);

    pico.out_jet_pt().push_back(nano.Jet_pt()[ijet]);
    pico.out_jet_eta().push_back(nano.Jet_eta()[ijet]);
    pico.out_jet_phi().push_back(nano.Jet_phi()[ijet]);
    pico.out_jet_m().push_back(nano.Jet_mass()[ijet]);
    pico.out_jet_breg_corr().push_back(nano.Jet_bRegCorr()[ijet]);
    pico.out_jet_breg_res().push_back(nano.Jet_bRegRes()[ijet]);
    pico.out_jet_deepcsv().push_back(nano.Jet_btagDeepB()[ijet]);
    pico.out_jet_deepflav().push_back(nano.Jet_btagDeepFlavB()[ijet]);
    pico.out_jet_qgl().push_back(nano.Jet_qgl()[ijet]);
    pico.out_jet_islep().push_back(islep);
    pico.out_jet_isphoton().push_back(isphoton);
    pico.out_jet_isgood().push_back(isgood);
    pico.out_jet_id().push_back(nano.Jet_jetId()[ijet]);
    pico.out_jet_mht_dphi().push_back(DeltaPhi(nano.Jet_phi()[ijet], mht_vec.Phi()));
    pico.out_jet_met_dphi().push_back(DeltaPhi(nano.Jet_phi()[ijet], nano.MET_phi()));

    if (!isData) {
	    pico.out_jet_hflavor().push_back(nano.Jet_hadronFlavour()[ijet]);
	    pico.out_jet_pflavor().push_back(nano.Jet_partonFlavour()[ijet]);
    }
    
    // will be overwritten with the overlapping fat jet index, if such exists, in WriteFatJets
    pico.out_jet_fjet_idx().push_back(-999);

    //the jets for the higgs pair with smallest dm will be set to true in hig_producer
    pico.out_jet_h1d().push_back(false);
    pico.out_jet_h2d().push_back(false);


    if (!islep && !isphoton) pico.out_ht5() += nano.Jet_pt()[ijet];

    if (isgood) {
      sig_jet_nano_idx.push_back(ijet);
      pico.out_njet()++;
      pico.out_ht() += nano.Jet_pt()[ijet];
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

  if (verbose) cout<<"Done with AK4 jets"<<endl;
  return sig_jet_nano_idx;
}

void JetProducer::WriteFatJets(nano_tree &nano, pico_tree &pico){
  pico.out_nfjet() = 0; 
  for(int ifjet(0); ifjet<nano.nFatJet(); ++ifjet){
    if (verbose) cout<<"FatJet "<<ifjet<<": pt = "<<setw(10)<<nano.FatJet_pt()[ifjet]
                                       <<" eta = "<<setw(10)<<nano.FatJet_eta()[ifjet]
                                       <<" phi = "<<setw(10)<<nano.FatJet_phi()[ifjet]
                                       <<" m = "<<setw(10)<<nano.FatJet_mass()[ifjet]
                                       <<endl;

    pico.out_fjet_pt().push_back(nano.FatJet_pt()[ifjet]);
    pico.out_fjet_eta().push_back(nano.FatJet_eta()[ifjet]);
    pico.out_fjet_phi().push_back(nano.FatJet_phi()[ifjet]);
    pico.out_fjet_m().push_back(nano.FatJet_mass()[ifjet]);
    pico.out_fjet_msoftdrop().push_back(nano.FatJet_msoftdrop()[ifjet]);
    // Mass-decorrelated Deep Double B, H->bb vs QCD discriminator, endorsed by BTV
    pico.out_fjet_deep_md_hbb_btv().push_back(nano.FatJet_btagDDBvL()[ifjet]);
    pico.out_fjet_mva_hbb_btv().push_back(nano.FatJet_btagHbb()[ifjet]);
    // Mass-decorrelated DeepAk8, H->bb vs QCD discriminator, endorsed by JME
    pico.out_fjet_deep_md_hbb_jme().push_back(nano.FatJet_deepTagMD_HbbvsQCD()[ifjet]);

    pico.out_fjet_subjet_idx1().push_back(nano.FatJet_subJetIdx1()[ifjet]);
    pico.out_fjet_subjet_idx2().push_back(nano.FatJet_subJetIdx2()[ifjet]);

    for(unsigned ijet(0); ijet<pico.out_jet_pt().size(); ++ijet){
      if (dR(pico.out_jet_eta()[ijet], nano.FatJet_eta()[ifjet], pico.out_jet_phi()[ijet], nano.FatJet_phi()[ifjet])<0.8)
        pico.out_jet_fjet_idx()[ijet] = ifjet;
    }

    pico.out_nfjet()++;
  }

  //calculate sum of R 1.4 jet masses
  vector<fastjet::PseudoJet> skinny_jets(0);
  //loop over Ak4 jets to add to skinny_jets vector
  for (unsigned int jet_idx(0); jet_idx < pico.out_jet_pt().size(); jet_idx++) {
	//currently includes leptons and photons in clustering
	if (pico.out_jet_pt()[jet_idx]> min_jet_pt || pico.out_jet_islep()[jet_idx] || pico.out_jet_isphoton()[jet_idx]) {
		TLorentzVector jet_4p;
		jet_4p.SetPtEtaPhiM(pico.out_jet_pt()[jet_idx],pico.out_jet_eta()[jet_idx],pico.out_jet_phi()[jet_idx],pico.out_jet_m()[jet_idx]);
		const fastjet::PseudoJet this_pseudo_jet(jet_4p.Px(), jet_4p.Py(), jet_4p.Pz(), jet_4p.E());
		skinny_jets.push_back(this_pseudo_jet);
	}
  } //loop over Ak4 Jets
  //cluster into R 1.4 jets with fastjet
  fastjet::JetDefinition jet_definition(fastjet::antikt_algorithm, 1.4);
  fastjet::ClusterSequence cluster_sequence(skinny_jets, jet_definition);
  vector<fastjet::PseudoJet> fat_jets = cluster_sequence.inclusive_jets();
  pico.out_mj14() = 0;
  for (unsigned int fat_jet_idx = 0; fat_jet_idx < fat_jets.size(); fat_jet_idx++) {
	  pico.out_mj14() += fat_jets[fat_jet_idx].m();
  } //loop over Ak14 jets

  if (verbose) cout<<"Done with fat jets"<<endl;
  return;
}

void JetProducer::WriteSubJets(nano_tree &nano, pico_tree &pico){
  pico.out_nsubfjet() = 0; 
  set<int> matched_ak4_jets;
  for(int isubj(0); isubj<nano.nSubJet(); ++isubj){
    pico.out_subfjet_pt().push_back(nano.SubJet_pt()[isubj]);
    pico.out_subfjet_eta().push_back(nano.SubJet_eta()[isubj]);
    pico.out_subfjet_phi().push_back(nano.SubJet_phi()[isubj]);
    pico.out_subfjet_m().push_back(nano.SubJet_mass()[isubj]);

    pico.out_subfjet_deepcsv().push_back(nano.SubJet_btagDeepB()[isubj]);
    pico.out_subfjet_raw_factor().push_back(nano.SubJet_rawFactor()[isubj]);
    pico.out_subfjet_tau1().push_back(nano.SubJet_tau1()[isubj]);
    pico.out_subfjet_tau2().push_back(nano.SubJet_tau2()[isubj]);
    pico.out_subfjet_tau3().push_back(nano.SubJet_tau3()[isubj]);
    pico.out_subfjet_tau4().push_back(nano.SubJet_tau4()[isubj]);

    //match to highest pT ak4 jet that has not already been matched to a previous subjet
    float mindr(999.); int closest_jet(-1);
    for(unsigned ijet(0); ijet<pico.out_jet_pt().size(); ++ijet){
      float idr = dR(pico.out_jet_eta()[ijet], nano.SubJet_eta()[isubj], pico.out_jet_phi()[ijet], nano.SubJet_phi()[isubj]);
      if (idr<mindr){
        mindr = idr;
        if (mindr<0.4 && matched_ak4_jets.find(ijet)==matched_ak4_jets.end()) {
          closest_jet = ijet;
          matched_ak4_jets.insert(ijet);
          break; 
        }
      }
    }
    pico.out_subfjet_jet_idx().push_back(closest_jet);

    if (verbose) cout<<"SubJet "<<isubj<<": pt = "<<setw(10)<<nano.SubJet_pt()[isubj]
                                       <<" eta = "<<setw(10)<<nano.SubJet_eta()[isubj]
                                       <<" phi = "<<setw(10)<<nano.SubJet_phi()[isubj]
                                       <<" m = "<<setw(10)<<nano.SubJet_mass()[isubj]
                                       <<" ijet = "<<closest_jet<<endl;
    pico.out_nsubfjet()++;
  }
  if (verbose) cout<<"Done with subjets"<<endl;
  return;
}

void JetProducer::WriteJetSystemPt(nano_tree &nano, pico_tree &pico, 
                                   vector<int> &sig_jet_nano_idx, const float &btag_wpt) {

  TLorentzVector jetsys_v4, jetsys_nob_v4;
  int njet_nob(0);
  for (auto &idx: sig_jet_nano_idx) {
    TLorentzVector ijet_v4;
    ijet_v4.SetPtEtaPhiM(nano.Jet_pt()[idx], nano.Jet_eta()[idx], nano.Jet_phi()[idx], nano.Jet_mass()[idx]);
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
