import unittest

import numpy as np

from pantherapy import CrossSection


class TestCrossSection(unittest.TestCase):

    def test_init(self):
        """Test initialization of a CrossSection"""

        # test successful init
        y = np.array([1, 0, 0, 0, 1])
        z = np.array([1, 1, 2, 3, 3])
        roughness = np.array([0.030])
        z_roughness = np.array([])
        xs = CrossSection(y, z, roughness)
        self.assertIsNotNone(xs)

        # non-float check
        message = 'could not convert string to float: \'a\''
        self.assertRaisesRegex(ValueError, message,
                               CrossSection, *('a', z, roughness))

        # 1D check
        y_2D = np.array([y])
        z_2D = np.array([z])
        one_d_message = 'y and z must be 1D arrays'
        self.assertRaisesRegex(ValueError, one_d_message,
                               CrossSection, *(y_2D, z,
                                               roughness))
        self.assertRaisesRegex(ValueError, one_d_message,
                               CrossSection, *(y, z_2D,
                                               roughness))
        self.assertRaisesRegex(ValueError, one_d_message,
                               CrossSection, *(y_2D, z_2D,
                                               roughness))

        # equal size check
        y_big = np.array([1, 0, 0, 0, 1, 2])
        z_big = np.array([1, 1, 2, 3, 3, 3])
        big_message = 'the size of y and z must be equal'
        self.assertRaisesRegex(ValueError, big_message,
                               CrossSection, *(y_big, z,
                                               roughness))
        self.assertRaisesRegex(ValueError, big_message,
                               CrossSection, *(y, z_big,
                                               roughness))

        y_small = np.array([1])
        z_small = np.array([1])
        small_message = 'the length of y and z must be greater than 2'
        self.assertRaisesRegex(ValueError, small_message,
                               CrossSection, *(y_small, z_small,
                                               roughness))

        z_descending = np.array([3, 3, 2, 1, 1])
        descending_message = 'z must be in ascending or equal order'
        self.assertRaisesRegex(ValueError, descending_message,
                               CrossSection, *(y, z_descending,
                                               roughness))

    def test_get_init_values(self):
        """Test values are equal after init of a CrossSection"""

        y = np.array([1, 0, 0, 0, 1])
        z = np.array([1, 1, 2, 3, 3])
        roughness = np.array([0.030])
        z_roughness = np.array([])
        xs = CrossSection(y, z, roughness)
        y_xs, z_xs = xs.coordinates()

        self.assertTrue(np.array_equal(y, y_xs))
        self.assertTrue(np.array_equal(z, z_xs))

    def test_area(self):
        """Test CrossSection.area"""

        # rectangle
        y = np.array([1, 0, 0, 0, 1])
        z = np.array([0, 0, 0.5, 1, 1])
        roughness = np.array([0.030])

        width = z[-1] - z[0]

        depth = np.linspace(0, 1)
        expected_area = width * depth

        xs = CrossSection(y, z, roughness)
        calculated_area = xs.area(depth)

        self.assertTrue(np.array_equal(expected_area, calculated_area))
        self.assertTrue(xs.area(1) == 1)
        self.assertRaisesRegex(
            ValueError, 'Depth values must be greater than or equal to ' +
            'lowest z value in cross section', xs.area, (-1))