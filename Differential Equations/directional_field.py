"""
Developer: Biren Patel
Creation Date: 2020 May 9
Last Modified: 2020 May 9
Description: Plot a directional field.
"""

import numpy as np
import matplotlib.pyplot as plt

class DirectionalField():
    """
    Plot a directional field given a suitable differential equation.

    Example
    -------
    >>> def differential_equation(t, x):
    >>>     return x*cos(t)
    >>>
    >>> canvas = DirectionalField(differential_equation)
    >>> canvas.create_field((0, 10, 20), (-3, 3, 20))
    """

    def __init__(self, differential_equation):
        """
        Parameters
        ----------
        differential_equation : function
            A suitable differential equation with one input variable t and an
            output variable f(t) = x.

        Returns
        -------
        None
        """

        self.differential_equation = differential_equation

    def create_field(self, t_params, x_params):
        """
        Parameters
        ----------
        t_params : tuple
            - tuple of (min, max, sample points).
            - min and max define boundaries of x-axis.
            - sample points defines number of evenly spaced lines to plot between
              min and max.
        x_params : tuple
            - tuple of (min, max, sample points).
            - min and max define boundaries of x-axis.
            - sample points defines number of evenly spaced lines to plot between
              min and max.

        Returns
        -------
        None
        """

        #set up coordinates
        t_coords = np.linspace(t_params[0], t_params[1], num=t_params[2])
        x_coords = np.linspace(x_params[0], x_params[1], num=x_params[2])

        #set up bounding box for the length of each directional line
        t_box_limits = 0.25  * abs(t_coords[0] - t_coords[1])
        x_box_limits = 0.25  * abs(x_coords[0] - x_coords[1])

        #plot each directional line
        self._plot_field(t_coords, x_coords, t_box_limits, x_box_limits)

    def _plot_field(self, t, x, t_box_limits, x_box_limits):
        """
        Parameters
        ----------
        t : ndarray
            A sloped line will be plotted at each pair (t,x). t array provides
            all possible values of t per pair.
        x : ndarray
            A sloped line will be plotted at each pair (t,x). x array provides
            all possible values of x per pair.
        t_box_limits : float
            For each plotted line, t_box_limits is used to prevent the line
            from drawing past the vertical boundaries of a bounding box.
        x_box_limits : float
            For each plotted line, x_box_limits is used to prevent the line
            from drawing past the horizontal boundaries of a bounding box.

        Returns
        -------
        None
        """

        #visualization set up
        plt.figure(figsize=(15, 15))
        plt.title('Directional Field', fontsize=25)
        plt.xlabel('t', fontsize=20)
        plt.ylabel('f(t) = x', fontsize=20)

        # loop through each (t,x) and draw a centered line with slope = ddt
        for i in t:
            for j in x:
                #create a line
                slope = self.differential_equation(i, j)
                domain = np.linspace(i-t_box_limits, i+t_box_limits, num=200)
                image = slope * (domain - i) + j

                #remove parts of the line that exceed the bounding square box
                lower_bound = image < j-x_box_limits
                upper_bound = image > j+x_box_limits

                del_inds = np.argwhere((lower_bound)|(upper_bound)).flatten()
                image = np.delete(image, del_inds)
                domain = np.delete(domain, del_inds)

                plt.plot(domain, image, color='black')