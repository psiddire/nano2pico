import ROOT
import os
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('input_path')
parser.add_argument('file_name')
args = parser.parse_args()

path = args.input_path
f = args.file_name

lst = os.popen("ls " + path + f + "*").read()
lst = list(filter(None, lst.split('\n')))

entries = 0
for l in lst:
    f = ROOT.TFile(l)
    t = f.Get("Events")
    # t = f.Get("tree")
    entries = entries + t.GetEntries()
print entries
