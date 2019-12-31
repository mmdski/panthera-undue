import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
import numpy as np
from scipy.linalg import solve_banded
from scipy.optimize import newton

from pantherapy.steady.solution import SteadySolution


class InitialValuePlan:
    """Initial value problem solution plan

    Parameters
    ----------
    reach : Reach
        Reach for solution
    flow_data : SteadyFlow
        Flow data for solution
    boundary_location : {'upstream', 'downstream'}
        Boundary condition location
    boundary_condition : StageRelation
        Stage boundary condition

    """

    def __init__(
            self,
            reach,
            flow_data,
            boundary_location,
            boundary_condition):

        self._reach = reach
        self._flow_data = flow_data
        self._bc_loc = boundary_location
        self._bc = boundary_condition

    def _simultaneous(self, eps=0.001, iter=20):

        if self._bc_loc == 'upstream':
            raise NotImplementedError(
                "Upstream boundary condition not implemented for " +
                "simultaneous solution")

        thalweg = self._reach.thalweg()
        bc_index = -1
        bc_x = self._reach.node_location(bc_index)
        bc_flow = self._flow_data.flow(bc_x)
        bc_elevation = self._bc.stage(bc_flow)
        bc_depth = bc_elevation - thalweg[bc_index]
        y = bc_depth + thalweg

        solver = SimultaneousSolver(self._reach, self._flow_data, bc_elevation)

        i = 0
        while i < iter:

            i = i + 1

            dy = solver.solve_iteration(y)
            y = y + dy

            if (np.abs(dy[:-1])).max() < eps:
                break

        if i == iter:
            raise RuntimeError(
                "Number of iterations exceeded without finding solution")

        return SteadySolution(self._reach.stream_distance(), thalweg, y)

    def _sstep(self):

        solver = StandardStepSolver(self._reach, self._flow_data)

        if self._bc_loc == 'downstream':
            first_index = len(self._reach) - 1
            last_index = -1
            direction = -1
        elif self._bc_loc == 'upstream':
            first_index = 0
            last_index = len(self._reach)
            direction = 1
        else:
            raise ValueError(
                "Invalid boundary location: {}".format(self._bc_loc))

        node_range = np.arange(first_index + direction, last_index, direction)

        stream_distance = self._reach.stream_distance()

        wse = np.zeros_like(stream_distance)
        wse[:] = np.nan

        wse[first_index] = self._bc.stage(
            self._flow_data.flow(
                stream_distance[first_index]))

        for i, j in zip(node_range, node_range - direction):
            wse[i] = solver.solve_node(i, j, wse[j])
            if np.isnan(wse[i]):
                break

        return SteadySolution(
            stream_distance, self._reach.thalweg(), wse)

    def solve(self, method='sstep'):
        """Solve the plan

        Parameters
        ----------
        method : {'sstep', 'simul'}, optional
            Solution method. The default is 'sstep'

        Returns
        -------
        SteadySolution

        """

        if method == 'sstep':
            return self._sstep()
        elif method == 'simul':
            return self._simultaneous()
        else:
            raise ValueError("Unknown solution method: {}".format(method))


class SimultaneousSolver:
    """Simultaneous solution method solver

    Parameters
    ----------
    reach : Reach
    flow_data : SteadyFlow
    y_d : float
        Downstream water surface elevation
    dy : float, optional
        Elevation difference for estimating minimization function derivative

    """

    def __init__(self, reach, flow_data, y_d, dy=0.01):

        self._reach = reach
        self._flow = flow_data.flow(reach.stream_distance())
        self._y_d = y_d

        if dy > 0:
            self._dy = dy
        else:
            raise ValueError("dy must be greater than zero")

    def _f(self, wse, i):

        return self._reach.energy_diff(
            wse[i + 1], self._flow[i + 1], i + 1, wse[i], self._flow[i], i)

    def _f_prime(self, wse, i, j):

        delta_y = np.array([-self._dy / 2, self._dy / 2])

        if i == j:
            y_i = wse[i] + delta_y
            y_j = wse[i + 1]
        else:
            y_i = wse[i]
            y_j = wse[j] + delta_y

        f = self._reach.energy_diff(
            y_j, self._flow[j], j, y_i, self._flow[i], i)
        df = f[1] - f[0]

        return df / self._dy

    def solve_iteration(self, wse):
        """Solve an iteration of the simultaneous solution method

        Parameters
        ----------
        wse : array_like
            Water surface elevation

        Returns
        -------
        numpy.ndarray

        """

        l = 0
        u = 1
        M = len(self._reach)

        ab = np.zeros((l + u + 1, M))
        f = np.empty((M, ))
        f[:] = np.nan

        for i in range(M - 1):
            for j in range(i, i + 2):
                ab[u + i - j, j] = self._f_prime(wse, i, j)
            f[i] = self._f(wse, i)

        i = M - 1
        j = M - 1
        ab[u + i - j, j] = 1
        f[i] = wse[i] - self._y_d

        l_and_u = (l, u)
        return -solve_banded(l_and_u, ab, f)


class StandardStepSolver:
    """

    Parameters
    ----------
    reach: Reach
    flow_data: SteadyFlow

    """

    def __init__(self, reach, flow_data):

        self._reach = reach
        self._stream_distance = reach.stream_distance()
        self._flow = flow_data.flow(self._stream_distance)

    def _solver_func(self, qj, j, yi, qi, i):

        return lambda y: self._reach.energy_diff(y, qj, j, yi, qi, i)

    def solve_node(self, j, i, wse_i):
        """Find the water surface elevation for a reach node.

        Uses the standard - step solution method.

        Parameters
        ----------
        j: int
            Current node index
        i: int
            Previous node index
        wse_i: float
            Water surface elevation of previous node

        Returns
        -------
        float
            Solution for water surface elevation at node i

        """

        qi = self._flow[i]
        qj = self._flow[j]

        args = (qj, j, wse_i, qi, i)

        solver_func = self._solver_func(*args)

        x0 = wse_i
        x1 = x0 + 0.7 * solver_func(x0)

        try:
            root = newton(solver_func, x0, x1=x1)
        except RuntimeError:
            root = np.nan

        return root
