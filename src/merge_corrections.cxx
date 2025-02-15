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
int GetHiggsinoMass(const string &path);
int GetGluinoMass(const string &path);
int GetSUSYMass(const string & patth, const string & tag);
void FixLumi(corrections_tree &corr, const string &corr_path, int year);
void Normalize(corrections_tree &corr);
void Fix0L(corrections_tree &corr);

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

  int year = Contains(input_paths[0], "RunIISummer20UL16") ? 2016 : (Contains(input_paths[0], "RunIISummer20UL17") ? 2017 : 2018);
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
  Fix0L(corr);

  Normalize(corr);

  corr.Fill();
  corr.Write();
  cout << "Wrote output to " << output_path << endl;
}

void Initialize(corrections_tree &wgt_sums, corrections_tree &corr){
  corr.out_weight() = 0.;
  corr.out_w_lumi() = 0.;
  corr.out_w_lep() = 0.;
  corr.out_w_fs_lep() = 0.;
  corr.out_w_btag() = 0.;
  corr.out_w_btag_df() = 0.;
  corr.out_w_bhig() = 0.;
  corr.out_w_bhig_df() = 0.;
  corr.out_w_pu() = 0.;
  // w_prefire should not be normalized!!

  corr.out_neff() = 0;
  corr.out_nent() = 0;
  corr.out_nent_zlep() = 0;
  corr.out_tot_weight_l0() = 0.;
  corr.out_tot_weight_l1() = 0.;

  CopySize(wgt_sums.sys_lep(),                corr.out_sys_lep());
  CopySize(wgt_sums.sys_fs_lep(),             corr.out_sys_fs_lep());
  CopySize(wgt_sums.sys_bchig(),              corr.out_sys_bchig());
  CopySize(wgt_sums.sys_udsghig(),            corr.out_sys_udsghig());
  CopySize(wgt_sums.sys_fs_bchig(),           corr.out_sys_fs_bchig());
  CopySize(wgt_sums.sys_fs_udsghig(),         corr.out_sys_fs_udsghig());

  CopySize(wgt_sums.sys_pu(),                 corr.out_sys_pu());
  CopySize(wgt_sums.sys_murf(),               corr.out_sys_murf());
  // CopySize(wgt_sums.w_pdf(),                  corr.out_w_pdf());
  // CopySize(wgt_sums.sys_pdf(),                corr.out_sys_pdf());
}


void AddEntry(corrections_tree &wgt_sums, corrections_tree &corr){
  corr.out_neff() += wgt_sums.neff();
  corr.out_nent() += wgt_sums.nent();
  corr.out_nent_zlep() += wgt_sums.nent_zlep();
  corr.out_tot_weight_l0() += wgt_sums.tot_weight_l0();
  corr.out_tot_weight_l1() += wgt_sums.tot_weight_l1();

  corr.out_weight()            += wgt_sums.weight();
  corr.out_w_lep()             += wgt_sums.w_lep();
  corr.out_w_fs_lep()          += wgt_sums.w_fs_lep();
  corr.out_w_bhig()            += wgt_sums.w_bhig();
  corr.out_w_btag()            += wgt_sums.w_btag();
  corr.out_w_bhig_df()         += wgt_sums.w_bhig_df();
  corr.out_w_btag_df()         += wgt_sums.w_btag_df();
  corr.out_w_pu()              += wgt_sums.w_pu();

  VecAdd(wgt_sums.sys_lep(),           corr.out_sys_lep());
  VecAdd(wgt_sums.sys_fs_lep(),        corr.out_sys_fs_lep());
  VecAdd(wgt_sums.sys_bchig(),         corr.out_sys_bchig());
  VecAdd(wgt_sums.sys_udsghig(),       corr.out_sys_udsghig());
  VecAdd(wgt_sums.sys_fs_bchig(),      corr.out_sys_fs_bchig());
  VecAdd(wgt_sums.sys_fs_udsghig(),    corr.out_sys_fs_udsghig());
  VecAdd(wgt_sums.sys_pu(),            corr.out_sys_pu());
  VecAdd(wgt_sums.sys_murf(),          corr.out_sys_murf());
  // VecAdd(wgt_sums.w_pdf(),             corr.out_w_pdf());
  // VecAdd(wgt_sums.sys_pdf(),           corr.out_sys_pdf());
}

