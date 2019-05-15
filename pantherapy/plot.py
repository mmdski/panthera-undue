import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon


class CrossSectionPlot:
    """Pantherapy cross section plots

    Parameters
    ----------
    cross_section : pantherapy.CrossSection
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
        ax : matplotlib.axes.Axes, optional
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
