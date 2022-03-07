export INDIR=/homes/psiddire/nano2pico/out/
export INFILE=GluGluHToZG_M-125_TuneCP5_13TeV-powheg-pythia8__RunIISummer20UL16NanoAODv9__100.root
./compile.sh && ./run/process_nano.exe --in_file $INFILE --in_dir $INDIR --out_dir out/
export INFILE=GluGluHToZG_M-125_TuneCP5_13TeV-powheg-pythia8__RunIISummer20UL16NanoAODv9__900.root
./compile.sh && ./run/process_nano.exe --in_file $INFILE --in_dir $INDIR --out_dir out/
