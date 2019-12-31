import numpy as np

from pantherapy.panthera import Constants


class ReachNode:

    def __init__(self, x, y, xs):

        self.x = x
        self.y = y
        self.xs = xs

    def friction_slope(self, y, q):
        """Computes friction slope for this node

        Computes the friction slope for this node for water surface
        elevation `y` and flow `q`.

        Parameters
        ----------
        y : float
            Water surface elevation
        q : float
            Flow

        Returns
        -------
        float
            Friction slope

        """

        y = np.array(y)
        q = np.array(q)

        depth = y - self.y
        conveyance = self.xs.conveyance(depth)
        return q**2 / conveyance**2

    def velocity(self, y, q):
        """Computes mean velocity

        Computes the mean cross section velocity for this node for
        water surface elevation `y` and flow `q`.

        Parameters
        ----------
        y : float
            Water surface elevation
        q : float
            Flow

        Returns
        -------
        float
            Mean velocity

        """

        depth = y - self.y
        area = self.xs.area(depth)
        return q / area

    def velocity_head(self, y, q):
        """Computes velocity head

        Computes the velocity head for this node for water surface
        elevation `y` and flow `q`.

        Parameters
        ----------
        y : float
            Water surface elevation
        q : float
            Flow

        Returns
        -------
        float
            Velocity head

        """

        depth = y - self.y
        velocity = self.velocity(y, q)
        velocity_coeff = self.xs.velocity_coeff(depth)
        return velocity_coeff * velocity**2 / (2 * Constants.gravity())


class Reach:
    """Stream reach

    """

    def __init__(self):

        self._nodes = {}
        self._array = None

    def __len__(self):

        return len(self._nodes)

    def _build_array(self):

        x_distance = np.array(list(self._nodes.keys()))
        x_sort_idx = np.argsort(x_distance)

        node_array = np.array(list(self._nodes.values()))
        self._array = node_array[x_sort_idx]

    def energy_diff(self, yj, qj, j, yi, qi, i):
        """Specific energy difference between nodes

        Parameters
        ----------
        yj : float
            Water surface elevation at node j
        qj : float
            Flow at node j
        j : int
            Index of node j
        yi : float
            Water surface elevation at node i
        qi : float
            Flow at node i
        i : int
            Index of node i

        Returns
        -------
        float
            Specific energy difference

        """

        hvi = self.velocity_head(i, yi, qi)
        hvj = self.velocity_head(j, yj, qj)

        sfi = self.friction_slope(i, yi, qi)
        sfj = self.friction_slope(j, yj, qj)

        xi = self.node_location(i)
        xj = self.node_location(j)

        dx = xj - xi

        head_loss = dx / 2 * (sfi + sfj)

        return (yj + hvj) - (yi + hvi) + head_loss

    def friction_slope(self, i, h, q):
        """Computes the friction slope at a node

        Parameters
        ----------
        i : int
            Node index
        h : float
            Stage to compute friction slope
        q : float
            Discharge to compute friction slope

        Returns
        -------
        float
            Friction slope

        """

        if self._array is None:
            self._build_array()

        return self._array[i].friction_slope(h, q)

    def node_location(self, i):
        """Returns distance downstream of node

        Parameters
        ----------
        i : int
            Node index

        Returns
        -------
        float
            Distance downstream of node

        """

        if self._array is None:
            self._build_array()

        return self._array[i].x

    def put(self, xs, x, y=0):
        """Puts a node in the reach

        Parameters
        ----------
        xs : CrossSection
            Cross section in node
        x : float
            Stream distance
        y : float, optional
            Thalweg elevation (the default is 0)

        """

        node = ReachNode(x, y, xs)
        self._nodes[x] = node
        self._array = None

    def stream_distance(self):
        """Returns the stream distance of each node

        Returns
        -------
        numpy.ndarray
            Stream distance

        """

        if self._array is None:
            self._build_array()

        x = [node.x for node in self._array]

        return np.array(x)

    def thalweg(self):
        """Returns the thalweg elevation of each node

        Returns
        -------
        numpy.array
            Thalweg elevation

        """

        if self._array is None:
            self._build_array()

        y = [node.y for node in self._array]

        return np.array(y)

    def velocity_head(self, i, h, q):
        """Computes the velocity head at a node

        Parameters
        ----------
        i : int
            Node index
        h : float
            Stage to compute friction slope
        q : float
            Discharge to compute friction slope

        Returns
        -------
        float
            Velocity head

        """

        if self._array is None:
            self._build_array()

        return self._array[i].velocity_head(h, q)
