#include "photon_producer.hpp"

#include "utilities.hpp"

using namespace std;

PhotonProducer::PhotonProducer(int year_, bool isData_){
    year = year_;
    isData = isData_;
}

PhotonProducer::~PhotonProducer(){
}

vector<int> PhotonProducer::WritePhotons(nano_tree &nano, pico_tree &pico, vector<int> &jet_isphoton_nano_idx,
                                         vector<int> &sig_el_nano_idx, vector<int> &sig_mu_nano_idx){
  pico.out_nphoton() = 0;
  pico.out_nfsrphoton() = 0;
  vector<int> sig_photon_nano_idx;
  int nphotons(0), ndr(0), shift(0);
  bool isSignal = false;

  for(int iph(0); iph < nano.nFsrPhoton(); ++iph){
    if (nano.FsrPhoton_pt()[iph] <= FsrPhotonPtCut) continue;
    if (fabs(nano.FsrPhoton_eta()[iph]) > FsrPhotonEtaCut) continue;
    if (nano.FsrPhoton_relIso03()[iph] > FsrPhotonIsoCut) continue;
    if (nano.FsrPhoton_dROverEt2()[iph] > FsrPhotondRCut) continue;
    pico.out_fsrphoton_pt().push_back(nano.FsrPhoton_pt()[iph]);
    pico.out_fsrphoton_eta().push_back(nano.FsrPhoton_eta()[iph]);
    pico.out_fsrphoton_phi().push_back(nano.FsrPhoton_phi()[iph]);
    pico.out_fsrphoton_reliso().push_back(nano.FsrPhoton_relIso03()[iph]);
    pico.out_fsrphoton_muonidx().push_back(nano.FsrPhoton_muonIdx()[iph]);
    pico.out_fsrphoton_droveret2().push_back(nano.FsrPhoton_dROverEt2()[iph]);
    pico.out_nfsrphoton()++;
  }

  for(int iph(0); iph < nano.nPhoton(); ++iph){
    float pt = nano.Photon_pt()[iph];
    float eta = nano.Photon_eta()[iph];
    float phi = nano.Photon_phi()[iph];
    float mva = nano.Photon_mvaID()[iph];
    bool isEB = nano.Photon_isScEtaEB()[iph];
    bool isEE = nano.Photon_isScEtaEE()[iph];
    bool eVeto = nano.Photon_electronVeto()[iph];
    bool isSignal = false;

    if (pt <= PhotonPtCut) continue;
    if (fabs(eta) > PhotonEtaCut) continue;
    if (!(isEB || isEE)) continue;

    // Find min(dR) between photon and signal lepton
    double minLepDR(999.);
    for(size_t iel(0); iel < sig_el_nano_idx.size(); iel++) {
      double tempDR = dR(eta, nano.Electron_eta()[sig_el_nano_idx.at(iel)],
                         phi, nano.Electron_phi()[sig_el_nano_idx.at(iel)]);
      if(tempDR < minLepDR) minLepDR = tempDR;
    }
    for(size_t imu(0); imu < sig_mu_nano_idx.size(); imu++) {
      double tempDR = dR(eta, nano.Muon_eta()[sig_mu_nano_idx.at(imu)],
                         phi, nano.Muon_phi()[sig_mu_nano_idx.at(imu)]);
      if(tempDR < minLepDR) minLepDR = tempDR;
    }

    if (((fabs(eta) < 1.4442 && mva > -0.4) || (fabs(eta) > 1.566 && fabs(eta) < 2.5 && mva > -0.58)) && eVeto && minLepDR > 0.4)
      isSignal = true;

    // photons passing signal cuts get put at the very front
    if(isSignal) { 
      shift = ndr;
      ndr++;
    }
    else
      shift = nphotons;
    pico.out_photon_pt()    .insert(pico.out_photon_pt()    .begin()+shift, pt);
    pico.out_photon_eta()   .insert(pico.out_photon_eta()   .begin()+shift, eta);
    pico.out_photon_phi()   .insert(pico.out_photon_phi()   .begin()+shift, phi);
    pico.out_photon_reliso().insert(pico.out_photon_reliso().begin()+shift, nano.Photon_pfRelIso03_all()[iph]);
    pico.out_photon_r9()    .insert(pico.out_photon_r9()    .begin()+shift, nano.Photon_r9()[iph]);
    pico.out_photon_pterr() .insert(pico.out_photon_pterr() .begin()+shift, nano.Photon_energyErr()[iph]);
    pico.out_photon_hoe()   .insert(pico.out_photon_hoe()   .begin()+shift, nano.Photon_hoe()[iph]);
    pico.out_photon_elveto().insert(pico.out_photon_elveto().begin()+shift, eVeto);
    pico.out_photon_id()    .insert(pico.out_photon_id()    .begin()+shift, nano.Photon_mvaID_WP90()[iph]);
    pico.out_photon_idmva() .insert(pico.out_photon_idmva() .begin()+shift, mva);
    pico.out_photon_sig()   .insert(pico.out_photon_sig()   .begin()+shift, isSignal);
    pico.out_photon_drmin() .insert(pico.out_photon_drmin() .begin()+shift, minLepDR);
    nphotons++;
    if (!isData)
      pico.out_photon_pflavor().insert(pico.out_photon_pflavor().begin()+shift, nano.Photon_genPartFlav()[iph]);

    // signal photon
    if(isSignal) {
      pico.out_nphoton()++;
      sig_photon_nano_idx.push_back(iph);
      // save indices of matching jets
      if (nano.Photon_jetIdx()[iph]>=0)
        jet_isphoton_nano_idx.push_back(nano.Photon_jetIdx()[iph]);
      else
        for (int ijet(0); ijet<nano.nJet(); ijet++)
          if (dR(nano.Photon_eta()[iph], nano.Jet_eta()[ijet], nano.Photon_phi()[iph], nano.Jet_phi()[ijet])<0.4)
            jet_isphoton_nano_idx.push_back(ijet);
    }
  }
  return sig_photon_nano_idx;
}
