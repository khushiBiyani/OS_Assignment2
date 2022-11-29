# OS Assignment $2$

Operating Systems (CS F372) Assignment (Post Midsem) @ BITS Pilani, Hyderabad Campus

# Process Workflow

1. Run `scripts/gen.py` to generate your matrices into `in1.txt` and `in2.txt` with their product being in `genOut.txt` Alternatively, you can manually type out your matrices into `in1.txt` and `in2.txt`

2. Run `scripts/transpose.py` to transpose the input in `in2.txt` to enable pre-processing.

3. Compile `group2_assignment2.c` into `./group2_assignment2.out`, `p1.c` into `./p1` and `p2.c` into `./p2`.

4. `./group2_assignment2.out` will schedule `p1.c` and `p2.c` through a $Round \ Robin$ mechanism and compute the matrix product into `out.txt`.

# C Programs

### group2_assignment2.c

To compile $group2_assignment2.c$, use the command: `gcc group2_assignment2.c -o group2_assignment2.out`

To run $group2_assignment2.c$, use the command: `./group2_assignment2.out i j k in1.txt in2.txt out.txt [MAXTHREADS]`

`in1.txt` contains a matrix of dimension $(i \times j)$

`in2.txt` contains a matrix of dimension $(j \times k)$

`out.txt` contains a matrix of dimension $(i \times k)$ and is the product of the above $2$ matrices.

`MAXTHREADS` is an **optional argument** which defaults to $1$ and indicates the number of threads **spawned** by both `p1` and `p2`.

### p1.c

To compile $p1.c$, use the command: `gcc -pthread p1.c -o p1`

To run $p1.c$, use the command: `./p1 i j k in1.txt in2.txt out.txt [MAXTHREADS]`

> `p1.c` is responsible for using multi-threading to read the files `in1.txt` and `in2.txt` into a shared memory segment.

### p2.c

To compile $p2.c$, use the command: `gcc -pthread p2.c -o p2`

To run $p2.c$, use the command: `./p2 [MAXTHREADS]`

> `p2` reads the $2$ matrices from the shared memory segment and computes their product with multithreading. After that, it writes the product matrix into `out.txt`.

# Python Scripts

### gen.py

To run program $gen.py$, use the command: `python scripts/gen.py i j k`

> A generator for the following tasks:
>- A random matrix of size $(i \times j)$ into `in1.txt`
>- A random matrix of size $(j \times k)$ into `in2.txt`
>- Their matrix product into `genOut.txt`

### transpose.py

To run program $transpose.py$, use the command: `python scripts/transpose.py`

> Reads the matrix in `in2.txt`, computes its transpose and stores it back into `in2.txt`.

### singleRun.py

To run program $singleRun.py$, use the command: `python scripts/singleRun.py i j k [MAXTHREADS]`

$i$, $j$ and $k$ denote the dimensions of the matrices in `in1.txt` and `in2.txt`.

> `singleRun.py` automates running `./group2_assignment2.out` against the inputs `i, j, k, MAXTHREADS` and verifies the output of `out.txt` against `genOut.txt`.

### runner.py

> **IMPORTANT:** To generate the CSV file for `p1.c` and `p2.c`, you must uncomment the code block at the end of the file which is responsible for writing into the `.csv` file.

Make sure you have run `python scripts/transpose.py`

To run program $runner.py$, use the command: `python scripts/runner.py`

> Automates the benchmarking of `p1.c` and `p2.c` against given input files `in1.txt` and `in2.txt`, going over all thread values $t$ such that $1 \le t \le i \times k$.

### plot.py

To run program $plot.py$, use the command: `python scripts/plot.py <csvfile-name>.csv <PlotTitle>`

> Generates a scatter plot for the input CSV file and applies regression to find the best fit curve against the data.

# Authors

- [Khushi Biyani](https://www.github.com/?)
- [Manik Arneja](https://www.github.com/?)
- [Ankesh Pandey](https://www.github.com/?)
- [Khooshrin Aspi Pithawalla](https://www.github.com/?)
- [Kavyanjali Agnihotri](https://www.github.com/?)
- [Tushar Brijesh Chenan](https://www.github.com/?)
