from svmtoy import SVM_toy
from sklearn.datasets import make_moons, make_circles
from sklearn.cross_validation import train_test_split
import numpy as np
import matplotlib.pylab as plt
import os
import itertools


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


def plot_classification(d_SVM, data_type, random_state):

    # Generate data
    X, y = generate_biclass_data(data_type, random_state)
    # Train / Test split
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.1, random_state=random_state)

    # Create SVM model and fit
    SVM = SVM_toy.SVM(
        kernel=d_SVM["kernel"],
        C=d_SVM["C"],
        sigma=d_SVM["sigma"],
        verbose=d_SVM["verbose"])
    SVM.fit_biclass(X_train, y_train)

    # Retrieve w (linear case), interept and support vectors
    w, b, Xsv = SVM.w, SVM.b, SVM.Xsv

    # Make predictions
    y_pred = SVM.predict(X_test)
    # Check how many predictions are correct
    correct = np.sum(y_pred == y_test)
    print "%d out of %d predictions correct" % (correct, len(y_pred))

    # Prepare mesh for plot
    mesh_step = 0.1
    x_min, x_max = X[:, 0].min() - .5, X[:, 0].max() + .5
    y_min, y_max = X[:, 1].min() - .5, X[:, 1].max() + 0.7
    xx, yy = np.meshgrid(np.arange(x_min, x_max, mesh_step),
                         np.arange(y_min, y_max, mesh_step))

    # Split between train/test and class 1/2
    l1_train, l2_train = np.where(y_train < 0), np.where(y_train > 0)
    l1_test, l2_test = np.where(y_test < 0), np.where(y_test > 0)

    # Color definition
    cm = plt.cm.RdBu
    b_bright, r_bright = "#0000FF", "#FF0000"

    # Case 1 : linear kernel, plot separating plane
    if d_SVM["kernel"] == "linear":
        # Optimal margin
        y_bound = (-1. / w[1]) * (b + w[0] * xx[0])
        y_bound_inf = (-1. / w[1]) * (b + 1 + w[0] * xx[0])
        y_bound_sup = (-1. / w[1]) * (b - 1 + w[0] * xx[0])

        # Plot margin boundaries
        plt.plot(xx[0], y_bound, "k")
        plt.plot(xx[0], y_bound_inf, "k--", alpha=0.5)
        plt.plot(xx[0], y_bound_sup, "k--", alpha=0.5)

    # Case 2 : non linear kernel, plot contour
    else:
        Xproj = np.array([[x1, x2]
                         for x1, x2 in zip(np.ravel(xx), np.ravel(yy))])
        z = SVM.project(Xproj).reshape(xx.shape)
        plt.contourf(xx, yy, z, cmap=cm, alpha=.8)

    # Plot support vectors
    plt.scatter(Xsv[:, 0], Xsv[:, 1], s=300, c="g",
                marker=(5, 1), label = "Support vector")
    # Plot the training and testing points
    plt.scatter(X_train[l1_train][:, 0], X_train[l1_train][:, 1],
                c=r_bright, s=60, label="Train Class 1")
    plt.scatter(X_test[l1_test][:, 0], X_test[l1_test][:, 1],
                c=r_bright, s=60, label="Test Class 1", alpha=0.5)
    plt.scatter(X_train[l2_train][:, 0], X_train[l2_train][:, 1],
                c=b_bright, s=60, label="Train Class 2")
    plt.scatter(X_test[l2_test][:, 0], X_test[l2_test][:, 1],
                c=b_bright, s=60, label="Test Class 2", alpha=0.5)

    # bbox top
    if d_SVM["kernel"] == "linear":
        bboxtop = 0.92
    else :
        bboxtop = 1.1

    # Prettify plot
    plt.xlim(xx.min(), xx.max())
    plt.ylim(yy.min(), yy.max())
    plt.xlabel("Variable 1")
    plt.ylabel("Variable 2")
    plt.legend(
        loc='best',
        bbox_to_anchor=(0.8,bboxtop),
        fancybox=True,
        shadow=True,
        fontsize=12,
        scatterpoints=1,
        numpoints=1,
        ncol = 2)
    plt.savefig(
        "./Figures/%s_kernel_%s_data_%s_regularisation_%s_sigma.png" %
        (d_SVM["kernel"], data_type, d_SVM["C"], d_SVM["sigma"]))

    plt.clf()
    plt.close()

if __name__ == "__main__":

    # Create repository and data files if needed
    if not os.path.exists("./Figures/"):
        os.makedirs("./Figures/")

    # Random seed for reproducible results
    random_state = 20

    data_types = ["moons", "lin_sep"]
    list_kernel = ["linear", "rbf"]
    list_C = [0.1]
    list_sigma = [0.2]
    d_k_C_s = list(
        itertools.product(
            data_types,
            list_kernel,
            list_C,
            list_sigma))

    # Loop over all combinations
    for d, k, C, s in d_k_C_s:
        # Dict of SVM parameters
        d_SVM = {"kernel": k, "C": C, "sigma": s, "verbose": True}
        plot_classification(d_SVM, d, random_state)
