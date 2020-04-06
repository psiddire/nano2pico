#!/usr/bin/env python
# The ./jobscript_check.py should return 'success' or 'fail' or 'to_submit' or 'submitted' for a job_log_string
# The input is given as sys.argv[1] = queue_system.compress_string(job_log_string) sys.argv[2] = queue_system.compress_string(job_argument_string)
import sys
import os
import queue_system
from ROOT import TChain
from skim_file import get_cuts

# job_argument_string = "/net/top/homes/oshiro/code/nano2pico/scripts/skim_file.py -k 2l -i /net/cms29/cms29r0/pico/NanoAODv5/ttz_cordellbankv2/2016/data/unskimmed/pico_Run2016B_0_SingleElectron_SingleMuon_Nano1June2019-v1_runs275290.root -o /net/cms29/cms29r0/pico/NanoAODv5/ttz_cordellbankv2/2016/data/skim_2l/"
job_log_string = queue_system.decompress_string(sys.argv[1])
job_argument_string = queue_system.decompress_string(sys.argv[2])

print(job_argument_string)

args = job_argument_string.split('--command="')[1].split('"')[0]
tmp = args.split(' ')
infile_path = tmp[4]

if '/merged_' in infile_path: 
  in_dir = os.path.dirname(infile_path)
  skim_name = tmp[2]
  out_dir = tmp[6]
  outfile_path = infile_path.replace(in_dir,out_dir).replace('pico_','pico_'+skim_name+'_')
else:
  outfile_basename = tmp[4].split('/')[-1][5:]
  outfile_path = tmp[6]+'pico_'+tmp[2]+'_'+outfile_basename

infile = TChain("tree");
infile.Add(infile_path);
in_nent = infile.GetEntries(get_cuts(tmp[2]))

if os.path.exists(outfile_path):
  outfile = TChain("tree");
  outfile.Add(outfile_path);
  out_nent = outfile.GetEntries()
else:
  out_nent = 0

if outfile.GetNbranches() == 0:
  print('[For queue_system] fail: output ({}) has no branches.'.format(outfile_path))
elif in_nent == out_nent:
  print('[For queue_system] success')
else:
  print('[For queue_system] fail: Input ({}) has {} entries, while output ({}) has {} entries.'.format(infile_path, in_nent, outfile_path, out_nent))
