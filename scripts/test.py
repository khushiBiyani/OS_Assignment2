from random import randint
from os import system

for T in range(1, 11):
    i = randint(1, 50001)
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


# Uncomment to generate CSV for Turnaround Time VS Workload
# threadCount=1
# for t in range(0,1000):
#     i = randint(1, 1001)
#     j = randint(1, 1001)
#     k = randint(1, 1001)
#     if (
#         system(
#             f"python3 scripts/singleRun.py {i} {j} {k} {threadCount}"
#         )
#         != 0
#     ):
#         exit(-1)
#     else:
#         print(f"{i}\t{j}\t{k}\n")
