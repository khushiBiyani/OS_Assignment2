import matplotlib.pyplot as plt
import csv
import numpy
from sys import argv

# in line 26, change the degree(in code is = 17) of polynomial line as per your requirement
# if the plot on exiting says "RankWarning: Polyfit may be poorly conditioned" that basically means use a smaller degree


def make_plot():
    assert (
        len(argv) == 3
    ), "Incorrect usage: python plot.py <csv-file-name>.csv '<Plot Title>'"
    fileName = str(argv[1])
    plotTitle = str(argv[2])

    x = []
    y = []
    data = []

    with open(fileName, "r") as csvfile:
        data = list(csv.reader(csvfile, delimiter=","))
        for row in data[1:]:
            x.append(int(row[0]))
            y.append(float(row[1]))

    end = max(x)

    polyModel = numpy.poly1d(numpy.polyfit(x, y, 12))
    polyLine = numpy.linspace(1, end)

    # scatterplot
    plt.scatter(x, y, color="b", marker=".")

    # regression curve
    plt.plot(polyLine, polyModel(polyLine), color="r")

    plt.xlabel(data[0][0])
    plt.ylabel(data[0][1])
    plt.title(plotTitle, fontsize=21)

    plt.show()


if __name__ == "__main__":
    make_plot()
