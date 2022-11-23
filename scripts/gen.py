import numpy as np
from sys import argv

LONG_LONG_MAX = 9223372036854775807


def genMatrix():
    assert len(argv) == 4, "Incorrect argument number, usage: python gen.py i j k"
    i = int(argv[1])
    j = int(argv[2])
    mat = np.random.randint(0, 1000, size=(i, j))
    np.savetxt("in1.txt", mat, fmt="%i")

    k = int(argv[3])
    mat2 = np.random.randint(0, 1000, size=(j, k))
    np.savetxt("in2.txt", mat2, fmt="%i")

    matRes = np.matmul(mat, mat2)
    if (matRes > LONG_LONG_MAX).sum() > 0:
        genMatrix()
    np.savetxt("genOut.txt", matRes, fmt="%i")


if __name__ == "__main__":
    genMatrix()
