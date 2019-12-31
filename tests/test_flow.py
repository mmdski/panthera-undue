from unittest import TestCase

import numpy as np

from pantherapy.steady.flow import SteadyFlow


class TestSteadyFlow(TestCase):

    def test_flow(self):
        """Test flow values from flow data"""

        flow_values = {0: 10, 10: 20}

        flow_data = SteadyFlow()

        for x, q in flow_values.items():
            flow_data.set_flow(x, q)

        x_test = np.linspace(0, 9.99)
        expected_flow = 10 * np.ones_like(x_test)
        self.assertTrue(np.array_equal(expected_flow, flow_data.flow(x_test)))

        x_test = np.linspace(10, 100)
        expected_flow = 20 * np.ones_like(x_test)
        self.assertTrue(np.array_equal(expected_flow, flow_data.flow(x_test)))
