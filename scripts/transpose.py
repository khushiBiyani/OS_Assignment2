import numpy as np

# Reading the matrix from file 2
input_matrix = np.loadtxt("in2.txt", dtype="i")

# Tranposing the matrix
transpose_matrix = input_matrix.transpose()

# Saving the array in a text file
file = open("in2.txt", "w")
for row in transpose_matrix:
    for element in row:
        file.write(str(element))
        file.write(" ")
    file.write("\n")
file.close()
