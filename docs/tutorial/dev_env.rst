Setting Up the Development Environment
======================================

On Linux (Ubuntu flavors)
-------------------------

First, clone the repository.


1. Install system packages

    .. code-block:: console

        $ apt-get install gcc python3-venv python3-dev valgrind libglib2.0-dev


2. Create and activate a Python virtual environment.

    .. code-block:: console

        $ python3 -m venv env
        $ . env/bin/activate
        (env) $


3. Install the required Python packages.

    .. code-block:: console

        (env) $ pip install -r requirements.txt


4. Test the panthera backend.

    .. code-block:: console

        (env) $ meson build && ninja -C build test


5. Install pantherapy

    .. code-block:: batch

        (env) $ pip install -e .


6. Run the Python test suite.

    .. code-block:: batch

        (env) $ python setup.py test


7. Build the documentation.

    .. code-block:: batch

        (env) $ python setup.py build_sphinx

On Windows
----------

Unit and memory tests for the panthera backend are not available on Windows
since the tests require GLib and Valgrind.

First, clone the repository.


1. Download and install `Visual C++ Build Tools <https://visualstudio.microsoft.com/visual-cpp-build-tools/>`_.


2. Download and install Python.

    pantherapy requires Python 3.6 or greater.


3. Create and activate a Python virtual environment.

    .. code-block:: batch

        >python -m venv env
        >env\Scripts\activate
        (env) >


4. Install the required packages.

    .. code-block:: batch

        (env) >pip install -r requirements.txt


5. Install pantherapy

    .. code-block:: batch

        (env) >pip install -e .


6. Run the Python test suite.

    .. code-block:: batch

        (env) >python setup.py test


7. Build the documentation.

    .. code-block:: batch

        (env) >python setup.py build_sphinx
