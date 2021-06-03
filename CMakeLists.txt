cmake_minimum_required (VERSION 3.5.1)

project (IBEX_NCONT VERSION 0.42 LANGUAGES CXX)
set (IBEX_NCONT_DESCRIPTION "Plugin for multi-parametric continuation")
set (IBEX_MIN_VERSION_REQ 2.8.9.1)

# If IBEX_DIR was given on the command-line and not in the environment, set it
# as an environment variable. Enable the usage of 'cmake -DIBEX_DIR=<path> ...'
if (NOT DEFINED ENV{IBEX_DIR} AND DEFINED IBEX_DIR)
  set (ENV{IBEX_DIR} "${IBEX_DIR}")
endif ()
find_package (IBEX ${IBEX_MIN_VERSION_REQ} REQUIRED)
message (STATUS "Found Ibex version ${IBEX_VERSION}")
ibex_init_common () # Ibex should have installed this function

# check if Ibex was compiled with an linear programming library
if (NOT IBEX_LP_LIB_NAME)
  message (FATAL_ERROR "A Linear Programming library is needed for this plugin")
endif ()

################################################################################
# Compile sources
################################################################################
add_subdirectory (src)

################################################################################
# Tests
################################################################################
add_make_target_for_ctest (check)

