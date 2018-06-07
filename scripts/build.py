#!/usr/bin/env python3

import argparse, os

all_targets = ['unittest_salt', 'minimal_salt', 'eval_single_salt', 'eval_batch_salt']
run_files = 'src/salt/base/flute/POST9.dat src/salt/base/flute/POWV9.dat'

def run(command):
    if args.print_commands:
        print(command)
    if os.system(command) is not 0:
        if not args.print_commands:
            print(command)
        quit()

# argparse
parser = argparse.ArgumentParser(description='Build SALT in Linux')
parser.add_argument('-t', '--targets', choices=all_targets, nargs='+')
parser.add_argument('-o', '--mode', choices=['debug', 'release', 'profile'], help='start with an empty space')
parser.add_argument('-c', '--cmake_options', default='', help='start with an empty space')
parser.add_argument('-m', '--make_options', default='-j 6', help='start with an empty space')
parser.add_argument('-u', '--unittest', action='store_true')
parser.add_argument('-p', '--print_commands', action='store_true')
parser.add_argument('-b', '--build_dir', default='build')
parser.add_argument('-r', '--run_dir', default='run')
args = parser.parse_args()

# targets
if args.unittest:
    build_targets = ['unittest_salt']
elif args.targets is None:
    build_targets = ['']
else:
    build_targets = args.targets

# cmake & copy run files
mode_cmake_options = {
    None        : '',
    'debug'     : '-DCMAKE_BUILD_TYPE=Debug',
    'release'   : '-DCMAKE_BUILD_TYPE=Release',
    'profile'   : '-DCMAKE_CXX_FLAGS=-pg',
}
run('cmake -B{} -Hsrc {} {}'.format(args.build_dir, mode_cmake_options[args.mode], args.cmake_options))
run('mkdir -p {}'.format(args.run_dir))
run('cp -u {} {}'.format(run_files, args.run_dir))

# make
for target in build_targets:
    run('cmake --build {} --target {} -- {}'.format(args.build_dir, target, args.make_options))
cp_targets = all_targets if build_targets == [''] else build_targets
for target in cp_targets:
    run('cp -u {}/{} {}'.format(args.build_dir, target, args.run_dir))

# unit test
if args.unittest:
    root_dir = os.path.curdir
    os.chdir(args.run_dir)
    run('./{}'.format('unittest_salt'))
    os.chdir(root_dir)