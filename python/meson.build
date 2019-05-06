pydist_path = join_paths(meson.current_source_dir(), 'pydist.py')

panthera_include_dir = join_paths(meson.source_root(), 'include')

command_args = [ 
                py_installation , pydist_path,
                target_machine.cpu(),
                meson.project_version(),
                meson.current_source_dir(),
                meson.current_build_dir(),
                panthera_include_dir,
                pantheralib.full_path(),
               ]

run_target('pytest',
            command : [command_args, 'test'],
            depends : pantheralib)

run_target('pybuild',
            command : [command_args, 'build'],
            depends : pantheralib)

run_target('pyinstall',
            command : [command_args, 'install'],
            depends : pantheralib)

run_target('pydist',
            command : [command_args,'dist'],
            depends : pantheralib)
