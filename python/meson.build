pydist_path = join_paths(meson.current_source_dir(), 'pydist.py')

panthera_include_dir = join_paths(meson.source_root(), 'include')

run_target('pydist',
            command : [
                       py_installation , pydist_path,
                       target_machine.cpu(),
                       meson.project_version(),
                       meson.current_source_dir(),
                       meson.current_build_dir(),
                       panthera_include_dir,
                       pantheralib.full_path(),
                       'dist'
                      ],
            depends : pantheralib)

run_target('pyinstall',
            command : [
                       py_installation , pydist_path,
                       target_machine.cpu(),
                       meson.project_version(),
                       meson.current_source_dir(),
                       meson.current_build_dir(),
                       panthera_include_dir,
                       pantheralib.full_path(),
                       'install'
                      ],
            depends : pantheralib)
