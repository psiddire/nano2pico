#!/usr/bin/env python3

import os, sys, argparse
from glob import glob

parser = argparse.ArgumentParser(description="Submits batch jobs to apply new SFs and compute sum-of-weights",
                                 formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("-i","--in_dir", default="",
                    help="Directory where the NanoAOD files are, e.g. /net/cms17/cms17r0/pico/NanoAODv2/nano/2017/mc/")
parser.add_argument("-p","--production", default="zgamma_mc_ul",
                    help="Determines the output folder.")
parser.add_argument("-d","--dataset_list", default="",
                    help="File with the list of dataset names as they appear in DAS or the list of filenames (with wildcards). If not specified will run on all files in input folder")
parser.add_argument('-t', '--tag', default='',
                    help='Optionally specify a tag to be used to differentiate helper files for batch submission.')
parser.add_argument("-l","--list_format", default="DAS", choices=["DAS","filename"],
                    help="Sets whether the dataset list is in DAS format or filename format.")
args = vars(parser.parse_args())

in_dir = args['in_dir']
out_base_dir = args['in_dir'].replace('/mnt/hadoop/','/net/cms29/cms29r0/').replace('/nano/','/'+args['production']+'/')

if not os.path.exists(out_base_dir): 
  os.makedirs(out_base_dir)
if not os.path.exists(out_base_dir+"/raw_pico/"): 
  os.mkdir(out_base_dir+"/raw_pico/")
if not os.path.exists(out_base_dir+"/wgt_sums/"): 
  os.mkdir(out_base_dir+"/wgt_sums/")

list_format = args['list_format']
in_file_paths = []
all_file_paths = glob(os.path.join(in_dir,'*.root'))
if args['dataset_list']!='':
  datasets = []
  with open(args['dataset_list']) as f:
    datasets = f.readlines()
  if (list_format == "DAS"):
    wanted_file_substr = []
    for ds in datasets: 
      if ds[0]!="/": # in case of empty lines or comments
        continue
      tmp_ = ds.split("/")
      if("NanoAODv2" in tmp_[2]):
        wanted_file_substr.append(tmp_[1]+'__'+tmp_[2].replace("NanoAODv2-","NanoAODv2__"))
      else:
        wanted_file_substr.append(tmp_[1]+'__'+tmp_[2].replace("NanoAODv7-","NanoAODv7__"))
    for istr in wanted_file_substr:
      for ifile in all_file_paths:
        if (istr in ifile):
          in_file_paths.append(ifile)
  else:
    for ds in datasets:
      if len(ds)<2: #in case of empty lines
        continue
      #remember to skip newline characters
      in_file_paths = in_file_paths + glob(os.path.join(in_dir,ds[0:-1]))
else:
  in_file_paths = all_file_paths

print("Found {} input files.\n".format(len(in_file_paths)))

cmdfile_name = 'process_nano_cmds.py'
if (args['tag']!=''): cmdfile_name = cmdfile_name.replace('.py', '_'+args['tag']+'.py')
cmdfile = open(cmdfile_name,'w')
cmdfile.write('#!/bin/env python\n')
for ifile_path in in_file_paths:
  ifile = os.path.basename(os.path.realpath(ifile_path))
  cmd = '{}/run/process_nano.exe -f {} -i {} -o {}'.format(os.getcwd(), ifile, in_dir, out_base_dir)
  cmdfile.write('print(\"'+cmd+'\")\n')

cmdfile.close()
os.chmod(cmdfile_name, 0o755)

json_name = cmdfile_name.replace('.py','.json')
os.system('convert_cl_to_jobs_info.py '+cmdfile_name+' '+json_name)

print("To generate job json and submit jobs do: ")
print('auto_submit_jobs.py '+json_name+' -c scripts/check_process_nano_job.py')
  
