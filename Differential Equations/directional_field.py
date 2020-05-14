"""
Developer: Biren Patel
Description: Plot a directional field for a first order partial differential
equation.
"""

import numpy as np
import matplotlib.pyplot as plt

class DirectionalField():
    def __init__(self, ODE, x_params, y_params, samples):
        """
        Parameters
        ----------
        ODE : function
            A first-order ordinary differential equation that must support
            vectorized operations on both 1D and 2D numpy arrays. Function must
            accept two arguments x and y and return a scalar derivative (when
            input are 1D) or a matrix of derivatives (when inputs are 2D).

        x_params : tuple
            tuple of (min, max) defining plot boundaries for independent axis.

        y_params : tuple
            tuple of (min, max) defining plot boundaries for dependent axis.

        samples : int
            within the rectangular section of space defined by the x and y
            parameters, a lattice of equally spaced points equal to samples^2
            will be used to sample the derivative.

        Examples
        --------
        1.
        >>> def diff_eq(x,y):
        >>>     return y * np.cos(x)
        >>>
        >>> model = DirectionalField(diff_eq, (-5,5), (-3,3), 20)
        >>> model.create_field()
        >>> model.plot()

        2.
        >>> def diff_eq(x,y):
        >>>     return np.sin(x+y)
        >>>
        >>> model = DirectionalField(diff_eq, (0,3), (0,3), 20)
        >>> model.create_field(vector_type = "stream")
        >>> model.plot()
        """
        self.ODE = ODE
        self.samples = samples

        #sample evenly spaced coordinates in the requested area of space
        self.x_coords, self.x_dist = np.linspace(*x_params, samples, retstep=True)
        self.y_coords, self.y_dist = np.linspace(*y_params, samples, retstep=True)
        self.X_mesh, self.Y_mesh = np.meshgrid(self.x_coords, self.y_coords)

        #calculate the derivative at each sample coordinate
        self.derivatives = self.ODE(self.X_mesh, self.Y_mesh)

        #attributes reserved for 2D numpy arrays to contain field info for plot
        self.x1 = None
        self.x2 = None
        self.y1 = None
        self.y2 = None

        #attribute to specify type of field, populated on create_field method
        self.vector_type = None

    def create_field(self, vector_type = "line"):
        """
        Control method exposed to user to create, but not plot, the directional
        field.

        Parameters
        ----------
        vector_type : string, optional
            Parameter to control what type of directional field is created. Must
            be one of "line" or "stream". The default is "line".

        Returns
        -------
        None
        """
        self.vector_type = vector_type

        if self.vector_type == "line":
            self._create_line_field()
        elif self.vector_type == "stream":
            self._create_stream_field()
        else:
            raise ValueError("vector_type must be one of 'line' or 'stream'")

    def _create_line_field(self):
        """
        Create a directional field composed of line segments. This algorithm
        is more complicated than _create_stream_field because we must manually
        prevent line segments from overlapping. Imagine an invisible ellipse
        that surrounds each sampled coordinate pair, and use the parametric
        equation of this ellipse to determine where each line segment should
        start and end.

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

        Returns
        -------
        None
        """
        #width and height of each ellipse, scaled by a factor between 0 and .5
        width = 0.25 * self.x_dist
        height = 0.25 * self.y_dist

        #angle of line with x-axis, but use the supplement if angle is negative
        atan_angle = np.arctan(self.derivatives)
        angle = np.where(atan_angle < 0, atan_angle + np.pi, atan_angle)

        #use the parametric equation of an ellipse to get each coordinate pair
        self.x1 = self.X_mesh + (width * np.cos(angle))
        self.x2 = self.X_mesh - (width * np.cos(angle))
        self.y1 = self.Y_mesh + (height * np.sin(angle))
        self.y2 = self.Y_mesh - (height * np.sin(angle))

    def _create_stream_field(self):
        """
        Create a directional field composed of streams. 3 of the 4 required
        objects are already created on initialization, we only need an extra
        object x2 to define the x-velocity. We copy references over for the
        other three objects x1, y1, y2 for notational consistency with the
        two _create_line_field and plot methods.

        Returns
        -------
        None
        """
        self.x1 = self.x_coords
        self.x2 = np.ones(self.derivatives.shape)
        self.y1 = self.y_coords
        self.y2 = self.derivatives

    def plot(self, figure_size=(6,6), title_font=15, axis_font=10):
        """
        Create a matplotlib plot using the field attributes. Must not be called
        before create_field.

        Parameters
        ----------
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
        """
        #set up plot details
        plt.figure(figsize = figure_size)
        plt.title("Directional Field", fontsize = title_font)
        plt.xlabel("x", fontsize = axis_font)
        plt.ylabel("f(x) = y", fontsize = axis_font)

        #cycle through sampled coordinates and plot respective line segments.
        #just provide endpoints, let matplotlib extrapolate to the line.
        if self.vector_type == "line":
            for i in range(self.samples):
                for j in range(self.samples):
                    plt.plot([self.x1[i][j], self.x2[i][j]], \
                             [self.y1[i][j], self.y2[i][j]], \
                             color="black")

        #stream does not need a nested loop, just pass all four arrays
        elif self.vector_type == "stream":
            plt.streamplot(self.x1, self.y1, self.x2, self.y2, \
                           color=self.y2, cmap="RdBu")

        plt.show()
