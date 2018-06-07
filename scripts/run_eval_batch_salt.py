#!/usr/bin/env python3

from run_base import *
import argparse
import datetime

parser = argparse.ArgumentParser()
parser.add_argument('input_files', nargs='+', metavar='<.nets>')
parser.add_argument('-m', '--mode', choices=modes)
args = parser.parse_args()

# constants
binary = 'eval_batch_salt'

# mode prefix
prefix = mode_prefixes[args.mode]

# run
now = datetime.datetime.now()
log_dir = 'batch_{:02d}{:02d}'.format(now.month, now.day)
run('mkdir -p {}'.format(log_dir))
print('The following input files will be ran:')
print('\n'.join(args.input_files))
print()
for input_file in args.input_files:
    file_name, file_ext = os.path.splitext(os.path.basename(input_file))
    assert(file_ext == '.nets')
    run('{0} ./{1} {2} {3}.eval | tee {3}.log'.format(prefix,
                                                      binary, input_file, file_name))
    if args.mode == 'gprof':
        run('gprof {} > {}.gprof'.format(binary, file_name))
run('mv *.eval *.log {}'.format(log_dir))
