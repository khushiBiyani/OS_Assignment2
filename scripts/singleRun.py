from os import system
from sys import argv

assert len(argv) == 4 or len(argv) == 5

i = int(argv[1])
j = int(argv[2])
k = int(argv[3])
threadCount = 1

if len(argv) == 5:
    threadCount = int(argv[4])

system(f"python scripts/gen.py {i} {j} {k}")
system("python scripts/transpose.py")

for n in range(1, 3):
    system(f"gcc -pthread p{n}.c -o p{n}")

system(f"./p1 {i} {j} {k} in1.txt in2.txt out.txt {threadCount} && ./p2 {threadCount}")

for n in range(1, 3):
    system(f"rm ./p{n}")
