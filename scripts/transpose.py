with open("in2.txt", "r") as file:
    data = file.read().strip().split("\n")

data = [x.split() for x in data]
data = [[int(col) for col in row] for row in data]
data = [list(i) for i in zip(*data)]

with open("in2.txt", "w") as file:
    for row in data:
        file.write(" ".join([str(x) for x in row]))
        file.write("\n")
