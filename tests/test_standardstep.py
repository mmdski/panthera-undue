import unittest

import numpy as np

from pantherapy import CrossSection, Reach, StandardStep
from pantherapy.panthera import StandardStepOptions


class TestStandardStep(unittest.TestCase):

    def test_init(self):
        """Test initialization of a StandardStep"""

        # test successful init
        xs_y = np.array([1, 0, 0, 0, 1])
        xs_z = np.array([1, 1, 2, 3, 3])
        roughness = np.array([0.030])
        xs = CrossSection(xs_y, xs_z, roughness)

        slope = 0.001
        x = np.linspace(0, 1e3)
        y = slope * x
        xs_numbers = np.zeros_like(x, dtype='int32')
        xs_table = {0: xs}
        reach = Reach(x, y, xs_numbers, xs_table)

        sstep = StandardStep(reach)

        self.assertIsInstance(sstep, StandardStep)

    def test_normal_solution(self):
        """Test for a normal flow solution with US and DS boundary"""

        boundary_depth = 0.75

        xs_y = [2, 0, 0, 0, 2]
        xs_z = [0, 0, 0.5, 1, 1]
        roughness = [0.03]

        n_nodes = 25
        last_node = n_nodes - 1
        slope = 0.001
        dx = 10

        xs = CrossSection(xs_y, xs_z, roughness)

        normal_area = xs.area(boundary_depth)
        normal_h_radius = xs.hydraulic_radius(boundary_depth)
        normal_q = 1 / roughness[0] * normal_area * \
            normal_h_radius**(2/3) * np.sqrt(slope)

        x = np.linspace(0, (n_nodes-1)*dx, n_nodes)
        y = (n_nodes - np.arange(n_nodes) - 1) * dx * slope
        xs_number = np.zeros_like(x, dtype=np.int32)
        xs_table = {0: xs}
        reach = Reach(x, y, xs_number, xs_table)
        sstep = StandardStep(reach)

        q_table = {last_node: normal_q}

        us_boundary_wse = boundary_depth + y[0]
        us_options = StandardStepOptions(q_table, us_boundary_wse, True)

        ds_boundary_wse = boundary_depth + y[-1]
        ds_options = StandardStepOptions(q_table, ds_boundary_wse, False)

        us_res = sstep.solve(us_options)
        ds_res = sstep.solve(ds_options)

        _, us_wse = us_res.ws_elevation()
        _, ds_wse = ds_res.ws_elevation()

        us_depth = us_wse - y
        ds_depth = ds_wse - y

        atol = 0.01

        self.assertTrue(np.alltrue(np.abs(us_depth - boundary_depth) <= atol))
        self.assertTrue(np.alltrue(np.abs(ds_depth - boundary_depth) <= atol))
