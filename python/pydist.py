import glob
import os
import platform
import shutil
import subprocess
import sys

setup_str = \
    """from setuptools import setup, Extension

import numpy as np

xsmodule = Extension('pantherapy.panthera',
                     sources=['py_panthera.c'],
                     include_dirs=[
                         '{}',
                         np.get_include()],
                     library_dirs=[
                         '{}'],
                     libraries=['{}'])

setup(name="pantherapy",
    version="{}",
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
    ext_modules=[xsmodule],
    install_requires = ["numpy"]
)
"""

readme_str = \
    """```
/*
*
*               _   _
*   ___ ___ ___| |_| |_ ___ ___ ___ ___ _ _
*  | . | .'|   |  _|   | -_|  _| .'| . | | |
*  |  _|__,|_|_|_| |_|_|___|_| |__,|  _|_  |
*  |_|                             |_| |___|
*                              Version {}
*
*         Marian Domanski <mmdski@gmail.com>
*
*/
```
"""


def copy_package_files(source_dir, build_dir):
    """copy source files"""

    # copy source files
    c_glob_path = os.path.join(source_dir, '*.c')
    for src in glob.glob(c_glob_path):
        _, root = os.path.split(src)
        dst = os.path.join(build_dir, root)
        shutil.copyfile(src, dst)

    # copy unit test
    unit_test_name = 'test_pantherapy.py'
    src = os.path.join(source_dir, unit_test_name)
    dst = os.path.join(build_dir, unit_test_name)
    shutil.copyfile(src, dst)

    package_dir = 'pantherapy'
    src_package_dir = os.path.join(source_dir, package_dir)
    dst_package_dir = os.path.join(build_dir, package_dir)

    try:
        os.mkdir(dst_package_dir)
    except FileExistsError:
        pass

    py_glob_path = os.path.join(src_package_dir, '*.py')
    for src in glob.glob(py_glob_path):
        _, root = os.path.split(src)
        dst = os.path.join(dst_package_dir, root)
        shutil.copyfile(src, dst)


def write_setup(build_dir, panthera_inc_dir, pantheralib_path,
                project_version):
    """write setup.py"""
    pantheralib_dir, _ = os.path.split(pantheralib_path)
    setup_file_text = setup_str.format(
        panthera_inc_dir, pantheralib_dir, 'panthera', project_version)
    setup_path = os.path.join(build_dir, 'setup.py')
    with open(setup_path, 'w') as f:
        f.writelines(setup_file_text)


def write_readme(build_dir, project_version):
    """write README.md"""
    readme_path = os.path.join(build_dir, 'README.md')
    with open(readme_path, 'w') as f:
        f.writelines(readme_str.format(project_version))


if __name__ == "__main__":

    cpu = sys.argv[1]
    project_version = sys.argv[2]
    source_dir = sys.argv[3]
    build_dir = sys.argv[4]
    panthera_inc_dir = sys.argv[5]
    pantheralib_path = sys.argv[6]
    target = sys.argv[7]

    # make sure the compiler and python architectures match
    if platform.system() == 'Windows':

        # assume anything other than AMD64 is 32-bit
        if platform.machine() == 'AMD64':
            machine = 'x86_64'
        else:
            machine = 'x86'
    else:
        machine = platform.machine()

    if machine != cpu:
        raise ValueError("Python/compiler CPU architectures don't match")

    copy_package_files(source_dir, build_dir)
    write_setup(build_dir, panthera_inc_dir, pantheralib_path, project_version)
    write_readme(build_dir, project_version)

    os.chdir(build_dir)
    python_exe = sys.executable
    if target == 'test':
        command = 'test'
    elif target == 'build':
        command = 'build'
    elif target == 'install':
        command = 'install'
    elif target == 'dist':
        command = 'bdist_wheel'

    subprocess.call([python_exe, 'setup.py', command])
