"""
Developer: Biren Patel
Description: Plot a directional field for a first-order ODE. Without a stream
or quiver plot, the algorithm is surprisingly longer than one would initially
think as it requires the parametric representation of an ellipse.
"""

import numpy as np
import matplotlib.pyplot as plt

def DirectionalField(diffeq, x_parameters, y_parameters, samples, \
                    figure_size=(6,6), title_font=15, axis_font=10):
    """
    Plot a directional field given some first-order ordinary differential
    equation.

    Parameters
    ----------
    diffeq : function
        A first-order ordinary differential equation that must support
        vectorized operations on 2D numpy arrays. Inputs X and Y are
        matrices generated from a numpy meshgrid. Output will be a single
        matrix providing the derivative at each sampled coordinate pair.
        See functions used in the examples under class docstring.

    t_parameters : tuple
        tuple of (min, max) defining plot boundaries of independent axis.

    x_parameters : tuple
        tuple of (min, max) defining plot boundaries of dependent axis.

    samples : int
        within the rectangular section of space defined by the x and y
        parameters, a lattice of equally spaced points equal to samples^2
        will be used to sample the derivative.

    figure_size : tuple, optional
        matplotlib figure method parameter for figsize. The default is
        (6,6).

    title_font : int, optional
        matplotlib title method parameter for fontsize. The default is 15.

    axis_font : int, optional
        matplotlib xlabel and ylabel method parameter for fontsize. The
        default is 10.

    Returns
    -------
    None

    Algorithm
    ---------
    1. Calculate derivative at each possible coordinate pair provided via
    the parameters.

    2. For each coordinate pair:
        2a. Center an ellipse over the coordinate pair.

        2b. Given a line passing through the pair with slope equal to the
        derivative, find the two coordinate pairs where the line intersects
        with the ellipse.

        2c. Retain both coordinate pairs as the endpoints of a segment to be
        plotted.

    Examples
    --------
    1.
    >>> def diffeq(x,y):
    >>>    return y * np.cos(x)
    >>>
    >>> DirectionalField(diffeq, (-5, 5), (-3,3), 20)

    2.
    >>> def diffeq(x,y):
    >>>    return x + y
    >>>
    >>> DirectionalField(diffeq, (-10, 10), (-10,10), 20)
    """
    #sample the x and y coordinates for the requested area in space
    x_coordinates, x_dist = np.linspace(*x_parameters, samples, retstep=True)
    y_coordinates, y_dist = np.linspace(*y_parameters, samples, retstep=True)

    #calculate derivative at each sampled coordinate pair
    X, Y = np.meshgrid(x_coordinates, y_coordinates)
    ddx = diffeq(X,Y)

    #width and height of each ellipse, scaled by a factor between 0 and .5
    width = 0.25 * x_dist
    height = 0.25 * y_dist

    #angle of line with x-axis, but use the supplement if angle is negative
    angle = np.where(ddx < 0, np.arctan(ddx) + np.pi, np.arctan(ddx))

    #use the parametric equation of an ellipse to get each coordinate pair
    x_1 = X + (width * np.cos(angle))
    y_1 = Y + (height * np.sin(angle))
    x_2 = X - (width * np.cos(angle))
    y_2 = Y - (height * np.sin(angle))

    #set up plot details
    plt.figure(figsize = figure_size)
    plt.title("Directional Field", fontsize = title_font)
    plt.xlabel("x", fontsize = axis_font)
    plt.ylabel("f(x) = y", fontsize = axis_font)

    #graph each segment of a line onto the plot
    for i in range(samples):
        for j in range(samples):
            plt.plot([x_1[i][j], x_2[i][j]], [y_1[i][j], y_2[i][j]], color="black")

    plt.show()
