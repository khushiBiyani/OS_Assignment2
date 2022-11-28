import pandas as pd
import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression
from sklearn.model_selection import train_test_split
from sys import argv


def make_plot():
    assert (
        len(argv) == 3
    ), "Incorrect usage: python plot.py <csv-file-name>.csv '<Plot Title>'"
    fileName = argv[1]
    plotTile = argv[2]

    df = pd.read_csv(fileName)

    X = df.iloc[:, :-1].values
    Y = df.iloc[:, 1].values

    X_train, X_test, Y_train, _ = train_test_split(X, Y, test_size=0.3, random_state=0)

    regressor = LinearRegression()
    regressor.fit(X_train, Y_train)

    Y_pred = regressor.predict(X_test)

    plt.scatter(X_train, Y_train, color="g", marker="o")
    plt.plot(X_test, Y_pred, color="red")

    labels = list(df)
    plt.xlabel(labels[0])
    plt.ylabel(labels[1])

    plt.title(plotTile, fontsize=21)

    plt.show()


if __name__ == "__main__":
    make_plot()
