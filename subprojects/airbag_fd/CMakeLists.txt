add_library(airbag_fd STATIC
        airbag_fd.c
    )
target_include_directories(airbag_fd INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )
