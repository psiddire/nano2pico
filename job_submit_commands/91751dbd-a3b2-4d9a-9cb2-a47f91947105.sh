#!/bin/bash
WORK_DIR=$PWD
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /net/cms29/cms29r0/pico/CMSSW_10_2_11_patch1/src
eval `scramv1 runtime -sh`
cd $WORK_DIR
echo [Info] command_divider : Start divided_command[0]
echo [Info] command_divider : Current directory: /net/top/homes/psiddire/nano2pico
echo [Info] command_divider : command: /net/top/homes/psiddire/nano2pico/scripts/skim_file.py -k llg -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data/2017/data/raw_pico/raw_pico_DoubleEG__Run2017E__UL2017_MiniAODv1_NanoAODv2-v1__280000__6589E9BE-9F7C-E541-A3FB-4987605E04D3.root -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data/2017/data/skim_llg/
/net/top/homes/psiddire/nano2pico/scripts/skim_file.py -k llg -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data/2017/data/raw_pico/raw_pico_DoubleEG__Run2017E__UL2017_MiniAODv1_NanoAODv2-v1__280000__6589E9BE-9F7C-E541-A3FB-4987605E04D3.root -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data/2017/data/skim_llg/
echo [Info] command_divider : End divided_command[0]
echo [Info] command_divider : Finished
