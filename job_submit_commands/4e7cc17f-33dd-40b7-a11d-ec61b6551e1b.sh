#!/bin/bash
WORK_DIR=$PWD
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /net/cms29/cms29r0/pico/CMSSW_10_2_11_patch1/src
eval `scramv1 runtime -sh`
cd $WORK_DIR
echo [Info] command_divider : Start divided_command[0]
echo [Info] command_divider : Current directory: /net/top/homes/psiddire/nano2pico
echo [Info] command_divider : command: /net/top/homes/psiddire/nano2pico/run/process_nano.exe -f DYJetsToLL_0J_TuneCP5_13TeV-amcatnloFXFX-pythia8__RunIISummer20UL17NanoAODv2__106X_mc2017_realistic_v8-v1__260000__7CE33232-6D33-C94F-A6E8-D179FA2E0EBF.root -i /net/cms17/cms17r0/pico/NanoAODv2/nano/2017/mc/ -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_mc_ul/2017/mc/
/net/top/homes/psiddire/nano2pico/run/process_nano.exe -f DYJetsToLL_0J_TuneCP5_13TeV-amcatnloFXFX-pythia8__RunIISummer20UL17NanoAODv2__106X_mc2017_realistic_v8-v1__260000__7CE33232-6D33-C94F-A6E8-D179FA2E0EBF.root -i /net/cms17/cms17r0/pico/NanoAODv2/nano/2017/mc/ -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_mc_ul/2017/mc/
echo [Info] command_divider : End divided_command[0]
echo [Info] command_divider : Finished
