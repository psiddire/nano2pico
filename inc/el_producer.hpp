#ifndef H_EL_PRODUCER
#define H_EL_PRODUCER


#include "nano_tree.hpp"
#include "pico_tree.hpp"

class ElectronProducer{
public:

  explicit ElectronProducer(int year, bool isData);
  ~ElectronProducer();

  const float ElectronPtCut      = 7.0;
  const float ElectronEtaCut     = 2.5;
  const float ElectronDxyCut     = 0.5;
  const float ElectronDzCut      = 1.0;

  std::vector<int> WriteElectrons(nano_tree &nano, pico_tree &pico, std::vector<int> &jet_islep_nano_idx, std::vector<int> &jet_isvlep_nano_idx, std::vector<int> &sig_el_pico_idx, bool isZgamma, bool isFastsim);

private:
  int year;
  bool isData;

  bool idElectron_noIso(int bitmap, int level);
  
};

#endif
