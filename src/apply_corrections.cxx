#include <iostream>
#include <ctime>
#include <getopt.h>

#include "pico_tree.hpp"
#include "corrections_tree.hpp"
#include "utilities.hpp"
#include "cross_sections.hpp"

#include "TError.h"

using namespace std;

namespace {
  string in_file = "";
  string in_dir = "";
  string corr_file = "";
}

void GetOptions(int argc, char *argv[]);

int main(int argc, char *argv[]){
  // gErrorIgnoreLevel=6000; // Turns off ROOT errors due to missing branches       
  GetOptions(argc, argv);

  time_t begtime, endtime;
  time(&begtime);

  string in_file_path = in_dir+"/"+in_file;
  cout<<"Running on input file: "<<in_file_path<<endl;

  string out_file = CopyReplaceAll(in_dir, "/raw_pico/","/unskimmed/") + CopyReplaceAll(in_file, "raw_","");
  corr_file = CopyReplaceAll(in_dir, "/raw_pico/","/corrections/")+corr_file;

  cout<<"Corrections file: "<<corr_file<<endl;
  corrections_tree corr(corr_file);
  if (corr.GetEntries()!=1) {
    cout<<"ERROR:: Corrections file has "<<corr.GetEntries()<<", expected 1."<<endl;
    exit(1);
  }
  corr.GetEntry(0);

  pico_tree pico(in_file_path, out_file);
  for(long entry(0); entry<pico.GetEntries(); entry++){

    pico.GetEntry(entry);
    if (entry%100000==0) {
      cout<<"Processing event: "<<entry<<endl;
    }

    pico.out_w_lumi() = pico.w_lumi()>0 ? 1. : -1.; //get the generator weight sign
    pico.out_w_lumi() *= corr.w_lumi();

    pico.out_weight() = pico.weight() * pico.out_w_lumi(); // * pico.w_prefire();

    pico.out_w_pu() = pico.w_pu()*corr.w_pu();

    pico.out_sys_pu().resize(2);
    for (unsigned i(0); i<2; i++) {        
      pico.out_sys_pu()[i]         = pico.sys_pu()[i]*corr.sys_pu()[i];
    } 

    pico.out_sys_murf().resize(9);

    string str1("WplusH_HToZG");
    string str2("ZH_HToZG");

    if (!((in_file.find(str1) != std::string::npos) || (in_file.find(str2) != std::string::npos))) {
      for (unsigned i(0); i<9; i++) {
	pico.out_sys_murf()[i]      = pico.sys_murf()[i]*corr.sys_murf()[i];
      }
    }

    // for (unsigned i(0); i<pico.w_pdf().size(); i++) 
    //   pico.out_w_pdf()[i] = pico.w_pdf()[i]*corr.w_pdf()[i];    
    // for (unsigned i(0); i<pico.sys_pdf().size(); i++) 
    //   pico.out_sys_pdf()[i] = pico.sys_pdf()[i]*corr.sys_pdf()[i];    
    
    pico.Fill();

  } // loop over events
  
  pico.Write();

  cout<<endl;
  time(&endtime); 
  cout<<"Time passed: "<<hoursMinSec(difftime(endtime, begtime))<<endl<<endl;  
}

void GetOptions(int argc, char *argv[]){
  while(true){
    static struct option long_options[] = {
      {"in_file", required_argument, 0, 'f'},  
      {"in_dir", required_argument, 0, 'i'},  
      {"corr_file", required_argument, 0, 'c'},
      {0, 0, 0, 0}
    };

    char opt = -1;
    int option_index;
    opt = getopt_long(argc, argv, "f:i:c:", long_options, &option_index);
    if(opt == -1) break;

    string optname;
    switch(opt){
    case 'f':
      in_file = optarg;
      break;
    case 'i':
      in_dir = optarg;
      break;
    case 'c':
      corr_file = optarg;
      break;
    case 0:
      break;
    default:
      printf("Bad option! getopt_long returned character code 0%o\n", opt);
      break;
    }
  }
}
