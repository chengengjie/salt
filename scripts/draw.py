#!/usr/bin/env python3

import argparse
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser(description='Draw Nets/Trees')
parser.add_argument('input_filenames', nargs='+')
parser.add_argument('-a', '--anno_text', choices=['no', 'pin_only', 'all'], default='pin_only')
parser.add_argument('-s', '--scale', type=float, default=1.0)
args = parser.parse_args()

types = ['Net', 'Tree']

class Node:
    def __init__(self, line, withParent = False):
        tokens = line.split()
        self.id, self.x, self.y = int(tokens[0]), float(tokens[1]), float(tokens[2])
        if withParent:
            self.parent_id = int(tokens[3])

for filename in args.input_filenames:
    with open(filename) as file:
        type = None
        line = file.readline()
        while line is not '':
            for t in types:
                if t in line:
                    type = t
                    break
            if type is not None:
                break
            line = file.readline()
        
        if type is 'Net' or type is 'Tree':
            # header
            tokens = line.split()
            net_id, net_name, num_pins = int(tokens[1]), tokens[2], int(tokens[3])
            nodes = []
            line = file.readline()
            while line is not '':
                nodes.append(Node(line, type is 'Tree')) # (id, x, y, parent_id)
                line = file.readline()
            plt.figure(figsize=(6, 6))

            # edges
            if type is 'Tree':
                for node in nodes:
                    if node.id is 0:
                        continue
                    parent = nodes[node.parent_id]
                    plt.plot([node.x, parent.x], [node.y, parent.y], c='k')

            # nodes
            for node in nodes:
                if node.id is 0: # source
                    plt.plot(node.x, node.y, c='r', marker='s', mew=0, ms=6 * args.scale)
                elif node.id < num_pins: # sinks
                    plt.plot(node.x, node.y, c='k', marker='s', mew=0, ms=4 * args.scale)
                else: # Steiner points
                    plt.plot(node.x, node.y, c='k', marker='o', mew=0, ms=3 * args.scale)
                # anno text
                if args.anno_text == 'no':
                    continue
                if node.id < 0:
                    continue
                if args.anno_text == 'pin_only' and node.id >= num_pins:
                    continue
                plt.text(node.x + .5, node.y, str(node.id), fontsize=8)

            # format & save
            plt.axis('square')
            plt.axis('off')
            plt.savefig('{}.pdf'.format(filename), bbox_inches='tight')
        else:
            print("Unknown type: ", type)