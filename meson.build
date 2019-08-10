project('panthera-undue', 'c',
    default_options : ['c_std=c99', 'c_args=-Wall', 'b_ndebug=if-release'],
    version : '0.0.0')

inc = include_directories('include')

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)

subdir('src')
subdir('app')

subdir('tests')
