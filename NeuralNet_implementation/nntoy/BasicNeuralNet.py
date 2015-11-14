import numpy as np

class ActivationFunction(object):

    """ Activation function class"""

    @staticmethod
    def sigmoid():
        return lambda x: 1. / (1 + np.exp(-x))

    @staticmethod
    def dsigmoid():
        return lambda x: 1. / (1 + np.exp(-x)) * (1  - 1. / (1 + np.exp(-x)))

    @staticmethod
    def tanh():
        return lambda x: np.tanh(x)

    @staticmethod
    def dtanh():
        return lambda x: 1. - np.power(np.tanh(x), 2)


class CostFunction(object):

    """ Cost function class"""

    @staticmethod
    def mse():
        return lambda y,a: 0.5 * np.power(y-a, 2)

    @staticmethod
    def dmse():
        return lambda y,a: a - y

    @staticmethod
    def crossent():
        return lambda y,a : np.nan_to_num(-y*np.log(a)-(1-y)*np.log(1-a))

    @staticmethod
    def dcrossent():
        return lambda y,a :(a-y) / (a * (1 - a))

class BasicNeuralNet(object):


    def __init__(
            self,
            sizes,
            lmbda = 0,
            actfuncname="sigmoid",
            costfuncname="mse",
            verbose=False):
        self._nlayers = len(sizes)
        self._sizes = sizes
        self._lmbda = lmbda
        # Random initialisation of biases and weights.
        #For the weights, use gaussian with std = sqrt(# of weights connecting to a neuron)
        # So that by the CLT, their sum is gaussian with std = 1
        # Add [0] for clearer indexing
        self._biases = [np.array([0])] + [np.random.randn(size, 1)
                                          for size in self._sizes[1:]]
        self._weights = [np.array([0])] + [np.random.randn(self._sizes[i], self._sizes[i - 1])/ \
                                           np.sqrt(self._sizes[i-1])
                                           for i in range(1, self._nlayers)]

        # Initialisation of z
        self._z = [np.array([0])] + [np.zeros((size, 1))
                                     for size in self._sizes[1:]]

        # Activation function
        self._actfuncname = actfuncname
        if self._actfuncname == "sigmoid":
            self._actfunc = ActivationFunction.sigmoid()
            self._dactfunc = ActivationFunction.dsigmoid()
        elif self._actfuncname == "tanh":
            self._actfunc = ActivationFunction.tanh()
            self._dactfunc = ActivationFunction.dtanh()

        # Cost function
        self._costfuncname = costfuncname
        if self._costfuncname == "mse":
            self._costfunc = CostFunction.mse()
            self._dcostfunc = CostFunction.dmse()

        # Cost function
        self._costfuncname = costfuncname
        if self._costfuncname == "cross":
            self._costfunc = CostFunction.crossent()
            self._dcostfunc = CostFunction.dcrossent()

    def _forward_pass(self, x):
        # Initialisation of activation matrix
        self._a = [x]
        for layer in range(1, self._nlayers):
            self._z[layer] = np.dot(self._weights[layer], self._a[layer-1]) \
                + self._biases[layer]
            a = self._actfunc(self._z[layer])
            self._a.append(a)

        # For scoring
        return self._a[-1]

    def _backward_pass(self, y):
        # Initialisation of error matrix
        delta_L = self._dcostfunc(y, self._a[-1]) \
            * self._dactfunc(self._z[-1])
        self._delta = [delta_L]
        for layer in range(1, self._nlayers - 1)[::-1]:
            delta_l = np.dot(
                self._weights[layer + 1].T, self._delta[self._nlayers - layer -2]) \
                * self._dactfunc(self._z[layer])

            self._delta = [delta_l] + self._delta
        self._delta = [np.array([0])] + self._delta

    def _update_gradient(self, batch, n_training):

        n = len(batch)

        # Initialise derivative of cost wrt bias and weights
        dCdb = [np.array([0])] + [np.zeros((size,1)) for size in self._sizes[1:]]
        dCdw = [np.array([0])] + [np.zeros((self._sizes[i], self._sizes[i - 1]))
                                  for i in range(1, self._nlayers)]
        # Loop over batch
        for X, y in batch:
            self._forward_pass(X)
            self._backward_pass(y)

            # Loop over layers
            for layer in range(1, self._nlayers):
                dCdb[layer] += self._delta[layer]/float(n)
                dCdw[layer] += np.dot(self._delta[layer], self._a[layer - 1].T)/float(n) + self._lmbda * self._weights[layer]/float(n_training)

        return dCdb, dCdw

    def fit_SGD(self, training_data, learning_rate, batch_size, epochs, test_data = None):

        n_samples = len(training_data)
        # Loop over epochs
        for ep in range(epochs):

            #Shuffle data
            np.random.shuffle(training_data)

            for k in xrange(0, n_samples, batch_size):
                batch = training_data[k:k+batch_size]

                dCdb, dCdw = self._update_gradient(batch, n_samples)
                # Update bias and weights
                self._biases = [self._biases[layer] - learning_rate * dCdb[layer]
                           for layer in range(self._nlayers)]
                self._weights = [self._weights[layer] - learning_rate * dCdw[layer]
                                 for layer in range(self._nlayers)]
                print self._weights[-1]
                raw_input()
            print "Epoch:", self.score(test_data), "/", len(test_data)

    def score(self, test_data):
        """ Score """

        test_results = [(np.argmax(self._forward_pass(np.reshape(x, (len(x), 1)))), y)
                        for (x, y) in test_data]
        return sum(int(x == y) for (x, y) in test_results)