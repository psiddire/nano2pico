#include "dilep_producer.hpp"

#include "utilities.hpp"

#include "TLorentzVector.h"

using namespace std;

DileptonProducer::DileptonProducer(int year_){
    year = year_;
}

DileptonProducer::~DileptonProducer(){
}

void DileptonProducer::WriteDileptons(pico_tree &pico, 
                                      std::vector<int> sig_el_pico_idx, std::vector<int> sig_mu_pico_idx) {
  pico.out_nll() = 0;
  if (pico.out_nmu() < 2 && pico.out_nel() < 2) return;
  double mindm(999), zmass(91.1876);
  int nll(0), shift(0);

  if (pico.out_nmu() >= 2)
    for(size_t i(0); i < sig_mu_pico_idx.size(); i++) 
      for(size_t j(i+1); j < sig_mu_pico_idx.size(); j++) {
        int imu1 = sig_mu_pico_idx.at(i);
        int imu2 = sig_mu_pico_idx.at(j);
        TLorentzVector mu1, mu2, dimu;

	if ((pico.out_mu_charge()[imu1]+pico.out_mu_charge()[imu2])!=0)
	  continue;
        mu1.SetPtEtaPhiM(pico.out_mu_pt()[imu1], pico.out_mu_eta()[imu1],
                         pico.out_mu_phi()[imu1],0.10566);
        mu2.SetPtEtaPhiM(pico.out_mu_pt()[imu2], pico.out_mu_eta()[imu2],
                         pico.out_mu_phi()[imu2],0.10566);
        dimu = mu1 + mu2;
	if (dimu.M() < 50)
	  continue;
        // Dilepton closest to Z mass gets put at the front
        if(abs(dimu.M() - zmass) < mindm) { 
          mindm = abs(dimu.M() - zmass);
	  shift = 0;
	}
	else
	  shift = nll;
	pico.out_nll()++;
	pico.out_ll_pt()   .insert(pico.out_ll_pt()   .begin()+shift, dimu.Pt());
	pico.out_ll_eta()  .insert(pico.out_ll_eta()  .begin()+shift, dimu.Eta());
	pico.out_ll_phi()  .insert(pico.out_ll_phi()  .begin()+shift, dimu.Phi());
	pico.out_ll_m()    .insert(pico.out_ll_m()    .begin()+shift, dimu.M());
	pico.out_ll_dr()   .insert(pico.out_ll_dr()   .begin()+shift, mu1.DeltaR(mu2));
	pico.out_ll_dphi() .insert(pico.out_ll_dphi() .begin()+shift, fabs(mu1.DeltaPhi(mu2)));
	pico.out_ll_deta() .insert(pico.out_ll_deta() .begin()+shift, fabs(mu1.Eta()-mu2.Eta()));
	pico.out_ll_lepid().insert(pico.out_ll_lepid().begin()+shift, 13);
	pico.out_ll_i1()   .insert(pico.out_ll_i1()   .begin()+shift, imu1);
	pico.out_ll_i2()   .insert(pico.out_ll_i2()   .begin()+shift, imu2);
	nll++;
      }
  if (pico.out_nel() >= 2)
    for(size_t i(0); i < sig_el_pico_idx.size(); i++) 
      for(size_t j(i+1); j < sig_el_pico_idx.size(); j++) {
        int iel1 = sig_el_pico_idx.at(i);
        int iel2 = sig_el_pico_idx.at(j);
        TLorentzVector el1, el2, diel;

        if ((pico.out_el_charge()[iel1]+pico.out_el_charge()[iel2])!=0)
          continue;
	el1.SetPtEtaPhiM(pico.out_el_pt()[iel1] ,pico.out_el_eta()[iel1],
                         pico.out_el_phi()[iel1],0.000511);
        el2.SetPtEtaPhiM(pico.out_el_pt()[iel2], pico.out_el_eta()[iel2],
                         pico.out_el_phi()[iel2],0.000511);
        diel = el1 + el2;
        if (diel.M() < 50)
          continue;
	// Dilepton closest to Z mass gets put at the front
        if(abs(diel.M() - zmass) < mindm) {
          mindm = abs(diel.M() - zmass);
          shift = 0;
        }
        else
          shift = nll;
	pico.out_nll()++;
	pico.out_ll_pt()   .insert(pico.out_ll_pt()   .begin()+shift, diel.Pt());
	pico.out_ll_eta()  .insert(pico.out_ll_eta()  .begin()+shift, diel.Eta());
	pico.out_ll_phi()  .insert(pico.out_ll_phi()  .begin()+shift, diel.Phi());
	pico.out_ll_m()    .insert(pico.out_ll_m()    .begin()+shift, diel.M());
	pico.out_ll_dr()   .insert(pico.out_ll_dr()   .begin()+shift, el1.DeltaR(el2));
	pico.out_ll_dphi() .insert(pico.out_ll_dphi() .begin()+shift, fabs(el1.DeltaPhi(el2)));
	pico.out_ll_deta() .insert(pico.out_ll_deta() .begin()+shift, fabs(el1.Eta()-el2.Eta()));
	pico.out_ll_lepid().insert(pico.out_ll_lepid().begin()+shift, 11);
	pico.out_ll_i1()   .insert(pico.out_ll_i1()   .begin()+shift, iel1);
	pico.out_ll_i2()   .insert(pico.out_ll_i2()   .begin()+shift, iel2);
	nll++;
      }
  return;
}
