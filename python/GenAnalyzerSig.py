import ROOT
import math
ROOT.ROOT.EnableImplicitMT()

path = "/net/cms17/cms17r0/pico/NanoAODv2/zgamma_signal_ul/2017/signal/merged_zgmc_unskimmed/"
fName = ["merged_pico_GluGluHToZG_M-125_TuneCP5_13TeV-powheg-pythia8_zgmc_unskimmed_nfiles_5.root",
"merged_pico_VBFHToZG_M-125_TuneCP5_13TeV-powheg-pythia8_zgmc_unskimmed_nfiles_3.root",
"merged_pico_WminusH_HToZG_WToAll_M-125_TuneCP5_13TeV-powheg-pythia8_zgmc_unskimmed_nfiles_4.root",
"merged_pico_WplusH_HToZG_WToAll_M-125_TuneCP5_13TeV-powheg-pythia8_zgmc_unskimmed_nfiles_3.root",
"merged_pico_ZH_HToZG_ZToAll_M-125_TuneCP5_13TeV-powheg-pythia8_zgmc_unskimmed_nfiles_6.root",
"merged_pico_ttHToZG_M125_TuneCP5_13TeV-powheg-pythia8_zgmc_unskimmed_nfiles_4.root"]

for fN in fName:
    f = ROOT.TFile(path + fN)
    t = f.Get("tree")
    nevt = t.GetEntries()
    fHP = 0
    for i in range(nevt):
        t.GetEntry(i)
        pdgId = map(int, t.mc_id)
        status = map(int, t.mc_statusflag)
        mom = map(int, t.mc_mom)
        for j, pId in enumerate(pdgId):
            if pId==22 and mom[j]==25:
                if status[j]==12673:
                    fHP = fHP + 1

    print nevt, fHP, fHP/float(nevt)
