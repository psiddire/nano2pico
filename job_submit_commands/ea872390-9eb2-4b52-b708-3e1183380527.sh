#!/bin/bash
WORK_DIR=$PWD
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /net/cms29/cms29r0/pico/CMSSW_10_2_11_patch1/src
eval `scramv1 runtime -sh`
cd $WORK_DIR
echo [Info] command_divider : Start divided_command[0]
echo [Info] command_divider : Current directory: /net/top/homes/psiddire/nano2pico
echo [Info] command_divider : command: /net/top/homes/psiddire/nano2pico/scripts/skim_file.py -k llg -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_mc_ul/2017/mc/raw_pico/raw_pico_WW_TuneCP5_13TeV-pythia8__RunIISummer20UL17NanoAODv2__106X_mc2017_realistic_v8-v1__40000__F8E020CC-D611-1E48-9E7C-A73BEE10CB4B.root -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_mc_ul/2017/mc/skim_llg/
/net/top/homes/psiddire/nano2pico/scripts/skim_file.py -k llg -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_mc_ul/2017/mc/raw_pico/raw_pico_WW_TuneCP5_13TeV-pythia8__RunIISummer20UL17NanoAODv2__106X_mc2017_realistic_v8-v1__40000__F8E020CC-D611-1E48-9E7C-A73BEE10CB4B.root -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_mc_ul/2017/mc/skim_llg/
echo [Info] command_divider : End divided_command[0]
echo [Info] command_divider : Finished
