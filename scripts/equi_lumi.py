from os import listdir
from os.path import isfile, join
from ROOT import TFile
from argparse import ArgumentParser

parser = ArgumentParser("Token for file")
parser.add_argument(
    '--token',
    type=str,
    action='store',
    dest='token',
    default='GluGluHToZG',
    help="Provide token of the file for number of events")
parser.add_argument(
    '--mypath',
    type=str,
    action='store',
    dest='mypath',
    default='/net/cms17/cms17r0/pico/NanoAODv2/nano/2017/mc/',
    help="Path for the files")
args = parser.parse_args()

token = args.token

mypath = args.mypath

onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]

print onlyfiles

nevt = 0

for fname in onlyfiles:
    if token in fname:
        f = TFile(mypath + fname)
        tree_evt = f.Get("Events")
        nevt = nevt + tree_evt.GetEntriesFast()

print nevt
