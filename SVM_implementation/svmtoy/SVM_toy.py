import numpy as np
import cvxopt.solvers


class Kernel(object):

    """ Kernel class"""

    @staticmethod
    def linear_kernel():
        return lambda x, y: np.inner(x, y)

    @staticmethod
    def rbf(sigma):
        return (
            lambda x, y: np.exp(
                -np.sqrt(np.linalg.norm(x - y) ** 2 / (2 * sigma ** 2)))
        )


class SVM(object):

    def __init__(self, kernel="linear", C=None, sigma = 1., verbose=False):
        self._verbose = verbose
        self._C = C
        self._kernelname = kernel
        if self._kernelname == "linear":
            self._kernel = Kernel.linear_kernel()
        elif self._kernelname == "rbf":
            self._kernel = Kernel.rbf(sigma)

    def _format_target(self, y):
        # 1st : check this is a 2-class problem
        targets = set(y.tolist())
        try:
            assert (len(targets) == 2)
        except AssertionError:
            print "Number of class must be exactly 2"
        # 2nd : format target to: -1 / +1
        t1 = min(targets)
        t2 = max(targets)
        l1 = np.where(y < t2)
        l2 = np.where(y > t1)
        y[l1] = -1.
        y[l2] = 1.

        return y.astype(float)

    def _build_kernel_matrix(self, X):
        return self._kernel(X, X)

    def _build_solver_matrices(self, X, y):

        # Define n_samples, n_param
        n_samples, n_param = X.shape

        # Kernel matrix
        K = self._build_kernel_matrix(X)
        Pmat = np.dot(np.diag(y), np.dot(K, np.diag(y)))

        P = cvxopt.matrix(Pmat)
        q = cvxopt.matrix(-1 * np.ones(n_samples))
        A = cvxopt.matrix(np.reshape(y, (1, y.shape[0])))
        b = cvxopt.matrix(0.0)

        if self._C is not None:
            # Increase dim of G (from m,m to 2m,m to account for the double
            # inequality)
            G1 = -1 * np.diag(np.ones(n_samples))
            G2 = np.diag(np.ones(n_samples))
            G = cvxopt.matrix(np.vstack((G1, G2)))
            h1 = np.zeros(n_samples)
            h2 = self._C * np.ones(n_samples)
            h = cvxopt.matrix(np.hstack((h1, h2)))
        else:
            G = cvxopt.matrix(-1 * np.diag(np.ones(n_samples)))
            h = cvxopt.matrix(np.zeros(n_samples))

        return P, q, G, h, A, b, K

    def _get_intercept(self, X, y, a):

        # Identify support vectors (a > 0)
        l_SV = np.where(self.a > 1E-5)
        # Define the X, a, y matrices of support vectors
        self.Xsv = X[l_SV]
        self.asv = a[l_SV]
        self.ysv = y[l_SV]

        # Initialise w, the normal vector to the separating hyperplane
        self.w = None

        # Store number of support vectors
        self.n_support = len(l_SV[0])

        # Case 1 : rigid margin
        if self._C is None:
            # Kernel matrix of Support vectors with 0 < a
            Ksv = self._build_kernel_matrix(self.Xsv)

            # Compute intercept b
            b_vec = y[l_SV] - np.dot(Ksv, self.a[l_SV] * y[l_SV])
            # Average for better stability
            b = np.average(b_vec)

            # Define w only in the linear case
            if self._kernelname == "linear":
                self.w = np.dot(self.a[l_SV] * y[l_SV], self.Xsv)

        # Case 2 : margin /w slack variables
        else:
            # Define list of index for which 0 < a < b
            condition = np.logical_and(self.a > 1E-5, self.a < self._C)
            l_b = np.where(condition)

            # Kernel matrix of Support Vectors with 0 < a < C
            Kb = self._build_kernel_matrix(X[l_b])

            #Comput intercept b
            b_vec = y[l_b] - np.dot(Kb, self.a[l_SV] * y[l_SV])
            # Average only on l_b for better stability
            b = np.average(b_vec)

            # Define w only in the linear case
            if self._kernelname == "linear":
                self.w = np.dot(self.a[l_SV] * y[l_SV], self.Xsv)

        return b

    def fit_biclass(self, X, y):
        """Fit an optimal margin classifier to the data

        minimize 1/2 w.T * P * w
        subject to y

        """

        # Format y
        y = self._format_target(y)

        # Get the matrices / vectors to solve the QP problem
        P, q, G, h, A, b, K = self._build_solver_matrices(X, y)

        # Solve QP problem
        solution = cvxopt.solvers.qp(P, q, G, h, A, b)

        # Get Lagrange multipliers
        self.a = np.ravel(np.array(solution["x"]))

        # Compute intercept
        self.b = self._get_intercept(X, y, self.a)

        # Indicate model has been trained
        self.trained = True

        if self._verbose:
            print "Found %s SV" % (self.n_support)
            print "w  = %s " % (self.w)
            print "b  = %s " % (self.b)

    def project(self, X):
        """ Project data X on the separating hyperplane of the SVM"""

        n_samples = X.shape[0]

        # First check the SVM has been trained
        try:
            assert(self.trained)
        except AssertionError:
            print "SVM not trained. Use a fit method."

        if self.w is not None:
            # Linear kernel case
            return np.dot(X, self.w) + self.b
        else:
            # Non linear kernels
            y_pred = np.zeros(n_samples)
            for i in range(n_samples):
                score = 0
                for a, y, sv in zip(self.asv, self.ysv, self.Xsv):
                    score += a * y * self._kernel(X[i], sv)
                y_pred[i] = score
            return y_pred + self.b

    def predict(self, X):
        """ Apply trained SVM to X"""

        pred = np.sign(self.project(X))
        return pred
