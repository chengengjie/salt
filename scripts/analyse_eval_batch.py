#!/usr/bin/env python3

import argparse
import numpy as np

pin_types = ['pin4', 'pin8', 'pin16', 'pin32', 'all']
case_ids = [1, 3, 4, 5, 7, 10, 16, 18]

parser = argparse.ArgumentParser()
parser.add_argument('tree_type')
args = parser.parse_args()

eval_stats = []  # <pin_type, epsilon, metric> -> score
count_stats = np.zeros((len(pin_types), len(case_ids)), dtype = int)  # <pin_type, metric> -> # nets
for (i, pin_type) in enumerate(pin_types):
    eval_stat = None
    total_count = 0
    for (j, case_id) in enumerate(case_ids):
        # read
        file = '{}_{}_superblue{}.eval'.format(args.tree_type, pin_type, case_id)
        data = np.loadtxt(file)  # <epsilon, metric> -> value
        if len(data.shape) == 1:
            data.shape = (1, data.size)
        # accumulate
        count_data = data[:,5]
        if any([count != count_data[0] for count in count_data]):
            print('Inconsistent count in', file)
        count_stats[i, j] = count_data[0]
        eval_data = data[:,0:5]
        eval_data *= count_stats[i, j]
        if j == 0:
            eval_stat = eval_data
        else:
            eval_stat += eval_data
    # average and append
    eval_stat /= np.sum(count_stats[i, :])
    eval_stats.append(eval_stat)

# print count stat
print('Below is the count stat for checking missing eval stat')
print(count_stats)
print(pin_types, np.sum(count_stats, axis=1))

# print eval stat
eval_stats = np.stack(eval_stats)
print(' '.join([metric + pin_type for pin_type in pin_types for metric in ['wl_', 'pl_', 'delay_']]))
for j in range(eval_stats.shape[1]):  # epsilon
    for i in range(eval_stats.shape[0]):  # pin
        for k in [0,1,3]:  # metric
            print(eval_stats[i,j,k], end=' ')
    print('')