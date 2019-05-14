from setuptools import setup, Extension

import os
import glob

import numpy as np

panthera_inc = os.path.abspath('pantherapy/panthera/include/')
panthera_src = glob.glob('pantherapy/panthera/src/**/*.c', recursive=True)

pantherapy_src = ['pantherapy/py_panthera.c']
pantherapy_src.extend(panthera_src)

panthera = Extension('pantherapy.panthera',
                     sources=pantherapy_src,
                     include_dirs=[
                         panthera_inc,
                         np.get_include()])

setup(name="pantherapy",
      version="0.0.0",
      url="https://github.com/mmdski/panthera-undue",
      author="Marian Domanski",
      author_email="mmdski@gmail.com",
      classifiers=[
          'Development Status :: 3 - Alpha',
          'Programming Language :: Python 3',
          'OSI Approved :: GNU General Public License v3 (GPLv3)'
      ],
      description="1-D hydraulics computation library",
      license="GPLv3",
      packages=["pantherapy"],
      ext_modules=[panthera],
      install_requires=["numpy", "matplotlib"]
      )
