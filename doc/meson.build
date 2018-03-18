doxygen = find_program('doxygen', required : false)
if doxygen.found()
    doxyfile = configure_file(
        input : 'Doxyfile.in',
        output : 'Doxyfile',
        configuration : conf_data,
        install : false)

    datadir = join_paths(get_option('datadir'), 'doc', 'ocher')

    docs = custom_target('docs',
        input : doxyfile,
        output : 'html',
        command : [doxygen, doxyfile],
        install : true,
        install_dir : datadir)
endif
