=========
Constants
=========

Application-wide management of physical constants

.. c:function:: double const_gravity()

    Returns the acceleration due to gravity

.. c:function:: double const_manning()

    Returns the conversion factor k used to convert the Manning coefficient
    from SI units.

.. c:function:: void const_set_gravity(double g)

    Sets the acceleration due to gravity to *g*.

.. c:function:: void const_set_manning(double k)

    Sets the conversion factor used to convert the Manning coefficient from
    SI units to *k*.

    Typical values are 1 for SI and 1.49 for English.
