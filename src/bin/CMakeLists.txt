################################################################################
# First example: compiling a binary that depends on libibex-ncont
################################################################################
# source files of sphere
list (APPEND SRC1 ${CMAKE_CURRENT_SOURCE_DIR}/sphere.cpp
                  # Add more source files here.
                  # For most plugins, the rest can be left as is.
                  )

# Create the target for sphere
add_executable (sphere ${SRC1})
target_link_libraries (sphere PUBLIC ibex-ncont)

# install the binary
install (TARGETS sphere DESTINATION ${CMAKE_INSTALL_BINDIR})

################################################################################
# Second example: compiling a binary that depends only on libibex
################################################################################
# source files of torus
#list (APPEND SRC2 ${CMAKE_CURRENT_SOURCE_DIR}/torus.cpp
                  ## Add more source files here.
                  ## For most plugins, the rest can be left as is.
                  #)

## Create the target for torus
#add_executable (torus ${SRC2})
#target_link_libraries (torus PUBLIC Ibex::ibex)

## install the binary
#install (TARGETS torus DESTINATION ${CMAKE_INSTALL_BINDIR})

