# cython : language_level 3


cimport pantherapy.cconstants as constants

cdef class Constants:
    """Physical constants

    Constants is a singleton class and shouldn't be initalized. All
    methods are static.

    """

    def __init__(self):
        raise NotImplementedError("cannot initialize Constants")

    @staticmethod
    def gravity():
        """Returns the value for the acceleration due to gravity

        Returns
        -------
        float
            Acceleration due to gravity

        """

        return constants.const_gravity()

    @staticmethod
    def manning():
        """Returns the factor to convert the Manning coefficient from SI
        to the appropriate system of units

        Returns
        -------
        float
            Manning coefficient conversion factor

        """

        return constants.const_manning()

    @staticmethod
    def set_gravity(double g):
        """set_gravity(g)

        Sets the acceleration due to gravity

        Parameters
        ----------
        g : float

        """

        constants.const_set_gravity(g)

    @staticmethod
    def set_manning(double k):
        """set_manning(k)

        Sets the factor to convert the Manning coefficient from SI
        to the appropriate system of units

        Parameters
        ----------
        k : float

        """

        constants.const_set_manning(k)