int GetHiggsinoMass(const string &path){
  string key = "_mChi-";
  // if (Contains(path, "T2tt")) key = "_mStop-"; 
  auto pos1 = path.rfind(key)+key.size();
  auto pos2 = path.find("_", pos1);
  string mass_string = path.substr(pos1, pos2-pos1);
  int unrounded_mass = stoi(mass_string);
  int rounded_mass = unrounded_mass;
  if (unrounded_mass != 127)
    rounded_mass = ((unrounded_mass+12)/25)*25;
  return rounded_mass;
}

// Tag examples: "_mChi-","-mGluino-" 
int GetSUSYMass(const string & path, const string & tag) {
  auto pos1 = path.rfind(tag)+tag.size();
  auto pos2 = path.find("_", pos1);
  string mass_string = path.substr(pos1, pos2-pos1);
  int unrounded_mass = stoi(mass_string);
  return unrounded_mass;
}

int GetGluinoMass(const string & path) {
  return GetSUSYMass(path, "-mGluino-");
}

void FixLumi(corrections_tree &corr, const string &corr_path, int year){
  double xsec(0.); const float lumi = 1000.;
  if (Contains(corr_path, "SMS-TChi")){
    double exsec(0.);
    int mglu = GetHiggsinoMass(corr_path);
    xsec::higgsinoCrossSection(mglu, xsec, exsec);
  } else if (Contains(corr_path, "SMS-T5qqqqZH_HToBB")) {
    double exsec(0.);
    int mglu = GetGluinoMass(corr_path);
    xsec::gluinoCrossSection(mglu, xsec, exsec);
    xsec = xsec * .5824*.5824; // Add in H to bb branch ratio
    exsec = exsec * .5824*.5824; // Add in H to bb branch ratio
  } else if (Contains(corr_path, "SMS-T5qqqqZH-")) {
    double exsec(0.);
    int mglu = GetGluinoMass(corr_path);
    xsec::gluinoCrossSection(mglu, xsec, exsec);
  }else{
    xsec = xsec::crossSection(corr_path, year);  
  }

  corr.out_w_lumi() = xsec*lumi/corr.out_neff();
}

void Fix0L(corrections_tree &corr){
  double nent = corr.out_nent();
  double nent_zlep = corr.out_nent_zlep();

  // Lepton weights corrections to be applied only to 0-lep events
  //----------------------------------------------------------------
  corr.out_w_lep()           = corr.out_w_lep() ? (nent-corr.out_w_lep())/nent_zlep : 1.;
  corr.out_w_fs_lep()        = corr.out_w_fs_lep() ? (nent-corr.out_w_fs_lep())/nent_zlep : 1.;
  for(size_t i = 0; i<corr.out_sys_lep().size(); i++){
    corr.out_sys_lep()[i]    = corr.out_sys_lep()[i] ? (nent-corr.out_sys_lep()[i])/nent_zlep : 1.;
  }
  for(size_t i = 0; i<corr.out_sys_fs_lep().size(); i++){
    corr.out_sys_fs_lep()[i] = corr.out_sys_fs_lep()[i] ? (nent-corr.out_sys_fs_lep()[i])/nent_zlep : 1.;
  }
}



void Normalize(corrections_tree &corr){
  double nent = corr.out_nent();

  // w_lep fixed in Fix0L

  Normalize(corr.out_w_btag(), nent);
  Normalize(corr.out_w_btag_df(), nent);
  Normalize(corr.out_w_bhig(), nent);
  Normalize(corr.out_w_bhig_df(), nent);

  Normalize(corr.out_w_pu(), nent);

  Normalize(corr.out_sys_bchig(), nent);
  Normalize(corr.out_sys_udsghig(), nent);
  Normalize(corr.out_sys_fs_bchig(), nent);
  Normalize(corr.out_sys_fs_udsghig(), nent);
  Normalize(corr.out_sys_pu(), nent);

  Normalize(corr.out_sys_murf(), nent);
  // Normalize(corr.out_w_pdf(), nent);
  // Normalize(corr.out_sys_pdf(), nent);
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

