import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
import numpy as np


class SteadySolution:
    """Solution of initial value problem

    Solution contains stream distance and thalweg and water surface
    elevations.
    """

    def __init__(self, stream_distance, thalweg, wse):

        self._stream_distance = stream_distance.copy()
        self._thalweg = thalweg.copy()
        self._wse = wse.copy()

    def plot(self, ax=None):
        """Plots the thalweg and water surface elevations for
        this solution

        Parameters
        ----------
        ax : matplotlib.axes.Axes, optional
            Axes to plot solution on (the default is None, which
            creates a new axes)

        Returns
        -------
        maplotlib.axes.Axes

        """

        if ax is None:
            ax = plt.axes()

        ax.plot(
            self._stream_distance,
            self._thalweg,
            'g',
            linewidth=5,
            label='Thalweg')
        ax.plot(
            self._stream_distance,
            self._wse,
            'b',
            linewidth=2.5,
            label='Water surface elevation')

        poly_y = np.append(self._thalweg, self._wse[::-1])
        poly_x = np.append(self._stream_distance, self._stream_distance[::-1])
        poly_xy = [*zip(poly_x, poly_y)]
        poly = Polygon(poly_xy, facecolor='b', alpha=0.25)
        ax.add_patch(poly)

        ax.set_xlabel("Distance downstream, in meters")
        ax.set_ylabel("Elevation, in meters")

        ax.legend()

        return ax

    def stream_distance(self):
        """Returns stream distance (distance downstream) of solution

        Returns
        -------
        numpy.ndarray

        """

        return self._stream_distance.copy()

    def thalweg(self):
        """Returns thalweg elevation

        Returns
        -------
        numpy.ndarray

        """

        return self._thalweg.copy()

    def wse(self):
        """Returns water surface elevation

        Returns
        -------
        numpy.ndarray

        """

        return self._wse.copy()
