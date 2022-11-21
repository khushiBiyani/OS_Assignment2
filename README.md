# OS Assignment $2$

Operating Systems (CS F372) Assignment (Post Midsem) @ BITS Pilani, Hyderabad Campus

# Running Locally

## p1.c

To compile program $1$, use the command: `gcc -pthread p1.c -o p1`

To run program $1$, use the command:

`./p1 i j k in1.txt in2.txt out.txt [MAXTHREADS]`

`in1.txt` contains a matrix of dimension $(i \times j)$

`in2.txt` contains a matrix of dimension $(j \times k)$

`out.txt` contains a matrix of dimension $(i \times k)$ and is the product of the above $2$ matrices.

`MAXTHREADS` is an **optional argument** which defaults to $1$ and indicates the number of threads **spawned** by `p1`.

## p2.c

To compile program $2$, use the command: `gcc p2.c -o p2`

To run program $2$, use the command:

`./p2`

Currently, `p2` reads the $2$ matrices from the shared memory segment.

## gen.py

`python gen.py i j k`

A generator for the following tasks:

- A random matrix of size $(i \times j)$ into `in1.txt`

- A random matrix of size $(j \times k)$ into `in2.txt`

- Their matrix product into `genOut.txt`

## plot.py

`python plot.py <csvfile-name>.csv`

Program to make scatter plot for the input CSV file.


# Authors

- [Khushi Biyani](https://www.github.com/?)
- [Manik Arneja](https://www.github.com/?)
- [Ankesh Pandey](https://www.github.com/?)
- [Khooshrin Aspi Pithawalla](https://www.github.com/?)
- [Kavyanjali Agnihotri](https://www.github.com/?)
- [Tushar Brijesh Chenan](https://www.github.com/?)
