import numpy as np
from sys import argv

LONG_LONG_MAX = 9223372036854775807


def main():
    assert len(argv) == 4, "Incorrect argument number, usage: python gen.py i j k"
    i = int(argv[1])
    j = int(argv[2])
    k = int(argv[3])

    lim = 1e6

    mat = np.random.randint(-lim, lim, size=(i, j))
    mat2 = np.random.randint(-lim, lim, size=(j, k))
    matRes = np.matmul(mat, mat2)

    if (matRes > LONG_LONG_MAX).sum() > 0:
        main()

    np.savetxt("in1.txt", mat, fmt="%i")
    np.savetxt("in2.txt", mat2, fmt="%i")
    np.savetxt("genOut.txt", matRes, fmt="%i")
    exit(0)


if __name__ == "__main__":
    main()
