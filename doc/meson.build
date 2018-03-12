doxygen = find_program('doxygen', required : false)
if doxygen.found()
    configure_file(input : 'Doxyfile.in',
                   output : 'Doxyfile',
                   configuration : conf_data)
    # TODO
    # run_target('docs', 'doxygen', meson.current_build_dir() + '/Doxyfile')
endif
