from unittest import TestCase

import numpy as np

from pantherapy.panthera import CrossSection
from pantherapy.relation import CriticalRelation, FixedStageRelation, \
    NormalRelation


class TestCrossSection:

    def __init__(self, q, y):

        self._q = q
        self._y = y

    def coordinates(self):

        y = np.linspace(0, 1)
        z = np.linspace(0, 1)

        return y, z

    def critical_depth(self, critical_flow, y0=None):

        return np.interp(critical_flow, self._q, self._y)

    def critical_flow(self, elevation):

        return np.interp(elevation, self._y, self._q)

    def normal_depth(self, normal_flow, slope, y0=None):

        return np.interp(normal_flow, self._q, self._y)

    def normal_flow(self, elevation, slope):

        return np.interp(elevation, self._y, self._q)


class TestFixedStageRelation(TestCase):

    def test_stage(self):
        """Test fixed stage relation"""

        stage = 10
        discharge = np.linspace(0, 50)

        stage_relation = FixedStageRelation(stage)

        for q in discharge:
            self.assertEqual(stage, stage_relation.stage(q))

        expected_stage_array = stage * np.ones_like(discharge)
        computed_stage_array = stage_relation.stage(discharge)
        self.assertTrue(np.array_equal(
            expected_stage_array, computed_stage_array))


class TestNormalRelation(TestCase):

    def test_stage(self):

        normal_depth = np.linspace(0.1, 0.9)
        normal_flow = 100 * normal_depth
        xs = TestCrossSection(normal_flow, normal_depth)

        slope = 0.001

        relation = NormalRelation(xs, slope)

        for i, qn in enumerate(normal_flow):
            self.assertAlmostEqual(normal_depth[i], relation.stage(qn), 3)

    def test_stage_datum(self):

        normal_depth = np.linspace(0.1, 0.9)
        normal_flow = 100 * normal_depth
        xs = TestCrossSection(normal_flow, normal_depth)

        slope = 0.001
        datum = 10

        relation = NormalRelation(xs, slope, datum)

        normal_stage = normal_depth + datum

        for i, qn in enumerate(normal_flow):
            self.assertAlmostEqual(normal_stage[i], relation.stage(qn), 3)


class TestCriticalRelation(TestCase):

    def test_stage(self):

        critical_depth = np.linspace(0.1, 0.9)
        critical_flow = 100 * critical_depth
        xs = TestCrossSection(critical_flow, critical_depth)

        relation = CriticalRelation(xs)

        for i, qn in enumerate(critical_flow):
            self.assertAlmostEqual(critical_depth[i], relation.stage(qn), 3)

    def test_stage_datum(self):

        critical_depth = np.linspace(0.1, 0.9)
        critical_flow = 100 * critical_depth
        xs = TestCrossSection(critical_flow, critical_depth)

        datum = 10

        relation = CriticalRelation(xs, datum)

        critical_stage = critical_depth + datum

        for i, qn in enumerate(critical_flow):
            self.assertAlmostEqual(critical_stage[i], relation.stage(qn), 3)
