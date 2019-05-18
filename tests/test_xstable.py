import unittest

import numpy as np

from pantherapy import CrossSection, XSTable


def init_xs():

    # test successful init
    y = np.array([1, 0, 0, 0, 1])
    z = np.array([1, 1, 2, 3, 3])
    roughness = np.array([0.030])
    z_roughness = np.array([])
    xs = CrossSection(y, z, roughness)
    return xs


class TestXSTable(unittest.TestCase):

    def test_put_get(self):

        xs = init_xs()
        xs_table = XSTable()

        xs_table.put(0, xs)

        y, z = xs.coordinates()

        got_xs = xs_table.get(0)
        y_got, z_got = got_xs.coordinates()

        self.assertTrue(np.array_equiv(y, y_got))
        self.assertTrue(np.array_equiv(z, z_got))
