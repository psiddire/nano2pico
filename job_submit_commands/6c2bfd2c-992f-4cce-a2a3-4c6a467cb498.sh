#!/bin/bash
WORK_DIR=$PWD
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /net/cms29/cms29r0/pico/CMSSW_10_2_11_patch1/src
eval `scramv1 runtime -sh`
cd $WORK_DIR
echo [Info] command_divider : Start divided_command[0]
echo [Info] command_divider : Current directory: /net/top/homes/psiddire/nano2pico
echo [Info] command_divider : command: /net/top/homes/psiddire/nano2pico/scripts/slim_and_merge.py -s /net/top/homes/psiddire/nano2pico/txt/slim_rules/zgmc.txt -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_mc_ul/2017/mc/merged_zgmc_llg/merged_raw_pico_llg_ZGToLLG_01J_5f_TuneCP5_13TeV-amcatnloFXFX-pythia8__RunIISummer20UL17NanoAODv2__106X_mc2017_realistic_v8-v1__230000__7C9F7806-210A-2145-A954-C137A6C96BA6_zgmc_llg_nfiles_1.root -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_mc_ul/2017/mc/skim_llg/*raw_pico_llg_ZGToLLG_01J_5f_TuneCP5_13TeV-amcatnloFXFX-pythia8__RunIISummer20UL17NanoAODv2__106X_mc2017_realistic_v8-v1__230000__7C9F7806-210A-2145-A954-C137A6C96BA6*.root
/net/top/homes/psiddire/nano2pico/scripts/slim_and_merge.py -s /net/top/homes/psiddire/nano2pico/txt/slim_rules/zgmc.txt -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_mc_ul/2017/mc/merged_zgmc_llg/merged_raw_pico_llg_ZGToLLG_01J_5f_TuneCP5_13TeV-amcatnloFXFX-pythia8__RunIISummer20UL17NanoAODv2__106X_mc2017_realistic_v8-v1__230000__7C9F7806-210A-2145-A954-C137A6C96BA6_zgmc_llg_nfiles_1.root -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_mc_ul/2017/mc/skim_llg/*raw_pico_llg_ZGToLLG_01J_5f_TuneCP5_13TeV-amcatnloFXFX-pythia8__RunIISummer20UL17NanoAODv2__106X_mc2017_realistic_v8-v1__230000__7C9F7806-210A-2145-A954-C137A6C96BA6*.root
echo [Info] command_divider : End divided_command[0]
echo [Info] command_divider : Finished
