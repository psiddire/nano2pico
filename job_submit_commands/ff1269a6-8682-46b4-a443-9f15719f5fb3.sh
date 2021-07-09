#!/bin/bash
WORK_DIR=$PWD
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /net/cms29/cms29r0/pico/CMSSW_10_2_11_patch1/src
eval `scramv1 runtime -sh`
cd $WORK_DIR
echo [Info] command_divider : Start divided_command[0]
echo [Info] command_divider : Current directory: /net/top/homes/psiddire/nano2pico
echo [Info] command_divider : command: /net/top/homes/psiddire/nano2pico/run/process_nano.exe -f DYJetsToLL_1J_TuneCP5_13TeV-amcatnloFXFX-pythia8__RunIISummer20UL17NanoAODv2__106X_mc2017_realistic_v8-v1__230000__A547239E-F9E9-8A44-9240-B17BB0C47D5D.root -i /net/cms17/cms17r0/pico/NanoAODv2/nano/2017/mc/ -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_mc_ul/2017/mc/
/net/top/homes/psiddire/nano2pico/run/process_nano.exe -f DYJetsToLL_1J_TuneCP5_13TeV-amcatnloFXFX-pythia8__RunIISummer20UL17NanoAODv2__106X_mc2017_realistic_v8-v1__230000__A547239E-F9E9-8A44-9240-B17BB0C47D5D.root -i /net/cms17/cms17r0/pico/NanoAODv2/nano/2017/mc/ -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_mc_ul/2017/mc/
echo [Info] command_divider : End divided_command[0]
echo [Info] command_divider : Finished
