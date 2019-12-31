import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
import numpy as np
from numpy.linalg import solve

from pantherapy.steady.solution import SteadySolution


class BoundaryValuePlan:
    """Boundary value problem solution plan

    Parameters
    ----------
    reach : Reach
        Reach for solution
    flow_data : SteadyFlow
        Flow data for solution
    us_boundary : float
        Upstream water surface elevation
    ds_boundary : float
        Downstream water surface elevation

    """

    def __init__(self, reach):

        self._reach = reach

    def solve(self, wse_0, q_0):

        if len(wse_0) != len(self._reach):
            raise ValueError(
                "Initial wse estimate must be the same length as reach")

        # number of unknown water surface elevations
        n = len(self._reach) - 2

        # vector of unknowns
        x = np.ones((n + 1, ))
        x[:-1] = wse_0[1:-1]
        x[-1] = q_0

        b = np.zeros_like(x)
        b[0] = wse_0[0]
        b[-2] = wse_0[-1]

        h_tol = 0.001
        q_tol = 0.01
        tol = np.zeros_like(x)
        tol[:-1] = h_tol
        tol[-1] = q_tol

        max_iter = 20

        solver = BoundaryValueSolver(self._reach)

        for i in range(max_iter):
            dx = solver.solve_iteration(x, b)
            x = x + dx
            if np.any(np.isnan(x)):
                break
            if np.all(np.abs(dx) < tol):
                break

        wse = np.zeros_like(wse_0)
        wse[0] = wse_0[0]
        wse[-1] = wse_0[-1]
        wse[1:-1] = x[:-1]
        discharge = x[-1]
        stream_distance = self._reach.stream_distance()
        thalweg = self._reach.thalweg()

        return BoundaryValueSolution(
            stream_distance, thalweg, wse, discharge, i)


class BoundaryValueSolution(SteadySolution):

    def __init__(self, stream_distance, thalweg, wse, discharge, n_iterations):

        super().__init__(stream_distance, thalweg, wse)

        self._q = discharge
        self._n_iter = n_iterations

    def discharge(self):

        return self._q

    def iterations(self):

        return self._n_iter


class BoundaryValueSolver:

    def __init__(self, reach, r_dx=0.01):

        self._reach = reach
        self._n_unknown_h = len(self._reach) - 2
        self._stream_distance = reach.stream_distance()
        self._r_dx = r_dx

    def _boundary_coeff(self, i, x, b):
        q = x[-1]

        if i == 0:
            us_reach_index = 0
            ds_reach_index = 1
            h_us = b[0]
            h_ds = x[0]
            factor = 1
        elif i == self._n_unknown_h - 1:
            us_reach_index = -2
            ds_reach_index = -1
            h_us = x[-2]
            h_ds = b[-2]
            factor = -1

        dx = self._stream_distance[ds_reach_index] - \
            self._stream_distance[us_reach_index]

        hv_us = self._reach.velocity_head(us_reach_index, h_us, q)
        hv_ds = self._reach.velocity_head(ds_reach_index, h_ds, q)

        hf_us = self._reach.friction_slope(us_reach_index, h_us, q)
        hf_ds = self._reach.friction_slope(ds_reach_index, h_ds, q)

        coeff = factor * (hv_ds - hv_us + dx / 2 * (hf_us + hf_ds)) / q

        return coeff

    def _discharge_func(self, i, x):

        q = x[-1]

        hv0 = self._reach.velocity_head(i, x[i - 1], q)
        hv1 = self._reach.velocity_head(i + 1, x[i], q)

        sf0 = self._reach.friction_slope(i, x[i - 1], q)
        sf1 = self._reach.friction_slope(i + 1, x[i], q)

        x0 = self._stream_distance[i]
        x1 = self._stream_distance[i + 1]

        dx = x1 - x0

        return (-hv0 + hv1 + dx / 2 * (sf0 + sf1)) / q

    def _a_scalar(self, i, j, x, b):
        """Coefficient of A matrix

        i and j are 0-indexed

        """

        # number of unknown water surface elevations
        n = len(self._reach) - 2

        if j == i:
            if i == n:
                return self._discharge_func(i - 1, x)
            else:
                return 1

        elif j == i - 1:
            if i == n - 1:
                return 0
            elif i == n:
                return 1
            else:
                return -1

        elif j == i - 2:
            if i == n:
                return -1
            else:
                return 0

        elif j == n:
            if i == 0:
                return self._boundary_coeff(i, x, b)
            elif i == n - 1:
                return self._boundary_coeff(i, x, b)
            else:
                return self._discharge_func(i, x)

        else:
            return 0

    def _a_row(self, x, b, i):

        n = len(x) - 1
        a_row = np.zeros((n + 1, ))
        for j in range(n + 1):
            a_row[j] = self._a_scalar(i, j, x, b)

        return a_row

    def _a_matrix(self, x, b):

        n = len(x) - 1
        a = np.zeros((n + 1, n + 1))
        for i in range(n + 1):
            a[i, :] = self._a_row(x, b, i)

        return a

    def _f_partial(self, i, j, x, b):

        x_hi = x.copy()
        x_hi[j] = (1 + self._r_dx) * x_hi[j]
        a_hi = self._a_row(x_hi, b, i)
        f_hi = np.dot(a_hi, x_hi) - b[j]

        x_lo = x.copy()
        x_lo[j] = (1 - self._r_dx) * x_lo[j]
        a_lo = self._a_row(x_lo, b, i)
        f_lo = np.dot(a_lo, x_lo) - b[j]

        dx = x_hi[j] - x_lo[j]

        return (f_hi - f_lo) / dx

    def _jacobian(self, x, b):

        n = len(x) - 1
        J = np.zeros((n + 1, n + 1))
        for i in range(n + 1):
            for j in range(n + 1):
                J[i, j] = self._f_partial(i, j, x, b)

        return J

    def solve_iteration(self, x, b):
        """Solve an interation of the boundary value solution

        """

        A = self._a_matrix(x, b)
        f = np.dot(A, x) - b
        J = self._jacobian(x, b)

        return -solve(J, f)
