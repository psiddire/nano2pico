#include "el_producer.hpp"

#include "utilities.hpp"

#include<iomanip>

using namespace std;

ElectronProducer::ElectronProducer(int year_, bool isData_){
    year = year_;
    isData = isData_;
}

ElectronProducer::~ElectronProducer(){
}

vector<int> ElectronProducer::WriteElectrons(nano_tree &nano, pico_tree &pico, vector<int> &jet_islep_nano_idx, vector<int> &jet_isvlep_nano_idx, vector<int> &sig_el_pico_idx, bool isFastsim){
  vector<float> Jet_pt, Jet_mass;
  getJetWithJEC(nano, isFastsim, Jet_pt, Jet_mass);

  vector<int> sig_el_nano_idx;
  pico.out_nel() = 0; pico.out_nvel() = 0;
  int pico_idx = 0;
  int nelectrons(0), ndr(0), shift(0);

  for(int iel(0); iel < nano.nElectron(); ++iel){
    float pt = nano.Electron_pt()[iel];
    float eta = nano.Electron_eta()[iel];
    float etasc = nano.Electron_eta()[iel] + nano.Electron_deltaEtaSC()[iel];
    float phi = nano.Electron_phi()[iel];
    float dz = nano.Electron_dz()[iel];
    float dxy = nano.Electron_dxy()[iel];
    double mva = nano.Electron_mvaFall17V2Iso()[iel];
    float sip3d = nano.Electron_sip3d()[iel];
    float iso = nano.Electron_pfRelIso03_all()[iel];
    bool isSignal = false;
    bool id80 = nano.Electron_mvaFall17V2Iso_WP80()[iel];
    bool id90 = nano.Electron_mvaFall17V2Iso_WP90()[iel];
    bool idLoose = nano.Electron_mvaFall17V2Iso_WPL()[iel];

    if (pt <= ElectronPtCut) continue;
    if (fabs(etasc) > ElectronEtaCut) continue;
    if (fabs(dz) > ElectronDzCut)  continue;
    if (fabs(dxy) > ElectronDxyCut) continue;

    if (idLoose) isSignal = true;

    // electrons passing signal cuts get put at the very front
    if(isSignal) {
      shift = ndr;
      ndr++;
    }
    else
      shift = nelectrons;
    pico.out_el_pt()       .insert(pico.out_el_pt()       .begin()+shift, pt);
    pico.out_el_eta()      .insert(pico.out_el_eta()      .begin()+shift, eta);
    pico.out_el_etasc()    .insert(pico.out_el_etasc()    .begin()+shift, etasc);
    pico.out_el_phi()      .insert(pico.out_el_phi()      .begin()+shift, phi);
    pico.out_el_energyErr().insert(pico.out_el_energyErr().begin()+shift, nano.Electron_energyErr()[iel]);
    pico.out_el_dz()       .insert(pico.out_el_dz()       .begin()+shift, dz);
    pico.out_el_dxy()      .insert(pico.out_el_dxy()      .begin()+shift, dxy);
    pico.out_el_miniso()   .insert(pico.out_el_miniso()   .begin()+shift, nano.Electron_miniPFRelIso_all()[iel]);
    pico.out_el_etPt()     .insert(pico.out_el_etPt()     .begin()+shift, nano.Electron_scEtOverPt()[iel]);
    pico.out_el_eminusp()  .insert(pico.out_el_eminusp()  .begin()+shift, nano.Electron_eInvMinusPInv()[iel]);
    pico.out_el_phidx()    .insert(pico.out_el_phidx()    .begin()+shift, nano.Electron_photonIdx()[iel]);
    pico.out_el_reliso()   .insert(pico.out_el_reliso()   .begin()+shift, iso);
    pico.out_el_idmva()    .insert(pico.out_el_idmva()    .begin()+shift, mva);
    pico.out_el_sip3d()    .insert(pico.out_el_sip3d()    .begin()+shift, sip3d);
    pico.out_el_ip3d()     .insert(pico.out_el_ip3d()     .begin()+shift, nano.Electron_ip3d()[iel]);
    pico.out_el_id80()     .insert(pico.out_el_id80()     .begin()+shift, id80);
    pico.out_el_id90()     .insert(pico.out_el_id90()     .begin()+shift, id90);
    pico.out_el_idLoose()  .insert(pico.out_el_idLoose()  .begin()+shift, idLoose);
    pico.out_el_sig()      .insert(pico.out_el_sig()      .begin()+shift, isSignal);
    pico.out_el_ispf()     .insert(pico.out_el_ispf()     .begin()+shift, nano.Electron_isPFcand()[iel]);
    pico.out_el_charge()   .insert(pico.out_el_charge()   .begin()+shift, nano.Electron_charge()[iel]);
    nelectrons++;
    if (!isData)
      pico.out_el_pflavor().insert(pico.out_el_pflavor().begin()+shift, nano.Electron_genPartFlav()[iel]);

    // signal electron
    if (isSignal) {
      pico.out_nel()++;
      pico.out_nlep()++;
      sig_el_nano_idx.push_back(iel);
      sig_el_pico_idx.push_back(pico_idx);
      // save indices of matching jets
      for (int ijet(0); ijet<nano.nJet(); ijet++) {
        if (dR(nano.Electron_eta()[iel], nano.Jet_eta()[ijet], nano.Electron_phi()[iel], nano.Jet_phi()[ijet])<0.4 &&
            fabs(Jet_pt[ijet] - nano.Electron_pt()[iel])/nano.Electron_pt()[iel] < 1)
          jet_islep_nano_idx.push_back(ijet);
      }
    }
    // veto electron selection
    else {
      pico.out_nvel()++;
      pico.out_nvlep()++;
      // save indices of matching jets
      for (int ijet(0); ijet<nano.nJet(); ijet++) {
        if (dR(nano.Electron_eta()[iel], nano.Jet_eta()[ijet], nano.Electron_phi()[iel], nano.Jet_phi()[ijet])<0.4 &&
            fabs(Jet_pt[ijet] - nano.Electron_pt()[iel])/nano.Electron_pt()[iel] < 1)
          jet_isvlep_nano_idx.push_back(ijet);
      }
    }
   
    pico_idx++;
  }
  return sig_el_nano_idx;
}

