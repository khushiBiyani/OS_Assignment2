import matplotlib.pyplot as plt
import csv
from sys import argv

# name csvfile as "p1" for program p1 and "p2" for program p2

def make_plot():
    assert len(argv) == 2, "Incorrect usage: python plot.py <csv-file-name>.csv"
    filename = str(argv[1])

    x = []          # number of threads, first column in csv
    y = []          # time(in nanoseconds), second column

    with open(filename + '.csv', 'r') as csvfile:
        data = csv.reader(csvfile, delimiter=',')
        for row in data:
            x.append(int(row[0]))
            y.append(float(row[1]))

    # scatterplot
    plt.scatter(x, y, color = 'g', marker = 'o')
    plt.xlabel('Number of Threads')
    if(filename == 'p1'):
        plt.ylabel('Time(in nanoseconds) to read the input files')
    elif(filename == 'p2'):
        plt.ylabel('Time(in nanoseconds) to compute the product')
    else:
        plt.ylabel('Time(in nanoseconds)')
    plt.title('Execution Time', fontsize=21)
    # plt.grid()
    plt.show()

if __name__ == "__main__":
    make_plot()
