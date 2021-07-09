#!/bin/bash
WORK_DIR=$PWD
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /net/cms29/cms29r0/pico/CMSSW_10_2_11_patch1/src
eval `scramv1 runtime -sh`
cd $WORK_DIR
echo [Info] command_divider : Start divided_command[0]
echo [Info] command_divider : Current directory: /net/top/homes/psiddire/nano2pico
echo [Info] command_divider : command: /net/top/homes/psiddire/nano2pico/scripts/slim_and_merge.py -s /net/top/homes/psiddire/nano2pico/txt/slim_rules/zgmc.txt -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data/2017/data/merged_zgmc_llg/merged_raw_pico_llg_DoubleEG__Run2017D__UL2017_Nano02Dec2019-v1__30000__3BBD6396-19D8-AD49-AE1A-A2E807B205B5_zgmc_llg_nfiles_1.root -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data/2017/data/skim_llg/*raw_pico_llg_DoubleEG__Run2017D__UL2017_Nano02Dec2019-v1__30000__3BBD6396-19D8-AD49-AE1A-A2E807B205B5*.root
/net/top/homes/psiddire/nano2pico/scripts/slim_and_merge.py -s /net/top/homes/psiddire/nano2pico/txt/slim_rules/zgmc.txt -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data/2017/data/merged_zgmc_llg/merged_raw_pico_llg_DoubleEG__Run2017D__UL2017_Nano02Dec2019-v1__30000__3BBD6396-19D8-AD49-AE1A-A2E807B205B5_zgmc_llg_nfiles_1.root -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data/2017/data/skim_llg/*raw_pico_llg_DoubleEG__Run2017D__UL2017_Nano02Dec2019-v1__30000__3BBD6396-19D8-AD49-AE1A-A2E807B205B5*.root
echo [Info] command_divider : End divided_command[0]
echo [Info] command_divider : Finished
