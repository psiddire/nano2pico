#!/bin/bash
WORK_DIR=$PWD
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /net/cms29/cms29r0/pico/CMSSW_10_2_11_patch1/src
eval `scramv1 runtime -sh`
cd $WORK_DIR
echo [Info] command_divider : Start divided_command[0]
echo [Info] command_divider : Current directory: /net/top/homes/psiddire/nano2pico
echo [Info] command_divider : command: /net/top/homes/psiddire/nano2pico/scripts/skim_file.py -k llg -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_signal_ul/2017/signal/raw_pico/raw_pico_WminusH_HToZG_WToAll_M-125_TuneCP5_13TeV-powheg-pythia8__RunIISummer19UL17NanoAODv2__106X_mc2017_realistic_v8-v1__280000__9B8B370B-B189-2942-BF79-DDD6D36B81B3.root -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_signal_ul/2017/signal/skim_llg/
/net/top/homes/psiddire/nano2pico/scripts/skim_file.py -k llg -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_signal_ul/2017/signal/raw_pico/raw_pico_WminusH_HToZG_WToAll_M-125_TuneCP5_13TeV-powheg-pythia8__RunIISummer19UL17NanoAODv2__106X_mc2017_realistic_v8-v1__280000__9B8B370B-B189-2942-BF79-DDD6D36B81B3.root -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_signal_ul/2017/signal/skim_llg/
echo [Info] command_divider : End divided_command[0]
echo [Info] command_divider : Finished
