#!/bin/bash
WORK_DIR=$PWD
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /net/cms29/cms29r0/pico/CMSSW_10_2_11_patch1/src
eval `scramv1 runtime -sh`
cd $WORK_DIR
echo [Info] command_divider : Start divided_command[0]
echo [Info] command_divider : Current directory: /net/top/homes/psiddire/nano2pico
echo [Info] command_divider : command: /net/top/homes/psiddire/nano2pico/scripts/slim_and_merge.py -s /net/top/homes/psiddire/nano2pico/txt/slim_rules/zgmc.txt -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data_muon/2017/data/merged_zgmc_llg/merged_raw_pico_llg_DoubleMuon__Run2017C__UL2017_MiniAODv1_NanoAODv2-v1__270000__8FA22850-45B0-9144-A15F-51478959C14B_zgmc_llg_nfiles_1.root -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data_muon/2017/data/skim_llg/*raw_pico_llg_DoubleMuon__Run2017C__UL2017_MiniAODv1_NanoAODv2-v1__270000__8FA22850-45B0-9144-A15F-51478959C14B*.root
/net/top/homes/psiddire/nano2pico/scripts/slim_and_merge.py -s /net/top/homes/psiddire/nano2pico/txt/slim_rules/zgmc.txt -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data_muon/2017/data/merged_zgmc_llg/merged_raw_pico_llg_DoubleMuon__Run2017C__UL2017_MiniAODv1_NanoAODv2-v1__270000__8FA22850-45B0-9144-A15F-51478959C14B_zgmc_llg_nfiles_1.root -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data_muon/2017/data/skim_llg/*raw_pico_llg_DoubleMuon__Run2017C__UL2017_MiniAODv1_NanoAODv2-v1__270000__8FA22850-45B0-9144-A15F-51478959C14B*.root
echo [Info] command_divider : End divided_command[0]
echo [Info] command_divider : Finished
