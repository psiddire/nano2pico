#include "lepton_weighter.hpp"

#include <string>
#include <numeric>

#include "TFile.h"
#include "TGraphAsymmErrors.h"

#include "utilities.hpp"

using namespace std;

namespace{
  template<typename T>
    T LoadSF(const string &file_name, const string &item_name){
    string path = "data/"+file_name;
    TFile f(path.c_str(), "read");
    if(!f.IsOpen()) ERROR("Could not open "+file_name);
    T* item = static_cast<T*>(f.Get(item_name.c_str()));
    if(!item) ERROR("Could not find "+item_name+" in "+file_name);
    return *item;
  }
  template<typename T>
    pair<double, double> GetSF(const T &h, double x, double y, bool ignore_error = false){
    pair<double, double> sf;
    auto bin = h.FindFixBin(x, y);
    if((h.IsBinOverflow(bin) || h.IsBinUnderflow(bin))
       && h.GetBinContent(bin) == 0. && h.GetBinError(bin) == 0.){
      auto bin_x = h.GetXaxis()->FindFixBin(x);
      auto bin_y = h.GetYaxis()->FindFixBin(y);
      if(bin_x <= 0) bin_x = 1;
      if(bin_x > h.GetNbinsX()) bin_x = h.GetNbinsX();
      if(bin_y <= 0) bin_y = 1;
      if(bin_y > h.GetNbinsY()) bin_y = h.GetNbinsY();
      sf = {h.GetBinContent(bin_x, bin_y), h.GetBinError(bin_x, bin_y)};
    }else{
      sf = {h.GetBinContent(bin), h.GetBinError(bin)};
    }
    if(ignore_error) sf.second = 0.;
    return sf;
  }

  pair<double, double> MergeSF(pair<double, double> a,
                               pair<double, double> b){
    double sf = a.first * b.first;
    double err = hypot(a.first*b.second, b.first*a.second);
    return {sf, err};
  }
}

LeptonWeighter::LeptonWeighter(int year, bool doGH16){
  if(year == 2016) {
    if(doGH16) {
      in_full_mu_id_ = "zgamma/RunGH_SF_ID.root";                           hist_full_mu_id_ = "NUM_LooseID_DEN_genTracks_eta_pt";
      in_full_mu_is_ = "zgamma/RunGH_SF_ISO.root";                          hist_full_mu_is_ = "NUM_LooseRelIso_DEN_LooseID_eta_pt";
    } else {
      in_full_mu_id_ = "zgamma/RunBCDEF_SF_ID.root";                        hist_full_mu_id_ = "NUM_LooseID_DEN_genTracks_eta_pt";
      in_full_mu_is_ = "zgamma/RunBCDEF_SF_ISO.root";                       hist_full_mu_is_ = "NUM_LooseRelIso_DEN_LooseID_eta_pt";
    }
    in_full_el_id_ = "zgamma/2016LegacyReReco_ElectronMVA90_Fall17V2.root"; hist_full_el_id_ = "EGamma_SF2D";
    in_full_el_tk_ = "zgamma/EGM2D_BtoH_GT20GeV_RecoSF_Legacy2016.root";    hist_full_el_tk_ = "EGamma_SF2D";
    // For electrons with pT < 20 GeV
    in_full_el_lo_ = "zgamma/EGM2D_BtoH_low_RecoSF_Legacy2016.root";        hist_full_el_lo_ = "EGamma_SF2D";
  }

  do_full_el_id_  = (in_full_el_id_ !=""); do_full_el_tk_  = (in_full_el_tk_ !=""); do_full_el_lo_  = (in_full_el_lo_ !="");
  do_full_mu_id_  = (in_full_mu_id_ !=""); do_full_mu_is_  = (in_full_mu_is_ !=""); 

  //https://twiki.cern.ch/twiki/bin/view/CMS/EgammaRunIIRecommendations#Electron_Scale_Factors
  if (do_full_el_id_)  sf_full_el_id_  = LoadSF<TH2F>(in_full_el_id_,  hist_full_el_id_);
  if (do_full_el_tk_)  sf_full_el_tk_  = LoadSF<TH2F>(in_full_el_tk_,  hist_full_el_tk_);
  if (do_full_el_lo_)  sf_full_el_lo_  = LoadSF<TH2F>(in_full_el_lo_,  hist_full_el_lo_);

  //https://twiki.cern.ch/twiki/bin/view/CMS/MuonReferenceEffs2016LegacyRereco
  if (do_full_mu_id_)  sf_full_mu_id_  = LoadSF<TH2F>(in_full_mu_id_,  hist_full_mu_id_);
  if (do_full_mu_is_)  sf_full_mu_is_  = LoadSF<TH2F>(in_full_mu_is_,  hist_full_mu_is_);
}

void LeptonWeighter::FullSim(pico_tree &pico, float &w_lep, vector<float> &sys_lep){
  pair<double, double> sf(1., 0.);
  for(size_t i = 0; i < pico.out_mu_sig().size(); ++i){
    if(pico.out_mu_sig().at(i))
      sf = MergeSF(sf, GetMuonScaleFactor(pico, i));
  }
  for(size_t i = 0; i < pico.out_el_sig().size(); ++i){
    if(pico.out_el_sig().at(i))
      sf = MergeSF(sf, GetElectronScaleFactor(pico, i));
  }
  w_lep = sf.first;
  sys_lep = vector<float>{static_cast<float>(sf.first+sf.second),
                          static_cast<float>(sf.first-sf.second)};
}

std::pair<double, double> LeptonWeighter::GetMuonScaleFactor(pico_tree &pico, size_t imu){
  //ID, iso, tracking SFs applied
  double pt = pico.out_mu_pt().at(imu);
  double eta = pico.out_mu_eta().at(imu);
  double abseta = fabs(eta);
  vector<pair<double, double> > sfs;
  if (do_full_mu_id_) sfs.push_back(GetSF(sf_full_mu_id_, eta, pt, false));
  if (do_full_mu_is_) sfs.push_back(GetSF(sf_full_mu_is_, eta, pt, false));
  return accumulate(sfs.cbegin(), sfs.cend(), make_pair(1., 0.), MergeSF);
}

std::pair<double, double> LeptonWeighter::GetElectronScaleFactor(pico_tree &pico, size_t iel){
  //Tracking SFs from https://twiki.cern.ch/twiki/bin/view/CMS/EgammaIDRecipesRun2#Electron_efficiencies_and_scale
  //3% tracking systematic below 20 GeV
  double pt = pico.out_el_pt().at(iel);
  double eta = pico.out_el_eta().at(iel);
  // double abseta = fabs(eta);
  vector<pair<double, double> > sfs;
  //Axes swapped, asymmetric in eta
  if (do_full_el_id_)  sfs.push_back(GetSF(sf_full_el_id_,  eta, pt));
  if (do_full_el_tk_) {
    if(pt > 20)        sfs.push_back(GetSF(sf_full_el_tk_,  eta, pt));
    else               sfs.push_back(GetSF(sf_full_el_lo_,  eta, pt));
  }
  sfs.push_back(make_pair(1., pt<20. || pt >80. ? 0.01 : 0.));//Systematic uncertainty
  return accumulate(sfs.cbegin(), sfs.cend(), make_pair(1., 0.), MergeSF);
}

