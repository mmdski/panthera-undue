import unittest

import numpy as np

from pantherapy import CrossSection


class TestCrossSection(unittest.TestCase):

    def test_init(self):
        """Test initialization of a CrossSection"""

        # test successful init
        y = np.array([1, 0, 0, 0, 1])
        z = np.array([1, 1, 2, 3, 3])
        xs = CrossSection(y, z)
        self.assertIsNotNone(xs)

        # non-float check
        message = 'could not convert string to float: \'a\''
        self.assertRaisesRegex(ValueError, message, CrossSection, *('a', z))

        # 1D check
        y_2D = np.array([y])
        z_2D = np.array([z])
        one_d_message = 'y and z must be 1D arrays'
        self.assertRaisesRegex(ValueError, one_d_message,
                               CrossSection, *(y_2D, y))
        self.assertRaisesRegex(ValueError, one_d_message,
                               CrossSection, *(y, z_2D))
        self.assertRaisesRegex(ValueError, one_d_message, CrossSection,
                               *(y_2D, z_2D))

        # equal size check
        y_big = np.array([1, 0, 0, 0, 1, 2])
        z_big = np.array([1, 1, 2, 3, 3, 3])
        big_message = 'the size of y and z must be equal'
        self.assertRaisesRegex(ValueError, big_message,
                               CrossSection, *(y_big, z))
        self.assertRaisesRegex(ValueError, big_message,
                               CrossSection, *(y, z_big))
#
