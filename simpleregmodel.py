import numpy as np
import matplotlib.pyplot as plt
import pandas as pd


 #csv name whatever was generated from the python script, replace < > with that
datasets = pd.read_csv('<name of plot generated from respective p1 or p2>.csv)
X = datasets.iloc[:, :-1].values
Y = datasets.iloc[:, 1].values

# Splitting the dataset into the Training set and Test set

from sklearn.model_selection import train_test_split
X_Train, X_Test, Y_Train, Y_Test = train_test_split(X, Y, test_size =1/'n', random_state = 0) # 1/n is analagous to the descent of regression for say, 1/4 or 1/5, generally 1/3 hota hai but not sure yaha kya lenge

# Fitting Simple Linear Regression to the training set

from sklearn.linear_model import LinearRegression
regressor = LinearRegression()
regressor.fit(X_Train, Y_Train)

# Predicting the Test set result ￼

Y_Pred = regressor.predict(X_Test)


# Visualising the Training set results

plt.scatter(X_Train, Y_Train, color = 'black')
plt.plot(X_Train, regressor.predict(X_Train), color = 'red')
plt.title('Time vs Number of threads (Training Set)')
plt.xlabel('Capped Number of threads in P1')
plt.ylabel('Time')
plt.show()

# Visualising the Test set results for the reg model

plt.scatter(X_Test, Y_Test, color = 'black')
plt.plot(X_Train, regressor.predict(X_Train), color = 'red')
plt.title('Time vs Number of threads (Training Set)')
plt.xlabel('Capped number of threads in P1')
plt.ylabel('Time')
plt.show()
