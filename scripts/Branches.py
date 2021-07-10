import argparse

parser = argparse.ArgumentParser()
parser.add_argument('input_file')
args = parser.parse_args()

with open(args.input_file, "r") as f:
    lines = f.readlines()
with open(args.input_file, "w") as f:
    for line in lines:
        if "*Br" in line and "HLT" in line:
            f.write(line)

