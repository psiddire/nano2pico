#include <vector>
#include <string>
#include <iostream>
#include <getopt.h>

#include "corrections_tree.hpp"
#include "cross_sections.hpp"
#include "utilities.hpp"

using namespace std;

template<typename T, typename U>
void CopySize(const vector<T> &wgt_sums, vector<U> &corr){
  corr = vector<U>(wgt_sums.size(), static_cast<U>(0.));
}

template<typename T, typename U>
void VecAdd(const vector<T> &wgt_sums, vector<U> &corr){
  for(size_t i = 0; i < wgt_sums.size(); ++i){
    corr.at(i) += wgt_sums.at(i);
  }
}

template<typename T>
void Normalize(T &x, double nent){
  x = x ? nent/x : 1.;
}

template<typename T>
void Normalize(vector<T> &v, double nent){
  for(auto &x: v) x = x ? nent/x : 1.;
}

void Initialize(corrections_tree &wgt_sums, corrections_tree &corr);
void AddEntry(corrections_tree &wgt_sums, corrections_tree &corr);
void FixLumi(corrections_tree &corr, const string &corr_path, int year);
void Normalize(corrections_tree &corr);
void GetOptions(int argc, char *argv[]);


int main(int argc, char *argv[]){

  GetOptions(argc, argv);

  if(argc-optind+1 < 3){
    cout << "Too few arguments! Usage: " << argv[0]
         << " output_file input_file [more_input_files...]" << endl;
    return 1;
  }
  
  string output_path = argv[optind];
  vector<string> input_paths(argv+optind+1, argv+argc);

  int year = Contains(input_paths[0], "UL16") ? 2016 : (Contains(input_paths[0], "UL17") ? 2017 : 2018);
  // int year = Contains(input_paths[0], "RunIISummer16") ? 2016 : (Contains(input_paths[0], "RunIIFall17") ? 2017 : 2018);
  cout << "Running with settings for year = "<<year<<"."<<endl; 

  corrections_tree corr("", output_path.c_str());
  corrections_tree wgt_sums(input_paths.front().c_str());
  for(size_t i = 1; i < input_paths.size(); ++i){
    wgt_sums.intree_->Add(input_paths.at(i).c_str());
  }

  size_t num_entries = wgt_sums.GetEntries();
  if(num_entries <= 0){
    cout << "No entries in input files!" << endl;
    return 1;
  } 
  wgt_sums.GetEntry(0);
  Initialize(wgt_sums, corr);

  for(size_t i = 0; i < num_entries; ++i){
    wgt_sums.GetEntry(i);
    AddEntry(wgt_sums, corr);
  }

  FixLumi(corr, output_path, year);
  Normalize(corr);

  corr.Fill();
  corr.Write();
  cout << "Wrote output to " << output_path << endl;
}


void Initialize(corrections_tree &wgt_sums, corrections_tree &corr){
  corr.out_weight() = 0.;
  corr.out_w_lumi() = 0.;
  corr.out_w_pu() = 0.;
  // w_prefire should not be normalized!!

  corr.out_neff() = 0;
  corr.out_nent() = 0;
  corr.out_nent_zlep() = 0;
  corr.out_tot_weight_l0() = 0.;
  corr.out_tot_weight_l1() = 0.;

  CopySize(wgt_sums.sys_pu(),                 corr.out_sys_pu());
  CopySize(wgt_sums.sys_murf(),               corr.out_sys_murf());
}


void AddEntry(corrections_tree &wgt_sums, corrections_tree &corr){
  corr.out_neff()          += wgt_sums.neff();
  corr.out_nent()          += wgt_sums.nent();
  corr.out_nent_zlep()     += wgt_sums.nent_zlep();
  corr.out_tot_weight_l0() += wgt_sums.tot_weight_l0();
  corr.out_tot_weight_l1() += wgt_sums.tot_weight_l1();

  corr.out_weight()        += wgt_sums.weight();
  corr.out_w_pu()          += wgt_sums.w_pu();

  VecAdd(wgt_sums.sys_pu(),            corr.out_sys_pu());
  VecAdd(wgt_sums.sys_murf(),          corr.out_sys_murf());
}


void FixLumi(corrections_tree &corr, const string &corr_path, int year){
  double xsec(0.); const float lumi = 1000.;
  xsec = xsec::crossSection(corr_path, year);  
  corr.out_w_lumi() = xsec*lumi/corr.out_neff();
}


void Normalize(corrections_tree &corr){
  double nent = corr.out_nent();
  Normalize(corr.out_w_pu(), nent);
  Normalize(corr.out_sys_pu(), nent);
  Normalize(corr.out_sys_murf(), nent);
}


void GetOptions(int argc, char *argv[]){
  while(true){
    static struct option long_options[] = {
      {0, 0, 0, 0}
    };

    char opt = -1;
    int option_index;
    opt = getopt_long(argc, argv, "", long_options, &option_index);
    if(opt == -1) break;

    string optname;
    switch(opt){
    default:
      printf("Bad option! getopt_long returned character code 0%o\n", opt);
      break;
    }
  }
}

