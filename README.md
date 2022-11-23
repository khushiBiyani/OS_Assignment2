# OS Assignment $2$

Operating Systems (CS F372) Assignment (Post Midsem) @ BITS Pilani, Hyderabad Campus

# Program Workflow

1. Run `scripts/gen.py` to generate your matrices into `in1.txt` and `in2.txt` with their product being in `out.txt` Alternatively, you can manually type out your matrices into `in1.txt` and `in2.txt`

2. Run `scripts/transpose.py` to transpose the input in `in2.txt` to enable pre-processing.

3. Compile and run `scheduler.c` which will schedule `p1.c` and `p2.c` and compute the matrix product into `out.txt`.

# C Programs

### p1.c

To compile program $1$, use the command: `gcc -pthread p1.c -o p1`

To run program $1$, use the command: `./p1 i j k in1.txt in2.txt out.txt [MAXTHREADS]`

`in1.txt` contains a matrix of dimension $(i \times j)$

`in2.txt` contains a matrix of dimension $(j \times k)$

`out.txt` contains a matrix of dimension $(i \times k)$ and is the product of the above $2$ matrices.

`MAXTHREADS` is an **optional argument** which defaults to $1$ and indicates the number of threads **spawned** by `p1`.

### p2.c

To compile program $2$, use the command: `gcc p2.c -o p2`

To run program $2$, use the command: `./p2`

Currently, `p2` reads the $2$ matrices from the shared memory segment.

# Python Scripts

### gen.py

To run program $gen.py$, use the command: `python scripts/gen.py i j k`

A generator for the following tasks:

- A random matrix of size $(i \times j)$ into `in1.txt`

- A random matrix of size $(j \times k)$ into `in2.txt`

- Their matrix product into `genOut.txt`

### transpose.py

To run program $transpose.py$, use the command: `python scripts/transpose.py`

Reads the matrix in `in2.txt`, computes its transpose and stores it back into `in2.txt`.

### singleRun.py

To run program $singleRun.py$, use the command: `python scripts/singleRun.py i j k`

Here $i$, $j$ and $k$ denote the dimensions of the matrices in `in1.txt` and `in2.txt`.

`singleRun.py` automates running `p1` and `p2` against the inputs `i, j, k`.

### runnerP1.py

To run program $runnerP1.py$, use the command: `python scripts/runnerP1.py`

Automates the benchmarking of `p1.c` against given input files `in1.txt` and `in2.txt`, going over all thread values $t$ such that $1 \le t \le i+k$.

### plot.py

To run program $plot.py$, use the command: `python scripts/plot.py <csvfile-name>.csv`

Generates a scatter plot for the input CSV file.

# Authors

- [Khushi Biyani](https://www.github.com/?)
- [Manik Arneja](https://www.github.com/?)
- [Ankesh Pandey](https://www.github.com/?)
- [Khooshrin Aspi Pithawalla](https://www.github.com/?)
- [Kavyanjali Agnihotri](https://www.github.com/?)
- [Tushar Brijesh Chenan](https://www.github.com/?)
