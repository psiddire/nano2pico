#!/bin/bash
WORK_DIR=$PWD
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /net/cms29/cms29r0/pico/CMSSW_10_2_11_patch1/src
eval `scramv1 runtime -sh`
cd $WORK_DIR
echo [Info] command_divider : Start divided_command[0]
echo [Info] command_divider : Current directory: /net/top/homes/psiddire/nano2pico
echo [Info] command_divider : command: /net/top/homes/psiddire/nano2pico/scripts/skim_file.py -k llg -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data/2017/data/raw_pico/raw_pico_DoubleEG__Run2017F__UL2017_02Dec2019-v1__260000__EF7147F9-4FC4-2748-8F9F-E54CC92E8818.root -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data/2017/data/skim_llg/
/net/top/homes/psiddire/nano2pico/scripts/skim_file.py -k llg -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data/2017/data/raw_pico/raw_pico_DoubleEG__Run2017F__UL2017_02Dec2019-v1__260000__EF7147F9-4FC4-2748-8F9F-E54CC92E8818.root -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data/2017/data/skim_llg/
echo [Info] command_divider : End divided_command[0]
echo [Info] command_divider : Finished
