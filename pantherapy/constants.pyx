# cython : language_level 3


cimport pantherapy.cconstants as constants

cdef class Constants:

    @staticmethod
    def gravity():
        """Returns the value for the acceleration due to gravity

        Returns
        -------
        float

        """

        return constants.const_gravity()

    @staticmethod
    def manning():
        """Returns the factor to convert the Manning coefficient from SI
        to the appropriate system of units

        """

        return constants.const_manning()

    @staticmethod
    def set_gravity(double g):
        """Sets the acceleration due to gravity

        Parameters
        ----------
        g : float

        """

        constants.const_set_gravity(g)

    @staticmethod
    def set_manning(double k):
        """Sets the factor to convert the Manning coefficient from SI
        to the appropriate system of units

        Parameters
        ----------
        k : float

        """

        constants.const_set_manning(k)
