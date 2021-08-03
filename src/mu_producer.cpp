#include "mu_producer.hpp"

#include "utilities.hpp"

using namespace std;

MuonProducer::MuonProducer(int year_, bool isData_){
    year = year_;
    isData = isData_;
}

MuonProducer::~MuonProducer(){
}

vector<int> MuonProducer::WriteMuons(nano_tree &nano, pico_tree &pico, vector<int> &jet_islep_nano_idx, vector<int> &jet_isvlep_nano_idx, vector<int> &sig_mu_pico_idx, bool isFastsim){
  vector<float> Jet_pt, Jet_mass;
  getJetWithJEC(nano, isFastsim, Jet_pt, Jet_mass);

  vector<int> sig_mu_nano_idx;
  pico.out_nmu() = 0; pico.out_nvmu() = 0;
  int pico_idx = 0;
  int nmuons(0), ndr(0), shift(0);

  for(int imu(0); imu<nano.nMuon(); ++imu){

    float pt = nano.Muon_pt()[imu];
    float eta = nano.Muon_eta()[imu];
    float phi = nano.Muon_phi()[imu];
    float dz = nano.Muon_dz()[imu];
    float dxy = nano.Muon_dxy()[imu];
    float sip3d = nano.Muon_sip3d()[imu];
    bool looseid = nano.Muon_looseId()[imu];
    bool mediumid = nano.Muon_mediumId()[imu];
    bool tightid = nano.Muon_tightId()[imu];
    bool isSignal = false;

    if (pt <= MuonPtCut) continue;
    if (fabs(eta) > MuonEtaCut) continue;
    if (fabs(dz) > MuonDzCut) continue;
    if (fabs(dxy) > MuonDxyCut) continue;
    if ((looseid || (pt > 200 && nano.Muon_highPtId()[imu])) &&
	nano.Muon_pfRelIso03_all()[imu] < MuonRelIsoCut && sip3d < 4)
      isSignal = true;

    // muons passing signal cuts get put at the very front
    if(isSignal) {
      shift = ndr;
      ndr++;
    }
    else
      shift = nmuons;
    pico.out_mu_pt()         .insert(pico.out_mu_pt()         .begin()+shift, pt);
    pico.out_mu_eta()        .insert(pico.out_mu_eta()        .begin()+shift, eta);
    pico.out_mu_phi()        .insert(pico.out_mu_phi()        .begin()+shift, phi);
    pico.out_mu_dz()         .insert(pico.out_mu_dz()         .begin()+shift, dz);
    pico.out_mu_dxy()        .insert(pico.out_mu_dxy()        .begin()+shift, dxy);
    pico.out_mu_miniso()     .insert(pico.out_mu_miniso()     .begin()+shift, nano.Muon_miniPFRelIso_all()[imu]);
    pico.out_mu_reliso()     .insert(pico.out_mu_reliso()     .begin()+shift, nano.Muon_pfRelIso03_all()[imu]);
    pico.out_mu_sip3d()      .insert(pico.out_mu_sip3d()      .begin()+shift, sip3d);
    pico.out_mu_ip3d()       .insert(pico.out_mu_ip3d()       .begin()+shift, nano.Muon_ip3d()[imu]);
    pico.out_mu_looseid()    .insert(pico.out_mu_looseid()    .begin()+shift, looseid);
    pico.out_mu_mediumid()   .insert(pico.out_mu_mediumid()   .begin()+shift, mediumid);
    pico.out_mu_tightid()    .insert(pico.out_mu_tightid()    .begin()+shift, tightid);
    pico.out_mu_highptid()   .insert(pico.out_mu_highptid()   .begin()+shift, nano.Muon_highPtId()[imu]);
    pico.out_mu_sig()        .insert(pico.out_mu_sig()        .begin()+shift, isSignal);
    pico.out_mu_charge()     .insert(pico.out_mu_charge()     .begin()+shift, nano.Muon_charge()[imu]);
    pico.out_mu_fsrphotonid().insert(pico.out_mu_fsrphotonid().begin()+shift, nano.Muon_fsrPhotonIdx()[imu]);
    nmuons++;
    if (!isData)
      pico.out_mu_pflavor().insert(pico.out_mu_pflavor().begin()+shift, nano.Muon_genPartFlav()[imu]);

    // signal muon
    if (isSignal) {
      pico.out_nmu()++;
      pico.out_nlep()++;
      sig_mu_nano_idx.push_back(imu);
      sig_mu_pico_idx.push_back(pico_idx);
      // save indices of matching jets
      for (int ijet(0); ijet<nano.nJet(); ijet++) {
        if (dR(nano.Muon_eta()[imu], nano.Jet_eta()[ijet], nano.Muon_phi()[imu], nano.Jet_phi()[ijet])<0.4 &&
          fabs(Jet_pt[ijet] - nano.Muon_pt()[imu])/nano.Muon_pt()[imu] < 1)
          jet_islep_nano_idx.push_back(ijet);
      }
    }
    // veto muon
    else {
      pico.out_nvmu()++;
      pico.out_nvlep()++;
      // save indices of matching jets
      for (int ijet(0); ijet<nano.nJet(); ijet++) {
        if (dR(nano.Muon_eta()[imu], nano.Jet_eta()[ijet], nano.Muon_phi()[imu], nano.Jet_phi()[ijet])<0.4 &&
          fabs(Jet_pt[ijet] - nano.Muon_pt()[imu])/nano.Muon_pt()[imu] < 1)
          jet_isvlep_nano_idx.push_back(ijet);
      }
    }
    pico_idx++;
  }
  return sig_mu_nano_idx;
}
