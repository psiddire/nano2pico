#!/bin/bash
WORK_DIR=$PWD
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /net/cms29/cms29r0/pico/CMSSW_10_2_11_patch1/src
eval `scramv1 runtime -sh`
cd $WORK_DIR
echo [Info] command_divider : Start divided_command[0]
echo [Info] command_divider : Current directory: /net/top/homes/psiddire/nano2pico
echo [Info] command_divider : command: /net/top/homes/psiddire/nano2pico/scripts/slim_and_merge.py -s /net/top/homes/psiddire/nano2pico/txt/slim_rules/zgmc.txt -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data_muon/2017/data/merged_zgmc_llg/merged_raw_pico_llg_DoubleMuon__Run2017F__UL2017_MiniAODv1_NanoAODv2-v1__280000__D530DEFA-84B9-FC43-95DF-FF7E4A103D68_zgmc_llg_nfiles_1.root -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data_muon/2017/data/skim_llg/*raw_pico_llg_DoubleMuon__Run2017F__UL2017_MiniAODv1_NanoAODv2-v1__280000__D530DEFA-84B9-FC43-95DF-FF7E4A103D68*.root
/net/top/homes/psiddire/nano2pico/scripts/slim_and_merge.py -s /net/top/homes/psiddire/nano2pico/txt/slim_rules/zgmc.txt -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data_muon/2017/data/merged_zgmc_llg/merged_raw_pico_llg_DoubleMuon__Run2017F__UL2017_MiniAODv1_NanoAODv2-v1__280000__D530DEFA-84B9-FC43-95DF-FF7E4A103D68_zgmc_llg_nfiles_1.root -i /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data_muon/2017/data/skim_llg/*raw_pico_llg_DoubleMuon__Run2017F__UL2017_MiniAODv1_NanoAODv2-v1__280000__D530DEFA-84B9-FC43-95DF-FF7E4A103D68*.root
echo [Info] command_divider : End divided_command[0]
echo [Info] command_divider : Finished
