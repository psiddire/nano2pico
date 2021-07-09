#!/bin/bash
WORK_DIR=$PWD
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /net/cms29/cms29r0/pico/CMSSW_10_2_11_patch1/src
eval `scramv1 runtime -sh`
cd $WORK_DIR
echo [Info] command_divider : Start divided_command[0]
echo [Info] command_divider : Current directory: /net/top/homes/psiddire/nano2pico
echo [Info] command_divider : command: /net/top/homes/psiddire/nano2pico/run/process_nano.exe -f DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8__RunIISummer20UL17NanoAODv2__106X_mc2017_realistic_v8-v1__260000__142F2BF0-9B79-EC42-BDDB-88FD33DDBFB7.root -i /net/cms17/cms17r0/pico/NanoAODv2/nano/2017/mc/ -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_mc_ul/2017/mc/
/net/top/homes/psiddire/nano2pico/run/process_nano.exe -f DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8__RunIISummer20UL17NanoAODv2__106X_mc2017_realistic_v8-v1__260000__142F2BF0-9B79-EC42-BDDB-88FD33DDBFB7.root -i /net/cms17/cms17r0/pico/NanoAODv2/nano/2017/mc/ -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_mc_ul/2017/mc/
echo [Info] command_divider : End divided_command[0]
echo [Info] command_divider : Finished
