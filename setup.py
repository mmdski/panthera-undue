from setuptools import setup, Extension

import os
import glob

import numpy as np

from sphinx.setup_command import BuildDoc


panthera_inc = os.path.abspath('pantherapy/panthera/include/')
panthera_src = glob.glob('pantherapy/panthera/src/**/*.c', recursive=True)

pantherapy_src = ['pantherapy/py_panthera.c']
pantherapy_src.extend(panthera_src)

panthera = Extension('pantherapy.panthera',
                     sources=pantherapy_src,
                     include_dirs=[
                         panthera_inc,
                         np.get_include()
                     ]
                     )

# for sphinx
name = 'pantherapy'
version = '0.0'
release = '0.0.0'
cmdclass = {'build_sphinx': BuildDoc}
docs_source = 'docs/'
docs_build_dir = 'docs/_build'
docs_builder = 'html'

setup(name=name,
      author='Marian Domanski',
      version=release,
      url='https://github.com/mmdski/panthera-undue',
      author_email='mmdski@gmail.com',
      classifiers=[
          'Development Status :: 3 - Alpha',
          'Programming Language :: Python 3',
          'OSI Approved :: GNU General Public License v3 (GPLv3)'
      ],
      description='1-D hydraulics computation library',
      license='GPLv3',
      packages=['pantherapy'],
      ext_modules=[panthera],
      install_requires=['numpy', 'matplotlib'],
      command_options={
          'build_sphinx': {
              'project': ('setup.py', name),
              'version': ('setup.py', version),
              'release': ('setup.py', release),
              'source_dir': ('setup.py', docs_source),
              'build_dir': ('setup.py', docs_build_dir),
              'builder': ('setup.py', docs_builder)}
      }
      )
