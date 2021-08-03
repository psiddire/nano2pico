#ifndef H_LEPTON_WEIGHTER
#define H_LEPTON_WEIGHTER

#include <vector>
#include <utility>

#include "TH2D.h"
#include "TH2F.h"

#include "pico_tree.hpp"

class LeptonWeighter{
public:
  LeptonWeighter(int year, bool doGH16 = false);

  void FullSim(pico_tree &pico, float &w_lep, std::vector<float> &sys_lep);
  
private:
  std::string in_full_el_id_,  hist_full_el_id_;
  std::string in_full_el_tk_,  hist_full_el_tk_;
  std::string in_full_el_lo_,  hist_full_el_lo_;
  std::string in_full_mu_id_,  hist_full_mu_id_;
  std::string in_full_mu_is_,  hist_full_mu_is_;

  bool do_full_el_id_, do_full_el_tk_, do_full_el_lo_;
  bool do_full_mu_id_, do_full_mu_is_;

  TH2F sf_full_el_id_, sf_full_el_tk_, sf_full_el_lo_;
  TH2F sf_full_mu_id_, sf_full_mu_is_;

  std::pair<double, double> GetMuonScaleFactor(pico_tree &pico, std::size_t ilep);
  std::pair<double, double> GetElectronScaleFactor(pico_tree &pico, std::size_t ilep);

};

#endif
