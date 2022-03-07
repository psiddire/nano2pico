import ROOT
import math
ROOT.ROOT.EnableImplicitMT()

path = "/net/cms17/cms17r0/pico/NanoAODv2/zgamma_signal_ul/2017/signal/merged_zgmc_unskimmed/"
fName = "merged_pico_GluGluHToZG_M-125_TuneCP5_13TeV-powheg-pythia8_zgmc_unskimmed_nfiles_5.root"

f = ROOT.TFile(path + fName)
t = f.Get("tree")
nevt = t.GetEntries()
for i in range(0, 10000):
    t.GetEntry(i)
    pdgId = map(int, t.mc_id)
    status = map(int, t.mc_statusflag)
    mom = map(int, t.mc_mom)
    pt = map(float, t.mc_pt)
    if 22 in pdgId:
        photonIdx = pdgId.index(22)
        if status[photonIdx]!=12673:
            print zip(pdgId, status, mom, pt)
            print
