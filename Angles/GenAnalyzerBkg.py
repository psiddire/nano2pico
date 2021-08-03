import ROOT
import math
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('input_path')
parser.add_argument('output_path')
args = parser.parse_args()

ROOT.ROOT.EnableImplicitMT()

f = ROOT.TFile(args.input_path)
t = f.Get("Events")

h_costheta_el = ROOT.TH1F("h_costheta_el", "h_costheta_el", 20, -1, 1)
h_cosTheta_el = ROOT.TH1F("h_cosTheta_el", "h_cosTheta_el", 20, -1, 1)
h_psi_el = ROOT.TH1F("h_phi_el", "h_phi_el", 80, -1, 7)
h_costheta_mu = ROOT.TH1F("h_costheta_mu", "h_costheta_mu", 20, -1, 1)
h_cosTheta_mu = ROOT.TH1F("h_cosTheta_mu", "h_cosTheta_mu", 20, -1, 1)
h_psi_mu = ROOT.TH1F("h_phi_mu", "h_phi_mu", 80, -1, 7)
for i in range(0, t.GetEntries()):
    if i%1000==0:
        print i
    t.GetEntry(i)
    pdgId = map(int, t.GenPart_pdgId)
    motherId = map(int, t.GenPart_genPartIdxMother)
    status = map(int, t.GenPart_statusFlags)

    if 23 in pdgId and 22 in pdgId:

        photonIdx = pdgId.index(22)
        if status[photonIdx]!=22913 and status[photonIdx]!=8193:
            continue
        phDecay = [j for j in range(t.nGenPart) if motherId[j]==photonIdx]

        if len(phDecay)==0:
            photonId = photonIdx
        else:
            photonId = phDecay[0]

        zDecay = [j for j in range(t.nGenPart) if motherId[j]==pdgId.index(23)]
        while len(zDecay)==1:
            zDecay = [j for j in range(t.nGenPart) if motherId[j]==zDecay[0]]
        l1Idx, l2Idx = zDecay[0], zDecay[1]

        if abs(pdgId[l1Idx])!=11 and abs(pdgId[l1Idx])!=13:
            continue

        weight_el, weight_mu = 1.0, 1.0
        if abs(pdgId[l1Idx])==11:
            weight_mu = 0
        if abs(pdgId[l1Idx])==13:
            weight_el = 0

        ph, lminus, lplus = ROOT.TLorentzVector(), ROOT.TLorentzVector(), ROOT.TLorentzVector()
        ph.SetPtEtaPhiM(t.GenPart_pt[photonId], t.GenPart_eta[photonId], t.GenPart_phi[photonId], t.GenPart_mass[photonId])
        lminus.SetPtEtaPhiM(t.GenPart_pt[l1Idx], t.GenPart_eta[l1Idx], t.GenPart_phi[l1Idx], t.GenPart_mass[l1Idx])
        lplus.SetPtEtaPhiM(t.GenPart_pt[l2Idx], t.GenPart_eta[l2Idx], t.GenPart_phi[l2Idx], t.GenPart_mass[l2Idx])

        dilep = lminus + lplus
        llg = dilep + ph

        # Variables used for defining kinematic angles presented in https://arxiv.org/pdf/1108.2274.pdf
        M = llg.M()
        mll = dilep.M()

        if mll > 130 or mll < 120:
            continue

        if math.pow(llg.Dot(dilep)/M,2) < math.pow(mll,2):
            continue
        lZ = math.sqrt(math.pow(llg.Dot(dilep)/M,2)-math.pow(mll,2))
        hBoost = llg.BoostVector()

        # Cosine of angle between lepton 1 and parent Z
        costheta = llg.Dot(lminus-lplus)/(M*lZ)

        # 4-momenta of q1/q2 (quarks from gluon-gluon fusion)
        q, qBar = ROOT.TLorentzVector(), ROOT.TLorentzVector()
        hTransverseBoost = llg.BoostVector()
        hTransverseBoost.SetZ(0)
        hH = llg
        hH.Boost(-1*hTransverseBoost)
        hPz = hH.Pz()
        hE = hH.E()
        q.SetPxPyPzE(0,0,(hPz+hE)/2,(hE+hPz)/2)
        qBar.SetPxPyPzE(0,0,(hPz-hE)/2,(hE-hPz)/2)
        q.Boost(hTransverseBoost)
        qBar.Boost(hTransverseBoost)

        # Cosine of angle between incoming quarks and outgoing Zs in Higgs frame
        cosTheta = (qBar-q).Dot(dilep)/(M*lZ)
        if (1 - math.pow(cosTheta, 2)) < 0:
            continue
        sinTheta = math.sqrt(1 - math.pow(cosTheta, 2))

        # Angle phi
        dilep.Boost(-1*hBoost)
        zBoost = dilep.BoostVector()
        q.Boost(-1*hBoost)
        lminus.Boost(-1*hBoost)
        lplus.Boost(-1*hBoost)
        l1, l2, Z = lminus.Vect(), lplus.Vect(), dilep.Vect()
        qvec = q.Vect()
        cospsi = -1*l1.Cross(l2).Dot(qvec.Cross(Z))/l1.Cross(l2).Mag()/qvec.Cross(Z).Mag()
        sinpsi = -1*l1.Cross(l2).Dot(qvec)/l1.Cross(l2).Mag()/qvec.Mag()/sinTheta
        if cospsi > 1:
            cospsi = 1
        elif cospsi < -1:
            cospsi = -1
        if sinpsi < 0:
            psi = -1*math.acos(cospsi)
        else:
            psi = math.acos(cospsi)
        if psi < 0:
            psi = psi + 2 * math.pi

        h_costheta_el.Fill(costheta, weight_el)
        h_cosTheta_el.Fill(cosTheta, weight_el)
        h_psi_el.Fill(psi, weight_el)
        h_costheta_mu.Fill(costheta, weight_mu)
        h_cosTheta_mu.Fill(cosTheta, weight_mu)
        h_psi_mu.Fill(psi, weight_mu)

f1 = ROOT.TFile(args.output_path, "RECREATE")
h_costheta_el.Write()
h_cosTheta_el.Write()
h_psi_el.Write()
h_costheta_mu.Write()
h_cosTheta_mu.Write()
h_psi_mu.Write()
f1.Close
