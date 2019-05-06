import unittest

import numpy as np

from pantherapy import CrossSection


class TestCrossSection(unittest.TestCase):

    def test_init(self):
        """Test initialization of a CrossSection"""

        # test successful init
        x = np.array([1, 1, 2, 3, 3])
        y = np.array([1, 0, 0, 0, 1])
        xs = CrossSection(x, y)
        self.assertIsNotNone(xs)

        self.assertRaises(ValueError, CrossSection, *('a', 'b'))
