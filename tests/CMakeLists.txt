set (TESTS_LIST TestCellDiff TestCont TestParametricProof)

foreach (test ${TESTS_LIST})
  # /!\ The test and the target building the executable have the same name
  add_executable (${test} ${test}.cpp ${test}.h)
  target_link_libraries (${test} ibex-ncont)
  add_dependencies (check ${test})
  add_test (${test} ${test})
endforeach ()
