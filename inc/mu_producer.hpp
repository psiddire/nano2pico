#ifndef H_MU_PRODUCER
#define H_MU_PRODUCER

#include "nano_tree.hpp"
#include "pico_tree.hpp"

class MuonProducer{
public:

  explicit MuonProducer(int year, bool isData);
  ~MuonProducer();

  const float MuonPtCut         = 5.0;
  const float MuonEtaCut        = 2.4;
  const float MuonDxyCut        = 0.5;
  const float MuonDzCut         = 1.0;
  const float MuonRelIsoCut     = 0.35;

  std::vector<int> WriteMuons(nano_tree &nano, pico_tree &pico, std::vector<int> &jet_islep_nano_idx, std::vector<int> &jet_isvlep_nano_idx, std::vector<int> &sig_mu_pico_idx, bool isZgamma, bool isFastsim);

private:
  int year;
  bool isData;
  
};

#endif
