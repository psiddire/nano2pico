#!/bin/bash
WORK_DIR=$PWD
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /net/cms29/cms29r0/pico/CMSSW_10_2_11_patch1/src
eval `scramv1 runtime -sh`
cd $WORK_DIR
echo [Info] command_divider : Start divided_command[0]
echo [Info] command_divider : Current directory: /net/top/homes/psiddire/nano2pico
echo [Info] command_divider : command: /net/top/homes/psiddire/nano2pico/scripts/slim_and_merge.py -s /net/top/homes/psiddire/nano2pico/txt/slim_rules/zgmc.txt -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_signal_ul/2017/signal/merged_zgmc_llg/merged_raw_pico_llg_ttHToZG_M125_TuneCP5_13TeV-powheg-pythia8__RunIISummer19UL17NanoAODv2__106X_mc2017_realistic_v8-v1__270000__90F9DDC7-464B-2341-BD1B-578EEC6C73FE_zgmc_llg_nfiles_1.root -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_signal_ul/2017/signal/skim_llg/*raw_pico_llg_ttHToZG_M125_TuneCP5_13TeV-powheg-pythia8__RunIISummer19UL17NanoAODv2__106X_mc2017_realistic_v8-v1__270000__90F9DDC7-464B-2341-BD1B-578EEC6C73FE*.root
/net/top/homes/psiddire/nano2pico/scripts/slim_and_merge.py -s /net/top/homes/psiddire/nano2pico/txt/slim_rules/zgmc.txt -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_signal_ul/2017/signal/merged_zgmc_llg/merged_raw_pico_llg_ttHToZG_M125_TuneCP5_13TeV-powheg-pythia8__RunIISummer19UL17NanoAODv2__106X_mc2017_realistic_v8-v1__270000__90F9DDC7-464B-2341-BD1B-578EEC6C73FE_zgmc_llg_nfiles_1.root -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_signal_ul/2017/signal/skim_llg/*raw_pico_llg_ttHToZG_M125_TuneCP5_13TeV-powheg-pythia8__RunIISummer19UL17NanoAODv2__106X_mc2017_realistic_v8-v1__270000__90F9DDC7-464B-2341-BD1B-578EEC6C73FE*.root
echo [Info] command_divider : End divided_command[0]
echo [Info] command_divider : Finished
