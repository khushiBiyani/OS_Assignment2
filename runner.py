from os import system

genMatrices = "python gen.py %d %d %d"

command = "gcc -pthread p1.c -o p1 && gcc p2.c -o p2 && ./p1 %d %d %d in1.txt in2.txt out.txt %d && ./p2 > /dev/null && rm ./p1 ./p2"

i = 5000
j = 1
k = 10000

system(genMatrices % (i, j, k))

for threadCount in range(1, 21):
    system(command % (i, j, k, threadCount))
