import numpy as np 
import pandas as pd 
from matplotlib import pyplot as plt 
import seaborn as sns 
from sklearn.linear_model import LinearRegression

times_df = pd.read_csv('Name of CSV file.csv') 
times_df.head()
times_df.describe()


# Data prep

X = times_df.iloc[:, :-1].values 
y = times_df.iloc[:, 1].values 


#splitting our data set to tests and training

from sklearn.model_selection import train_test_split 
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=0)  # test size might vary, 1/4 or 1.5 or 1/3


#training
from sklearn.linear_model import LinearRegression 

regressor = LinearRegression() 
regressor.fit(X_train, y_train) 


#predictions

y_pred = regressor.predict(X_test)


#reglinesplotfinal

plt.scatter(X_train, y_train,color='black') 
plt.plot(X_test, y_pred,color='red') 
plt.show()
