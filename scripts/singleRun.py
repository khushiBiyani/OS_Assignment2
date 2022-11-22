from os import system
from sys import argv

assert len(argv) == 4

i = int(argv[1])
j = int(argv[2])
k = int(argv[3])

system(f"python scripts/gen.py {i} {j} {k}")
system("python scripts/transpose.py")

for n in range(1, 3):
    system(f"gcc -pthread p{n}.c -o p{n}")

system(f"./p1 {i} {j} {k} in1.txt in2.txt out.txt && ./p2")

system("rm p1.csv")

for n in range(1, 3):
    system(f"rm ./p{n}")
