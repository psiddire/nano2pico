#!/bin/bash
WORK_DIR=$PWD
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /net/cms29/cms29r0/pico/CMSSW_10_2_11_patch1/src
eval `scramv1 runtime -sh`
cd $WORK_DIR
echo [Info] command_divider : Start divided_command[0]
echo [Info] command_divider : Current directory: /net/top/homes/psiddire/nano2pico
echo [Info] command_divider : command: /net/top/homes/psiddire/nano2pico/run/process_nano.exe -f DoubleEG__Run2017F__UL2017_MiniAODv1_NanoAODv2-v1__50000__874B57D6-E42B-6244-ADF9-39EDD548F84D.root -i /net/cms17/cms17r0/pico/NanoAODv2/nano/2017/data/ -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data/2017/data/
/net/top/homes/psiddire/nano2pico/run/process_nano.exe -f DoubleEG__Run2017F__UL2017_MiniAODv1_NanoAODv2-v1__50000__874B57D6-E42B-6244-ADF9-39EDD548F84D.root -i /net/cms17/cms17r0/pico/NanoAODv2/nano/2017/data/ -o /net/cms17/cms17r0/pico/NanoAODv2/zgamma_data/2017/data/
echo [Info] command_divider : End divided_command[0]
echo [Info] command_divider : Finished
