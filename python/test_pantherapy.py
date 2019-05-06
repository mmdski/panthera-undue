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

        # non-float check
        message = 'could not convert string to float: \'a\''
        self.assertRaisesRegex(ValueError, message, CrossSection, *('a', y))

        # 1D check
        x_2D = np.array([x])
        y_2D = np.array([y])
        one_d_message = 'y and z must be 1D arrays'
        self.assertRaisesRegex(ValueError, one_d_message,
                               CrossSection, *(x_2D, y))
        self.assertRaisesRegex(ValueError, one_d_message,
                               CrossSection, *(x, y_2D))
        self.assertRaisesRegex(ValueError, one_d_message, CrossSection,
                               *(x_2D, y_2D))

        # equal size check
        x_big = np.array([1, 1, 2, 3, 3, 3])
        y_big = np.array([1, 0, 0, 0, 1, 2])
        big_message = 'the size of y and z must be equal'
        self.assertRaisesRegex(ValueError, big_message,
                               CrossSection, *(x_big, y))
        self.assertRaisesRegex(ValueError, big_message,
                               CrossSection, *(x, y_big))
