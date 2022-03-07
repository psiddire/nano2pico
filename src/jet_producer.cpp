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
                                   bool isFastsim){
  vector<int> sig_jet_nano_idx;
  pico.out_njet() = 0; 
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

    bool isgood = !islep && !isphoton && pass_jetid;

    if (Jet_pt[ijet] <= min_jet_pt) continue;
    if (fabs(nano.Jet_eta()[ijet]) > max_jet_eta) continue;

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
    if (!isData) {
      pico.out_jet_hflavor().push_back(nano.Jet_hadronFlavour()[ijet]);
      pico.out_jet_pflavor().push_back(nano.Jet_partonFlavour()[ijet]);
    }
    
    if (isgood) {
      sig_jet_nano_idx.push_back(ijet);
      pico.out_njet()++;
      if (nano.Jet_btagDeepB()[ijet] > btag_wpts[0]) pico.out_nbl()++; 
      if (nano.Jet_btagDeepB()[ijet] > btag_wpts[1]) pico.out_nbm()++; 
      if (nano.Jet_btagDeepB()[ijet] > btag_wpts[2]) pico.out_nbt()++;
      if (nano.Jet_btagDeepFlavB()[ijet] > btag_df_wpts[0]) pico.out_nbdfl()++; 
      if (nano.Jet_btagDeepFlavB()[ijet] > btag_df_wpts[1]) pico.out_nbdfm()++; 
      if (nano.Jet_btagDeepFlavB()[ijet] > btag_df_wpts[2]) pico.out_nbdft()++; 
    }
  } // end jet loop

  if (verbose) cout<<"Done with AK4 jets"<<endl;
  return sig_jet_nano_idx;
}
