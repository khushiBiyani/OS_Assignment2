from os import system
from random import randint

for _ in range(1, 101):
    i = randint(1, 100001)
    j = randint(1, 11)
    k = randint(1, 11)
    threadCount = randint(1, 21)
    system(
        f"python3 scripts/gen.py {i} {j} {k} && python3 scripts/transpose.py &&  gcc -pthread p1.c -o p1 && gcc -pthread p2.c -o p2 && gcc schedulerfinal.c -o sched && ./sched {i} {j} {k} in1.txt in2.txt out.txt {threadCount} "
    )
    with open("genOut.txt", "r") as file:
        genData = file.read().strip().split("\n")
    genData = [x.split() for x in genData]
    genData = [[int(y) for y in x] for x in genData]
    with open("out.txt", "r") as file:
        data = file.read().strip().split("\n")
    data = [x.split() for x in data]
    data = [[int(y) for y in x] for x in data]
    if data != genData:
        print(f"Test Case #{_}: [WA]")
        print(i, j, k, threadCount)
        exit(-1)
    else:
        print(f"Test Case #{_}: [AC]")
