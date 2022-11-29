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
system("gcc group2_assignment2.c -o group2_assignment2.out")

system(f"./group2_assignment2.out {i} {j} {k} in1.txt in2.txt out.txt {threadCount}")

with open("genOut.txt", "r") as file:
    genData = file.read().strip().split("\n")
genData = [x.split() for x in genData]
genData = [[int(y) for y in x] for x in genData]

with open("out.txt", "r") as file:
    data = file.read().strip().split("\n")
data = [x.split() for x in data]
data = [[int(y) for y in x] for x in data]

if data != genData:
    print(f"Test Case (I: {i}, J: {j}, K: {k}, MAXTHREADS: {threadCount}) -> [WA]")
    exit(-1)
else:
    print(f"Test Case (I: {i}, J: {j}, K: {k}, MAXTHREADS: {threadCount}) -> [AC]")
    system("rm ./p1 ./p2 ./group2_assignment2.out *.txt")
