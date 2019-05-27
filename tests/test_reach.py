import unittest

import numpy as np

from pantherapy import CrossSection, Reach


class TestReach(unittest.TestCase):

    def test_init(self):
        """Test initialization of a Reach"""

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
        self.assertIsInstance(reach, Reach)

        # test init with non-dict_like object as table
        self.assertRaisesRegex(
            AttributeError, "'NoneType' object has no attribute 'items'",
            Reach, *(x, y, xs_numbers, None))

        # test failure with 2D arrays
        x_2D = np.array([x])
        y_2D = np.array([y])
        xs_numbers_2D = np.array([xs_numbers])
        self.assertRaisesRegex(
            ValueError, "'x' must be 1D", Reach, *
            (x_2D, y, xs_numbers, xs_table)
        )
        self.assertRaisesRegex(
            ValueError, "'y' must be 1D", Reach, *
            (x, y_2D, xs_numbers, xs_table)
        )
        self.assertRaisesRegex(
            ValueError, "'xs_number' must be 1D", Reach, *
            (x, y, xs_numbers_2D, xs_table)
        )

        x_small = x[0:-1]
        y_small = y[0:-1]
        xs_numbers_small = xs_numbers[0:-1]

        # test failure with irregularly size array
        self.assertRaisesRegex(
            ValueError, "The size of x, y, and xs_number must be equal",
            Reach, *(x_small, y, xs_numbers, xs_table)
        )
        self.assertRaisesRegex(
            ValueError, "The size of x, y, and xs_number must be equal",
            Reach, *(x, y_small, xs_numbers, xs_table)
        )
        self.assertRaisesRegex(
            ValueError, "The size of x, y, and xs_number must be equal",
            Reach, *(x, y, xs_numbers_small, xs_table)
        )

        # test failure with non-cross section values in xs_table
        xs_table_no_xs = {0: 1}
        self.assertRaisesRegex(
            TypeError, "xs_table values must be CrossSection type",
            Reach, *(x, y, xs_numbers, xs_table_no_xs)
        )

        # test failure with incorrect keys
        xs_numbers_ones = np.ones_like(xs_numbers)
        self.assertRaisesRegex(
            ValueError, "Values in 'xs_number' must be keys in 'xs_table'",
            Reach, *(x, y, xs_numbers_ones, xs_table)
        )

        # test failure with x in descending order
        x_descending = np.linspace(1, 0)
        self.assertRaisesRegex(
            ValueError, "Values in 'x' must be in ascending or equal order",
            Reach, *(x_descending, y, xs_numbers, xs_table)
        )

    def test_thalweg(self):
        """Test the thalweg method of Reach"""

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

        thalweg = reach.thalweg()

        self.assertTrue(np.array_equal(x, thalweg[0]))
        self.assertTrue(np.array_equal(y, thalweg[1]))
