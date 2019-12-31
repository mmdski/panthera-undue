from abc import ABC, abstractmethod

import numpy as np


class StageRelation(ABC):
    """Abstract base class for stage relations"""

    @abstractmethod
    def stage(self, *args):
        """Returns stage value defined by relation

        Returns
        -------
        float
            Stage

        """
        pass


class DischargeRelation(ABC):
    """Abstract base class for discharge relations"""

    @abstractmethod
    def discharge(self, *args):
        pass


class StageDischargeRelation(StageRelation, DischargeRelation):
    pass


class FixedStageRelation(StageRelation):
    """Fixed stage relation

    Returns a fixed stage value

    Parameters
    ----------
    y : float
        Stage

    """

    def __init__(self, y):

        self._y = y

    def stage(self, discharge):
        """Returns fixed stage value

        Parameters
        ----------
        discharge : float

        Returns
        -------
        float
            Stage

        """

        if np.isscalar(discharge):
            return self._y
        else:
            return self._y * np.ones_like(discharge)


class CrossSectionRelation(StageDischargeRelation):

    def __init__(self, xs, datum=0):

        self._xs = xs
        self._datum = datum
        self._last_depth = None

    def _get_y0(self, y0):

        if y0 is None:
            if self._last_depth is None:
                y, _ = self._xs.coordinates()
                y0 = 0.8 * (y.max() - y.min()) + self._datum
            else:
                y0 = self._last_depth

        return y0

    def discharge(self, stage):

        raise NotImplementedError

    def stage(self, discharge):

        raise NotImplementedError


class CriticalRelation(CrossSectionRelation):

    def discharge(self, stage):

        depth = stage - self._datum

        return self._xs.critical_flow(depth)

    def stage(self, discharge, y0=None):

        y0 = self._get_y0(y0)
        h0 = y0 - self._datum
        depth = self._xs.critical_depth(discharge, h0)
        y = depth + self._datum
        if not np.isnan(depth):
            self._last_depth = y

        return y


class NormalRelation(CrossSectionRelation):
    """Normal stage-discharge relation

    Parameters
    ----------
    xs : CrossSection
        Cross section to base relation on
    slope : float
        Bed slope
    datum : float, optional
        Stage datum, optional (the default is 0)

    """

    def __init__(self, cross_section, slope, datum=0):

        super().__init__(cross_section, datum)
        self._slope = slope

    def discharge(self, stage):
        """Computes discharge from relation

        Parameters
        ----------
        stage : float
            Stage to compute discharge

        Returns
        -------
        float
            Discharge

        """

        depth = stage - self._datum
        return self._xs.normal_flow(depth, self._slope)

    def stage(self, discharge, y0=None):
        """Computes stage from relation

        Parameters
        ----------
        discharge : float
            Discharge to compute stage
        y0 : float, optional
            Initial estimate of normal stage, optional (the default is None)

        Returns
        -------
        float
            Stage

        """

        y0 = self._get_y0(y0)
        h0 = y0 - self._datum
        depth = self._xs.normal_depth(discharge, self._slope, h0)
        y = depth + self._datum
        if not np.isnan(depth):
            self._last_depth = y

        return y
