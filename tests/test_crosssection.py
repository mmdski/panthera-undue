import unittest

import numpy as np

from pantherapy.panthera import CrossSection


class TestCrossSection(unittest.TestCase):

    def test_init(self):
        """Test initialization of a CrossSection"""

        # test successful init
        y = np.array([1, 0, 0, 0, 1])
        z = np.array([1, 1, 2, 3, 3])
        roughness = 0.030
        xs = CrossSection(y, z, roughness)
        self.assertIsNotNone(xs)

        # non-float check
        message = 'could not convert string to float: \'a\''
        self.assertRaisesRegex(ValueError, message,
                               CrossSection, *('a', z, roughness))

        # 1D check
        y_2D = np.array([y])
        z_2D = np.array([z])
        one_d_message = 'y and z must be one-dimensional'
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
        big_message = 'y and z must be the same size'
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
        descending_message = 'z must be in ascending order'
        self.assertRaisesRegex(ValueError, descending_message,
                               CrossSection, *(y, z_descending,
                                               roughness))

    def test_get_init_values(self):
        """Test values are equal after init of a CrossSection"""

        y = np.array([1, 0, 0, 0, 1])
        z = np.array([1, 1, 2, 3, 3])
        roughness = 0.030
        xs = CrossSection(y, z, roughness)
        y_xs, z_xs = xs.coordinates()

        self.assertTrue(np.array_equal(y, y_xs))
        self.assertTrue(np.array_equal(z, z_xs))

    def test_area(self):
        """Test CrossSection.area"""

        # rectangle
        y = np.array([1, 0, 0, 0, 1])
        z = np.array([0, 0, 1, 2, 2])
        roughness = 0.030

        width = z[-1] - z[0]

        depth = np.linspace(y.min(), y.max()) - y.min()
        expected_area = width * depth

        print("initializing cross section")
        xs = CrossSection(y, z, roughness)
        print("cross section initialized")
        calculated_area = xs.area(depth)
        self.assertTrue(np.array_equal(expected_area, calculated_area))
        self.assertEqual(xs.area(-1), 0)
        self.assertTrue(np.isnan(xs.area(np.nan)))

        # elevated cross section
        y = y + 20
        depth = np.linspace(y.min(), y.max()) - y.min()
        expected_area = width * depth
        xs = CrossSection(y, z, roughness)
        calculated_area = xs.area(depth + 20)
        self.assertTrue(np.array_equal(expected_area, calculated_area))
        self.assertEqual(xs.area(-1), 0)
        self.assertTrue(np.isnan(xs.area(np.nan)))

    def test_critical_flow(self):

        # rectangle
        y = np.array([1, 0, 0, 0, 1])
        z = np.array([0, 0, 0.5, 1, 1])
        roughness = 0.030
        xs = CrossSection(y, z, roughness)

        yc = np.linspace(0.1, 0.95)
        qc = xs.critical_flow(yc)
        yc_estimate = xs.critical_depth(qc)
        self.assertTrue(np.allclose(yc, yc_estimate, rtol=1e-3, atol=0))

    def test_normal_flow(self):

        # rectangle
        y = np.array([1, 0, 0, 0, 1])
        z = np.array([0, 0, 0.5, 1, 1])
        roughness = 0.030
        xs = CrossSection(y, z, roughness)

        slope = 0.001
        yn = np.linspace(0.1, 0.95)
        qn = xs.normal_flow(yn, slope)

        yn_estimate = xs.normal_depth(qn, slope)
        self.assertTrue(np.allclose(yn, yn_estimate, rtol=1e-3, atol=0))

    def test_specific_energy(self):

        # rectangle
        y = np.array([5, 0, 0, 0, 5])
        z = np.array([0, 0, 0.5, 1, 1])
        roughness = 0.030
        xs = CrossSection(y, z, roughness)

        yc = 0.75
        Qc = xs.critical_flow(yc)

        y = np.linspace(0.25, 3.6)
        width = z[-1] - z[0]
        q = Qc / width
        e_expected = y + q**2 / (2 * 9.81 * y**2)

        e_computed = xs.specific_energy(y, Qc)

        self.assertTrue(np.allclose(e_expected, e_computed, rtol=1e-5, atol=0))
        self.assertTrue(np.isnan(xs.specific_energy(np.nan, Qc)))
