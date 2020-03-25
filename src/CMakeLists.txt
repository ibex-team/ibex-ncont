################################################################################
# First example: compiling a library that depends on libibex
################################################################################
# source files of libibex-ncont
list (APPEND SRC ${CMAKE_CURRENT_SOURCE_DIR}/ibex_Cont.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/ibex_Cont.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/ibex_ContCell.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/ibex_ContCell.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/ibex_ParametricProof.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/ibex_ParametricProof.h

                 # Add more source files here.
                 # For most plugins, the rest can be left as is.
                 )

# Create the target for libibex-ncont
add_library (ibex-ncont ${SRC})
# TODO BUILD_INTERFACE and INSTALL_INTERFACE for inc dirs
target_include_directories (ibex-ncont PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (ibex-ncont PUBLIC Ibex::ibex)

# installation of libibex-ncont files
list_filter_header (HDR ${SRC}) # Ibex should have installed this function
install (TARGETS ibex-ncont DESTINATION ${CMAKE_INSTALL_LIBDIR})
install (FILES ${HDR} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ibex-ncont)

################################################################################
# More examples in the bin subdirectory
################################################################################
add_subdirectory (bin)
