"""Pantherapy plots"""

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon


class CrossSectionPlot:
    """Cross section plots

    Parameters
    ----------
    cross_section : :class:`pantherapy.panthera.CrossSection`
        Cross section to plot

    """

    def __init__(self, cross_section):

        self._xs = cross_section

    def plot(self, h=None, ax=None):
        """Plot cross section geometry

        Parameters
        ----------
        h : float, optional
            Water surface elevation relative to cross section coordinate
            elevations, optional (default is None). If not None, wetted
            properties will be plotted if h is greater than the lowest
            y-coordinate in the cross section.
        ax : :class:`matplotlib.axes.Axes`, optional
            Plot axes, optional (the default is None, which creates a
            new Axes instance).

        Returns
        -------
        matplotlib.axes.Axes
            Plot axes

        """

        if ax is None:

            ax = plt.axes()

        y, z = self._xs.coordinates()
        ax.plot(z, y, 'k', marker='.', label='Coordinates')

        if h is not None and h > y.min():

            y_wp, z_wp = self._xs.wp_array(h)
            y_tw, z_tw = self._xs.tw_array(h)

            xs_area_zy = [*zip(z_wp, y_wp)]

            if h > y_wp[0]:
                xs_area_zy.insert(0, (z_wp[0], h))
            if h > y_wp[-1]:
                xs_area_zy.append((z_wp[-1], h))

            ax.plot(z_tw, y_tw, 'b', linewidth=2.5, label='Top width')
            ax.plot(z_wp, y_wp, 'g', linewidth=5, label='Wetted perimeter')

            if len(xs_area_zy) > 2:
                poly = Polygon(xs_area_zy, facecolor='b',
                               alpha=0.25, label='Wetted area')
                ax.add_patch(poly)

        ax.set_xlabel('z, in meters')
        ax.set_ylabel('y, in meters')

        ax.legend()

        return ax


class SStepResultsPlot:
    """Standard step solution results plots

    Parameters
    ----------
    results : :class:`pantherapy.panthera.StandardStepResults`
        Results to plot

    """

    def __init__(self, results):

        self._res = results

    def plot(self, ax=None):
        """Plot standard step solution results

        Parameters
        ----------
        ax : :class:`matplotlib.axes.Axes`, optional
            Plot axes, optional (the default is None, which creates a
            new Axes instance).

        Returns
        -------
        matplotlib.axes.Axes
            Plot axes

        """

        if ax is None:
            ax = plt.axes()

        x_tw, y_tw = self._res.thalweg()
        x_ws, y_ws = self._res.ws_elevation()

        poly_pts = [*zip(x_tw, y_tw)]
        poly_pts.extend([*zip(np.flip(x_ws), np.flip(y_ws))])
        poly = Polygon(poly_pts, facecolor='b', alpha=0.25)
        ax.add_patch(poly)

        ax.plot(x_ws, y_ws, 'b', linewidth=2.5, label='Water surface')
        ax.plot(x_tw, y_tw, 'g', label='Thalweg')

        ax.set_xlabel('x, in meters')
        ax.set_ylabel('y, in meters')

        ax.legend()

        return ax
