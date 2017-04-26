if(${verbose_traversal})
  message(STATUS "Entered ODE/Tests subdirectory")
endif(${verbose_traversal})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB Test_c  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.C")

foreach( test_c ${Test_c} )
  get_filename_component( test ${test_c} NAME_WE )
  add_executable(${test} ${test_c})
  target_link_libraries( ${test} ODE Spline OneDfunction LinkList Io Math String )
endforeach( test_c )

set(EOSLIB_DATABASE_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/ODE)

# list( APPEND tests Harmonic HarmonicF HarmonicB RandomHarmonic)
# 
# foreach(test ${tests})
#   add_executable(${test} ${test}.C)
#   target_link_libraries(${test} ODE)
# #  add_test(${test} ${test})
# endforeach(test)
# 
# #set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data type hydro name std")
# add_test(NAME Harmonic
#   COMMAND ${CMAKE_COMMAND}
#   -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Harmonic
#   -D test_args:string=${args}
#   -D out_blessed=${EOSLIB_ODE_BLESSED_OUTPUT}/HarmonicOut.blessed
#   -D err_blessed=${EOSLIB_ODE_BLESSED_OUTPUT}/HarmonicErr.blessed 
#   -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
#   )
# set_tests_properties(Harmonic PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# 
# #set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data type hydro name std")
# add_test(NAME HarmonicF
#   COMMAND ${CMAKE_COMMAND}
#   -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/HarmonicF
#   -D test_args:string=${args}
#   -D out_blessed=${EOSLIB_ODE_BLESSED_OUTPUT}/HarmonicFOut.blessed
#   -D err_blessed=${EOSLIB_ODE_BLESSED_OUTPUT}/HarmonicFErr.blessed 
#   -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
#   )
# set_tests_properties(HarmonicF PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# 
# #set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data type hydro name std")
# add_test(NAME HarmonicB
#   COMMAND ${CMAKE_COMMAND}
#   -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/HarmonicB
#   -D test_args:string=${args}
#   -D out_blessed=${EOSLIB_ODE_BLESSED_OUTPUT}/HarmonicBOut.blessed
#   -D err_blessed=${EOSLIB_ODE_BLESSED_OUTPUT}/HarmonicBErr.blessed 
#   -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
#   )
# set_tests_properties(HarmonicB PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# 
# #set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data type hydro name std")
# add_test(NAME RandomHarmonic
#   COMMAND ${CMAKE_COMMAND}
#   -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/RandomHarmonic
#   -D test_args:string=${args}
#   -D out_blessed=${EOSLIB_ODE_BLESSED_OUTPUT}/RandomHarmonicOut.blessed
#   -D err_blessed=${EOSLIB_ODE_BLESSED_OUTPUT}/RandomHarmonicErr.blessed 
#   -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
#   )
# set_tests_properties(RandomHarmonic PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")



if(${verbose_traversal})
  message(STATUS "Exiting ODE/Tests subdirectory")
endif(${verbose_traversal})
