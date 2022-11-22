import numpy as np
import subprocess

# Calculating number of rows in both files for max value of threads to create
with open("in2.txt", 'r') as file:
    for col2, line in enumerate(file):
        pass

with open("in1.txt", 'r') as file:
    for row1, line in enumerate(file):
        pass

file.close()

number_of_numbers_in_row=line.split(' ')
col1=len(number_of_numbers_in_row)
print(col1)

# Begin of Thread Count Iteration
row1 = row1 + 1
col2 = col2 + 1
col1 = col1 + 1
subprocess.call(["gcc", "-pthread","p1.c","-o", "p1"])
curr_thread_cnt=1
while (curr_thread_cnt <= row1+col2):
    #cmd = "./p1.out "+ str(row1) + " " + str(col1) + " " + str(row2) +" in1.txt in2.txt out.txt "+ str(curr_thread_cnt)
    #tmp=subprocess.call([cmd])
    tmp=subprocess.call(["./p1",str(row1),str(col1),str(col2),"in1.txt","in2.txt","out.txt",str(curr_thread_cnt)])
    curr_thread_cnt = curr_thread_cnt + 1

# End of Thread Count Iteration