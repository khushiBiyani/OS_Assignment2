from random import randint
from os import system

for T in range(1, 11):
    i = randint(1, 5000)
    j = randint(1, 501)
    k = randint(1, 501)
    threadCount = randint(1, 51)
    if (
        system(
            f"echo -n '#{T} ' && python scripts/singleRun.py {i} {j} {k} {threadCount}"
        )
        != 0
    ):
        exit(-1)
