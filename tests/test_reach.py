from unittest import TestCase

import numpy as np

from pantherapy.reach import Reach


class TestXS:

    def __init__(self, area, conveyance):

        self._area = area
        self._conveyance = conveyance

    def area(self, depth):

        return self._area

    def conveyance(self, depth):

        return self._conveyance

    def velocity_coeff(self, depth):

        return 1


class TestReach(TestCase):

    def test_init(self):
        """Test initialization of Reach"""

        S = 0.001

        xs = TestXS(1, 1)

        x_reach = np.linspace(0, 1e3)
        y_reach = S*x_reach[::-1]

        reach = Reach()

        for x, y in zip(x_reach, y_reach):
            reach.put(xs, x, y)

    def test_stream_values(self):
        """Test correctness of stream_distance and thalweg methods"""

        S = 0.001

        xs = TestXS(1, 1)

        x_reach = np.linspace(0, 1e3)
        y_reach = S*x_reach[::-1]

        reach = Reach()

        for x, y in zip(x_reach, y_reach):
            reach.put(xs, x, y)

        self.assertTrue(np.array_equal(x_reach, reach.stream_distance()))
        self.assertTrue(np.array_equal(y_reach, reach.thalweg()))

    def test_node_values(self):
        """Test the computation of the values at nodes"""

        S = 0.001

        xs = TestXS(1, 1)

        x_reach = np.linspace(0, 1e3)
        y_reach = S*x_reach[::-1]

        reach = Reach()

        for x, y in zip(x_reach, y_reach):
            reach.put(xs, x, y)

        expected_velocity_head = 1/(2*9.81)

        for i, y in enumerate(y_reach):
            self.assertTrue(reach.friction_slope(i, y, 1) == 1)
            self.assertAlmostEqual(
                expected_velocity_head, reach.velocity_head(i, y, 1))
