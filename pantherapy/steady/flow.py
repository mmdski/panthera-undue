import numpy as np


class SteadyFlow:
    """Flow data for initial value problem"""

    def __init__(self):

        self._flow_table = {}
        self._x = None
        self._flow = None

    def flow(self, x):
        """Returns the flow value at a downstream distance

        Parameters
        ----------
        x : float
            Distance downstream

        Returns
        -------
        float
            Flow

        """

        if len(self._flow_table) == 0:
            raise ValueError("Empty flow table")

        # build x and flow arrays
        if self._x is None or self._flow is None:
            self._x = np.array(list(self._flow_table.keys()))
            self._flow = np.array(list(self._flow_table.values()))

            sort_idx = np.argsort(self._x)
            self._x = self._x[sort_idx]
            self._flow = self._flow[sort_idx]

        flow_idx = np.digitize(x, self._x) - 1
        if np.any(flow_idx < 0):
            raise ValueError("Distance value outside of domain")

        return self._flow[flow_idx]

    def set_flow(self, x, flow):
        """Sets the flow value to a downstream distance

        Parameters
        ----------
        x : float
            Distance downstream
        flow : float
            Flow value

        """

        self._flow_table[x] = flow
        self._x = None
        self._flow = None
