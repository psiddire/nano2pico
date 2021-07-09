#!/bin/bash
WORK_DIR=$PWD
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /net/cms29/cms29r0/pico/CMSSW_10_2_11_patch1/src
eval `scramv1 runtime -sh`
cd $WORK_DIR
echo [Info] command_divider : Start divided_command[0]
echo [Info] command_divider : Current directory: /net/top/homes/psiddire/nano2pico
echo [Info] command_divider : command: /net/top/homes/psiddire/nano2pico/run/process_nano.exe -f GluGluHToZG_M-125_TuneCP5_13TeV-powheg-pythia8__RunIISummer19UL17NanoAODv2__106X_mc2017_realistic_v8-v1__40000__341A16B2-76BC-5D45-9741-7F9366D351DE.root -i /net/cms17/cms17r0/pico/NanoAODv2/nano/2017/signal/ -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_signal_ul/2017/signal/
/net/top/homes/psiddire/nano2pico/run/process_nano.exe -f GluGluHToZG_M-125_TuneCP5_13TeV-powheg-pythia8__RunIISummer19UL17NanoAODv2__106X_mc2017_realistic_v8-v1__40000__341A16B2-76BC-5D45-9741-7F9366D351DE.root -i /net/cms17/cms17r0/pico/NanoAODv2/nano/2017/signal/ -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_signal_ul/2017/signal/
echo [Info] command_divider : End divided_command[0]
echo [Info] command_divider : Finished
