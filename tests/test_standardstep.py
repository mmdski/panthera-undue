import unittest

import numpy as np

from pantherapy import CrossSection, Reach, StandardStep


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
