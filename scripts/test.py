from os import system
from random import randint

for T in range(100):
    i = randint(1, 100)
    j = randint(1, 100)
    k = randint(1, 100)
    threadCount = randint(1, 100)

    system(f"python scripts/singleRun.py {i} {j} {k} {threadCount} ")

    with open("genOut.txt", "r") as file:
        genData = file.read().strip().split("\n")

    genData = [x.split() for x in genData]
    genData = [[int(y) for y in x] for x in genData]

    with open("out.txt", "r") as file:
        data = file.read().strip().split("\n")

    data = [x.split() for x in data]
    data = [[int(y) for y in x] for x in data]

    if data == genData:
        print(f"Test Case #{T+1}: [OK]")
    else:
        print(f"Test Case #{T+1}: [WA]")
        exit(-1)
