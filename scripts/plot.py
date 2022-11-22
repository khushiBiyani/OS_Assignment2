import matplotlib.pyplot as plt
import csv
from sys import argv


def make_plot():
    assert len(argv) == 2, "Incorrect usage: python plot.py <csv-file-name>.csv"
    filename = str(argv[1])

    x = []  # number of threads, first column in csv
    y = []  # time(in nanoseconds), second column in csv
    data = []

    with open(filename, "r") as csvfile:
        data = list(csv.reader(csvfile, delimiter=","))
        for row in data[1:]:
            x.append(int(row[0]))
            y.append(float(row[1]))

    # scatterplot
    plt.scatter(x, y, color="g", marker="o")
    plt.xlabel(data[0][0])
    plt.ylabel(data[0][1])
    plt.title("Matplotlib Plots", fontsize=21)
    plt.show()


if __name__ == "__main__":
    make_plot()
