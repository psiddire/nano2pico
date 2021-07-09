#!/usr/bin/env python
import ROOT
import os
import argparse
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument('input_path')
parser.add_argument('output_path')
args = parser.parse_args()

ROOT.ROOT.EnableImplicitMT()

entries = 29885702
xsec = 55.48
equi_lumi = entries/xsec
lumi = 41500
weight = lumi/equi_lumi

h_llg = ROOT.TH1F("h_llg", "llg_mass", 80, 100, 180)
fIn = ROOT.TFile(args.input_path)
t = fIn.Get("tree")
for i in range(0, t.GetEntries()):
    if i % 1000 == 0:
        print i
    t.GetEntry(i)
    if not t.HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL:
        continue
    if t.nll < 1 or t.nphoton < 1: # or t.nbm
        continue
    if all(dr < 0.4 for dr in t.photon_drmin):
        continue
    if t.ll_charge.size()==1 and t.ll_charge[0]!=0:
        continue
    if sum(map(bool, t.el_id)) < 2:
        continue
    if sum(abs(dz) < 0.01 for dz in t.el_dz) < 2:
        continue
    if sum(abs(dxy) < 0.005 for dxy in t.el_dxy) < 2:
        continue

    idll = []
    for z in range(t.nll):
        x = t.ll_i1[z]
        y = t.ll_i2[z]
        if (t.el_charge[x]*t.el_charge[y] == -1 and \
                t.el_pt[x] > 25 and t.el_dz[x] < 0.01 and t.el_dxy[x] < 0.005 and bool(t.el_id[x]) and \
                t.el_pt[y] > 15 and t.el_dz[y] < 0.01 and t.el_dxy[y] < 0.005 and bool(t.el_id[y])):
            idll.append(z)
            break
    if len(idll) == 0:
        continue
    massZ = t.ll_m[idll[0]]

    idllg = []
    iph = 0
    ph = ROOT.TLorentzVector()
    for z in range(t.nllphoton):
        if t.llphoton_ill[z]!=idll[0]:
            continue
        if (bool(t.photon_id[iph]) and t.photon_drmin[iph] > 0.4 and t.photon_pt[iph] > 15):
            ph.SetPtEtaPhiM(t.photon_pt[iph], t.photon_eta[iph], t.photon_phi[iph], 0)
            idllg.append(z)
            break
        iph = iph + 1
    if len(idllg) == 0:
        continue
    massH = t.llphoton_m[idllg[0]]

    if (massZ + massH) < 185:
        continue
    if (ph.E()/massH) < 15/110:
        continue

    print "weight, w_lumi, w_lep, w_fs_lep, w_photon, w_btag, w_btag_df, w_bhig, w_bhig_df, w_isr, w_pu, w_prefire, eff_trig"
    print t.weight, t.w_lumi, t.w_lep, t.w_fs_lep, t.w_photon, t.w_btag, t.w_btag_df, t.w_bhig, t.w_bhig_df, t.w_isr, t.w_pu, t.w_prefire, t.eff_trig

    h_llg.Fill(massH, weight)

input_path = args.input_path.split('/')
name = input_path[10]

fOut = ROOT.TFile(args.output_path, "RECREATE");
h_llg.Write()
fOut.Close()
