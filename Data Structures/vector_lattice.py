"""
Developer: Biren Patel
Description: A lattice data structure isomorphic to tabular data, but designed
to be used for vectorized operations on data that would naturally exists in an
n-dimensional cartesian coordinate system.

status: WORK IN PROGRESS
"""

import numpy as np
from collections import OrderedDict

class FeatureLattice():
    def __init__(self, shape):
        """
        A data structure representing a lattice of points, where each point
        is a feature vector of data. If one imagines a tabular dataset with rows
        as data points and columns as features, then the vector lattice is an
        isomorphic structure where each row is now represented by a point on a
        lattice.

        Parameters
        ----------
        shape : tuple
            Couple of (x,y) which defines the shape of the lattice. x and y are
            both of int data type.
        """
        #attributes specific to the lattice
        self.shape = shape
        self._points = np.prod(self.shape)

        #attributes specific to the lattice points
        self._feature_map = OrderedDict()
        self.features = ()

    def __setitem__(self, name, data):
        """
        Add a named feature to every lattice point and populate with data.

        Parameters
        ----------
        name : string
            The name of the feature, which is later used to access the feature.
        data : numpy.ndarray
            An array of data with the same shape as the lattice.
        """
        if type(data) is not np.ndarray:
            raise TypeError("'data' must be numpy ndarray")

        if data.shape != self.shape:
            raise ValueError("shape of data does not match shape of lattice")

        if type(name) is not str:
            raise TypeError("'name' must be string")

        #add feature layer to the feature map
        self._feature_map[name] = data

        #update features
        self.features = tuple(self._feature_map.keys())

    def __getitem__(self, name):
        """
        Retrieve an entire layer of the requested feature.

        Parameters
        ----------
        name : string
            The name of the feature.
        """
        return self._feature_map[name]

if __name__ == "__main__":

    lattice = FeatureLattice((3,3))

    layer_1 = np.zeros(lattice.shape)
    layer_2 = np.ones(lattice.shape)

    lattice["layer 1"] = layer_1
    lattice["layer 2"] = layer_2
    lattice["layer 3"] = 1 + lattice["layer 2"]
