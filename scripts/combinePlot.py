import matplotlib.pyplot as plt
import csv
import numpy
from sys import argv

# ASSUMPTION : number of entries is same for both the csv files
# in line 44, 47, change the degree(in code is = 3) of polynomial line as per your requirement
# if the plot on exiting says "RankWarning: Polyfit may be poorly conditioned" that basically means use a smaller degree


def make_plot():
    assert (
        len(argv) == 4
    ), "Incorrect usage: python combinePlot.py <csv-file(1ms)>.csv <csv-file(2ms)>.csv <Plot Title>"
    file1 = str(argv[1])
    file2 = str(argv[2])
    plotTitle = str(argv[3])

    x1 = []
    y1 = []
    x2 = []
    y2 = []
    data = []

    with open(file1, "r") as csvfile:
        data = list(csv.reader(csvfile, delimiter=","))
        for row in data[1:]:
            x1.append(int(row[0]))
            y1.append(float(row[1]))

    with open(file2, "r") as csvfile:
        data = list(csv.reader(csvfile, delimiter=","))
        for row in data[1:]:
            x2.append(int(row[0]))
            y2.append(float(row[1]))

    # scatterplots
    plt.scatter(x1, y1, color="g", marker="o", label="Time quanta : 1ms")
    plt.scatter(x2, y2, color="mediumblue", marker="o", label="Time quanta : 2ms")

    # regression curves
    start = min(min(x1), min(x2))
    end = max(max(x1), max(x2))

    polyModel1 = numpy.poly1d(numpy.polyfit(x1, y1, 3))
    polyLine1 = numpy.linspace(start, end)

    polyModel2 = numpy.poly1d(numpy.polyfit(x2, y2, 3))
    polyLine2 = numpy.linspace(start, end)

    plt.plot(
        polyLine1, polyModel1(polyLine1), color="greenyellow", label="Reg curve : 1ms"
    )
    plt.plot(polyLine2, polyModel2(polyLine2), color="aqua", label="Reg curve : 2ms")

    plt.xlabel(data[0][0])
    plt.ylabel(data[0][1])
    plt.title(plotTitle, fontsize=21)

    # legend
    plt.legend()

    plt.show()


if __name__ == "__main__":
    make_plot()
