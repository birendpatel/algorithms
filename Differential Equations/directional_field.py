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
    Plot a directional field given a differential equation. 

    Example
    -------
    >>> def differential_equation(t, x):
    >>>     return x*cos(t)
    >>>
    >>> DF = DirectionalField(differential_equation)
    >>> DF.create_field((0, 10, 20), (-3, 3, 20))
    >>> DF.plot()
    """

    def __init__(self, differential_equation):
        """
        Parameters
        ----------
        differential_equation : function
            A suitable differential equation with input floats t and f(t) = x
            and an output float representing the derivative. Function must
            accept arguments in the order of t, x. 

        Returns
        -------
        None
        """

        self.differential_equation = differential_equation
        self.domain_image_pairs = []
        
    def calculate_derivative(self, t, x):
        """
        A simple wrapper over the provided differential equation to improve
        readability of create_field() method and to expose additionality
        functionality to end user if needed.

        Parameters
        ----------
        t : float
        x : float
        
        Returns
        -------
        float
            The derivative at (t,x) coordinate pair.

        """
        return self.differential_equation(t, x)
        
    def create_field(self, t_parameters, x_parameters, samples):
        """
        Calculate derivative at each possible coordinate pair provided via
        parameters and define the function of a line passing through each
        coordinate pair with the given slop. Truncate starting and ending
        coordinates of each line for visualization purposes.
        
        Parameters
        ----------
        t_parameters : tuple
            tuple of (min, max) defining plot boundaries of independent axis.
        x_parameters : tuple
            tuple of (min, max) defining plot boundaries of dependent axis.
        samples : int
            within the rectangular section of space defined by the t and x
            parameters, a lattice of equally spaced points equal to samples^2
            will be used to sample the derivative.

        Returns
        -------
        None
        """
        #reset domain-image pairs if method is called more than once
        self.domain_image_pairs = []

        #the derivative will be calculated for all pairs (t_coords, x_coords)
        t_coordinates, t_dist = np.linspace(*t_parameters, samples, retstep=True)
        x_coordinates, x_dist = np.linspace(*x_parameters, samples, retstep=True)
        
        #an invisible bounding box will be centered over each sampled pair.
        #We define here the distance of box edges from the center of the box.
        #these limits prevents the drawns lines from intersecting.
        t_box_limits = 0.25  * t_dist
        x_box_limits = 0.25  * x_dist

        # loop through each (t,x) and draw a centered line with slope = ddt
        for i in t_coordinates:
            for j in x_coordinates:
                #define the equation of a line using the derivative
                slope = self.calculate_derivative(i, j)
                domain = np.linspace(i-t_box_limits, i+t_box_limits, num=200)
                image = slope * (domain - i) + j

                #remove parts of the line that exceed the bounding square box
                lower_bound = image < j - x_box_limits
                upper_bound = image > j + x_box_limits
                del_inds = np.argwhere((lower_bound)|(upper_bound)).flatten()
                image = np.delete(image, del_inds)
                domain = np.delete(domain, del_inds)
                
                self.domain_image_pairs.append((domain, image))

    def plot(self, figure_size=(15,15), title_font=25, axis_font=20):
        """
        Plot a line at each sampled coordinate pair.
        
        Parameters
        ----------
        figure_size : tuple, optional
            matplotlib figure method parameter for figsize. The default is 
            (15,15).
        title_font : int, optional
            matplotlib title method parameter for fontsize. The default is 25.
        axis_font : int, optional
            matplotlib xlabel and ylabel method parameter for fontsize. The 
            default is 20.

        Returns
        -------
        None
        """
        
        plt.figure(figsize = figure_size)
        plt.title("Directional Field", fontsize = title_font)
        plt.xlabel("t", fontsize = axis_font)
        plt.ylabel("f(t) = x", fontsize = axis_font)

        #loop through collection of lines and plot each one
        for i in self.domain_image_pairs:
            plt.plot(*i, color="black")