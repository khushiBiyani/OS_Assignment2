import numpy as np
from sys import argv

LONG_LONG_MAX =  9223372036854775807

def genMatrix():
    assert len(argv) == 4, "Incorrect argument number, usage: python gen.py i j k"
    i = int(argv[1])
    j = int(argv[2])
    mat = np.random.randint(0,1000,size=(i,j))
    print("Matrix 1:\n", mat)
    np.savetxt("in1.txt",mat,fmt='%i')

    k = int(argv[3])
    mat2 = np.random.randint(0,1000,size=(j,k))
    print("Matrix 2:\n", mat2)
    np.savetxt("in2.txt",mat2,fmt='%i')

    matRes = np.matmul(mat,mat2)
    print("Multiplied Matrix:\n", matRes)
    if (matRes>LONG_LONG_MAX).sum() > 0:
        genMatrix()
    np.savetxt("genOut.txt",matRes,fmt='%i')

    print(f"gcc p1.c -o p1 -pthread && gcc p2.c -o p2 && ./p1 {i} {j} {k} in1.txt in2.txt out.txt && ./p2 && rm ./p1 && rm ./p2")

if __name__ == "__main__":
    genMatrix()