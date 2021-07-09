export INDIR=/net/cms17/cms17r0/pico/NanoAODv7/nano/2017/mc
export INFILE=DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8__RunIIFall17NanoAODv7__PU2017RECOSIMstep_12Apr2018_Nano02Apr2020_102X_mc2017_realistic_v8-v1__130000__1B0B0463-0E98-B241-B61F-00036CA24B7B.root
./compile.sh && ./run/process_nano.exe --in_file $INFILE --in_dir $INDIR --out_dir /net/cms17/cms17r0/pico/NanoAODv7/zgamma_mc/2017/mc

