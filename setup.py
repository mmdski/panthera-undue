from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext as _build_ext

from Cython.Build import cythonize

import os
import glob

# handle case when Sphinx isn't installed
try:
    from sphinx.setup_command import BuildDoc
    sphinx_imported = True
except ImportError:
    sphinx_imported = False


# binary build
class build_ext(_build_ext):
    def finalize_options(self):
        _build_ext.finalize_options(self)
        # Prevent numpy from thinking it is still in its setup process:
        __builtins__.__NUMPY_SETUP__ = False
        import numpy
        self.include_dirs.append(numpy.get_include())


panthera_inc = os.path.abspath('include')
panthera_src = glob.glob('src/*.c', recursive=True)

pantherapy_src = ['pantherapy/panthera.pyx']
pantherapy_src.extend(panthera_src)
pantherapy_ext = Extension('pantherapy.panthera',
                           sources=pantherapy_src,
                           include_dirs=[panthera_inc]
                           )

ext_modules = cythonize([pantherapy_ext], annotate=True)

# general setup
name = 'pantherapy'
release = '0.0.1'
version = '0.0'

setup_kwargs = {
    'name': name,
    'author': 'Marian Domanski',
    'version': release,
    'url': 'https://github.com/mmdski/panthera-undue',
    'author_email': 'mmdski@gmail.com',
    'classifiers': [
        'Development Status :: 1 - Planning',
        'Intended Audience :: Science/Research',
        'Programming Language :: C',
        'Programming Language :: Python :: 3',
        'License :: OSI Approved :: GNU General Public License v3 (GPLv3)',
        'Topic :: Scientific/Engineering :: Hydrology'
    ],
    'description': '1D hydraulics',
    'license': 'GPLv3',
    'packages': ['pantherapy'],
    'ext_modules': ext_modules,
    'install_requires': ['matplotlib', 'numpy', 'scipy'],
    'setup_requires': ['cython', 'numpy'],
    'python_requires': '>=3.7',
    'cmdclass': {'build_ext': build_ext},
}

# for sphinx
if sphinx_imported:
    docs_source = 'docs/'
    docs_build_dir = 'docs/_build'
    docs_builder = 'html'
    setup_kwargs['cmdclass'].update({'build_sphinx': BuildDoc})
    sphinx_kwargs = {
        'command_options': {
            'build_sphinx': {
                'project': ('setup.py', name),
                'version': ('setup.py', version),
                'release': ('setup.py', release),
                'source_dir': ('setup.py', docs_source),
                'build_dir': ('setup.py', docs_build_dir),
                'builder': ('setup.py', docs_builder)}
        }
    }
    setup_kwargs.update(sphinx_kwargs)

setup(**setup_kwargs)
