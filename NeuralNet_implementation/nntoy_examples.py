from nntoy import BasicNeuralNet
from sklearn.datasets import make_moons, make_circles
from sklearn.cross_validation import train_test_split
import numpy as np
import matplotlib.pylab as plt
import os
import itertools
from sklearn.datasets import fetch_mldata
import cPickle as pickle
import subprocess


def generate_biclass_data(data_type, random_state):
    """ Generate biclass data to classify

    arg : data_type (str) possible type of data
            choose any in ["lin_sep", "non_lin_sep", "overlap"]
            'lin_sep' : Bi-class, linearly separable data
            'non_lin_sep' : Bi-class, non linearly separable data
            'overlap' : Bi-class, non linearly separable data with class overlap

        random_state (int) seed for numpy.random
    """

    # Set seed for reproducible results
    np.random.seed(random_state)

    # Case 1 : linearly separable data
    if data_type == "lin_sep":
        mean1 = np.array([0, 2])
        mean2 = np.array([2, 0])
        cov = np.array([[0.8, 0.6], [0.6, 0.8]])
        X1 = np.random.multivariate_normal(mean1, cov, 100)
        y1 = np.ones(len(X1))
        X2 = np.random.multivariate_normal(mean2, cov, 100)
        y2 = np.ones(len(X2)) * -1
        X = np.vstack((X1, X2))
        y = np.hstack((y1, y2))

    # Case 2 : non -linearly separable data
    elif data_type == "moons":
        X, y = make_moons(n_samples=200, noise=0.2)

    elif data_type == "circles":
        X, y = make_circles(n_samples=200, noise=0.2, factor=0.5)

    # Case 3 : data with overlap between classes
    elif data_type == "overlap":
        mean1 = np.array([0, 2])
        mean2 = np.array([2, 0])
        cov = np.array([[1.5, 1.0], [1.0, 1.5]])
        X1 = np.random.multivariate_normal(mean1, cov, 100)
        y1 = np.ones(len(X1))
        X2 = np.random.multivariate_normal(mean2, cov, 100)
        y2 = np.ones(len(X2)) * -1
        X = np.vstack((X1, X2))
        y = np.hstack((y1, y2))

    assert(X.shape[0] == y.shape[0])

    # Format target to: -1 / +1
    targets = set(y.tolist())
    t1 = min(targets)
    t2 = max(targets)
    l1 = np.where(y < t2)
    l2 = np.where(y > t1)
    y[l1] = -1
    y[l2] = 1

    return X, y


def get_mnist_data():

    if not os.path.isfile("./Data/mnist.pkl"):
        subprocess.call("gunzip -k ./Data/mnist.pkl.gz".split(" "))
    with open("./Data/mnist.pkl", "r") as f:
        return pickle.load(f)


def format_mnist(y):
    """ Convert the 1D to 10D """

    # convert to 10 categeories
    y_new = np.zeros((10, 1))
    y_new[y] = 1
    return y_new

    # X_new, y_new = [], []

    # for i in range(X.shape[0]):
    #     X_new.append(X[i,:])
    #     ytemp = np.zeros(10)
    #     ytemp[y[i]] = 1
    #     y_new.append(ytemp)

    # return X_new, y_new


def plot_classification(d_NN, data_type, random_state):

    if data_type == "mnist":
        train, test, valid = get_mnist_data()
        # X_train, y_train = train[0], train[1] #format_mnist(train[1])
        # X_train, y_train = format_mnist(X_train, y_train)
        training_inputs = [np.reshape(x, (784, 1)) for x in train[0]]
        training_results = [format_mnist(y) for y in train[1]]
        training_data = zip(training_inputs, training_results)
        test_fm = []
        for i in range(len(test[0])):
            test_fm.append((test[0][i], test[1][i]))
        test = test_fm

    else:
        X, y = generate_biclass_data(data_type, random_state)
        # Train / Test split
        X_train, X_test, y_train, y_test = train_test_split(
            X, y, test_size=0.1, random_state=random_state)

    # Create NN model and fit
    NeuralNet = BasicNeuralNet.BasicNeuralNet(
        d_NN["sizes"],
        lmbda = d_NN["lambda"],
        actfuncname=d_NN["actfuncname"],
        costfuncname=d_NN["costfuncname"],
        verbose=d_NN["verbose"])

    NeuralNet.fit_SGD(
        training_data,
        d_NN["learning_rate"],
        d_NN["batch_size"],
        d_NN["epochs"],
        test_data=test)


if __name__ == "__main__":

    # Create repository and data files if needed
    if not os.path.exists("./Figures/"):
        os.makedirs("./Figures/")

    # Random seed for reproducible results
    random_state = 20
    np.random.seed(random_state)

    data_type = "mnist"
    d_NN = {"sizes": [784, 30, 10],
            "actfuncname": "sigmoid",
            "costfuncname": "cross",
            "batch_size": 10,
            "learning_rate": 0.5,
            "epochs": 5,
            "lambda":5,
            "verbose": True}

    plot_classification(d_NN, data_type, random_state)
