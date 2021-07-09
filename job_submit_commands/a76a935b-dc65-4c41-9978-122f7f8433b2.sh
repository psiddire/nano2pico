#!/bin/bash
WORK_DIR=$PWD
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /net/cms29/cms29r0/pico/CMSSW_10_2_11_patch1/src
eval `scramv1 runtime -sh`
cd $WORK_DIR
echo [Info] command_divider : Start divided_command[0]
echo [Info] command_divider : Current directory: /net/top/homes/psiddire/nano2pico
echo [Info] command_divider : command: /net/top/homes/psiddire/nano2pico/scripts/skim_file.py -k llg -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data_muon/2017/data/raw_pico/raw_pico_DoubleMuon__Run2017G__UL2017_MiniAODv1_NanoAODv2-v1__00000__31F4DF67-F251-A74D-9CCF-88B902F582E4.root -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data_muon/2017/data/skim_llg/
/net/top/homes/psiddire/nano2pico/scripts/skim_file.py -k llg -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data_muon/2017/data/raw_pico/raw_pico_DoubleMuon__Run2017G__UL2017_MiniAODv1_NanoAODv2-v1__00000__31F4DF67-F251-A74D-9CCF-88B902F582E4.root -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data_muon/2017/data/skim_llg/
echo [Info] command_divider : End divided_command[0]
echo [Info] command_divider : Finished
