# SALT

SALT (**S**teiner sh**A**llow-**L**ight **T**ree) is for generating VLSI routing topology.
It trades off between path length (shallowness) and wirelength (lightness).
More details are in [ICCAD'17](https://doi.org/10.1109/ICCAD.2017.8203828) paper.

## Quick Start

The simplest way to build and run SALT is as follows.
~~~
$ git clone https://github.com/chengengjie/salt
$ cd salt
$ ./scripts/build.py -o release
$ cd run
$ ./minimal_salt ../toys/iccad17_example.net 1.0
~~~
If you have some questions, later sections may help.

## Building SALT

**Step 1:** Download the source codes. For example,
~~~
$ git clone https://github.com/chengengjie/salt
~~~

**Step 2:** Go to the project root and build by
~~~
$ cd salt
$ ./scripts/build.py -o release
~~~

Note that this will generate two folders under the root, `build` and `run` (`build` contains intermediate files for build/compilation, while `run` contains binaries and auxiliary files).
More details are in `scripts/build.py`.

### Dependencies

* g++ (version >= 5.4.0)
* CMake (version >= 3.5.1)
* Boost (version >= 1.58)
* Python (version 3)

## Runing SALT

### Toy

Go to the `run` directory and run binary `minimal_salt` with a toy net:
~~~
$ cd run
$ ./minimal_salt ../toys/iccad17_example.net <epsilon>
~~~
The epsilon is the parameter controlling the trade-off between shallowness and lightness.
The output will be stored in `SALT_R_iccad17_example.tree` file.
You can visualize it by
~~~
$ ../scripts/draw.py SALT_R_iccad17_example.tree
~~~
Besides, to compare with some other methods, you may run binary `eval_single_salt`:
~~~
$ cd run
$ ./eval_single_salt -net ../toys/iccad17_example.net -eps <epsilon>
~~~

### Batch Test

First, a file of input nets is needed.
The nets extracted from [ICCAD'15 Contest Problem B](https://doi.org/10.1109/ICCAD.2015.7372672) can be downloaded via [Dropbox](https://www.dropbox.com/sh/gcq1dh84ko9rjpz/AAAVT0pLZG_FMiOi0ORiKddva?dl=0).
For each input file, run binary `eval_batch_salt`, which constructs routing trees with several methods and epsilon values:
~~~
$ cd run
$ ./eval_batch_salt <nets_file> <eval_file_suffix>
~~~
It dumps the evaluation statistics into several files.
Each file is for a specific ranges of # pins, a specific method, and various epsilon values.

### Unit Test

Run the `build.py` with flag `-u` at the project root:
~~~
$ ./scripts/build.py -u
~~~

## Modules

* `scripts`: utility python scripts
* `src`: c++ source codes
    * `examples`: example application codes
    * `other_methods`: implemetation of other routing topology generation
    * `salt`: salt implementation
    * `unittest`: unit test
* `toys`: toy benchmarks

To use SALT in your own project, you only needs module `src/salt`.

## File Formats

### Net

~~~
Net <net_id> <net_name> <pin_num> [-cap]
0 x0 y0 [cap0]
1 x1 y1 [cap1]
...
~~~

### Tree

~~~
Tree <net_id> <net_name> <pin_num> [-cap]
0 x0 y0 -1 [cap0]
1 x1 y1 parent_idx1 [cap1]
2 x2 y2 parent_idx2 [cap2]
...
k xk yk parent_idxk
...
~~~
Notes:
* Tree nodes with indexes [0, pin_num) are pins, others are Steiner.
* Steiner nodes have no capacitance.