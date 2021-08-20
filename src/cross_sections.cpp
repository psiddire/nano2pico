// Lists the MC cross sections

#include <iostream>
#include "cross_sections.hpp"

using namespace std;

namespace xsec{

  float crossSection(const TString &file, int year){
    float xsec(-999999.), Htobb(0.5824), HToZG(0.001533), ZToLL(0.100974);

    // Zgamma cross sections at 13 TeV
    // https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns
    if(file.Contains("DYJetsToLL"))           xsec = 6077.22;
    if(file.Contains("ZGTo2LG"))              xsec = 117.864;
    if(file.Contains("ZGToLLG"))              xsec = 55.48; // Other sample name
    if(file.Contains("TTTo2L2Nu"))            xsec = 88.29;
    if(file.Contains("TTToSemi"))             xsec = 365.35;
    if(file.Contains("TTToHadronic"))         xsec = 377.96;
    if(file.Contains("TTJets"))               xsec = 831.76;
    if(file.Contains("TT_Tune"))              xsec = 831.76;
    if(file.Contains("TGJets"))               xsec = 2.967;
    if(file.Contains("ST_tW_antitop"))        xsec = 35.85;
    if(file.Contains("ST_tW_top"))            xsec = 35.85;
    if(file.Contains("ST_t-channel_antitop")) xsec = 80.95;
    if(file.Contains("ST_t-channel_top_"))    xsec = 136.02;
    if(file.Contains("LLAJJ"))                xsec = 0.1084; // from XSDB
    if(file.Contains("WZ_Tune"))              xsec = 51.11;
    if(file.Contains("WW_Tune"))              xsec = 118.7; // From arXiv:1408.5243
    if(file.Contains("ZZ_Tune"))              xsec = 16.91;
    if(file.Contains("WGGJets"))              xsec = 1.715;
    if(file.Contains("WWG"))                  xsec = 0.2147;
    if(file.Contains("ZGGJetsToLLGG"))        xsec = 0.1699;
    
    // Zgamma signal
    if(file.Contains("GluGluHToZG"))   xsec = HToZG * ZToLL * 48.58 ;
    if(file.Contains("VBFHToZG"))      xsec = HToZG * ZToLL * 3.782 ;
    if(file.Contains("WplusH_HToZG"))  xsec = HToZG * ZToLL * 0.831 ;
    if(file.Contains("WminusH_HToZG")) xsec = HToZG * ZToLL * 0.527 ;
    if(file.Contains("ZH_HToZG"))      xsec = HToZG * ZToLL * 0.8839;
    if(file.Contains("ttHToZG"))       xsec = HToZG * ZToLL * 0.5071;

    if(xsec<=0) std::cout<<"ERROR:: Cross section not found for "<<file<<std::endl;

    return xsec;
  }

  float fractionNegWeights(const TString &file){
    float fneg(0.);

    // Single top
    if (file.Contains("ST_s-channel_4f_leptonDecays_13TeV-amcatnlo-pythia8")) fneg = 0.1884;

    return fneg;
  }

}
