from os import system

# Calculating number of rows in both files for max value of threads to create
with open("in1.txt", "r") as file:
    for row1, line in enumerate(file):
        pass

with open("in2.txt", "r") as file:
    for col2, line in enumerate(file):
        pass

number_of_numbers_in_row = line.split(" ")
col1 = len(number_of_numbers_in_row)
row1 = row1 + 1
col2 = col2 + 1

print(f"Running for: {row1} X {col1} X {col2} matrices")

# Begin of Thread Count Iteration
system("gcc -pthread p1.c -o p1")
system("gcc -pthread p2.c -o p2")

curr_thread_cnt = 1
while curr_thread_cnt <= row1 * col2:
    system(
        f"./p1 {row1} {col1} {col2} in1.txt in2.txt out.txt {curr_thread_cnt} && ./p2 {curr_thread_cnt}"
    )
    curr_thread_cnt += 1

system("rm ./p1 ./p2")

# End of Thread Count Iteration
