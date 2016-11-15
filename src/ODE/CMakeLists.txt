if(${verbose_traversal})
  message(STATUS "Entered ODE subdirectory")
endif(${verbose_traversal})

message(STATUS "blessed output in ODE: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_ODE_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/ODE)

set(ODE_src)

add_subdirectory(Subs)

add_library( ODE ${${projectName}_policy} ${ODE_src} )
set_property(TARGET ODE PROPERTY CXX_STANDARD 11)
set_property(TARGET ODE PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ODE PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( ODE Io Math OneDFunction LinkList)

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
#   -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
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
#   -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
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
#   -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
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
#   -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
#   )
# set_tests_properties(RandomHarmonic PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")

install(TARGETS ODE
  RUNTIME DESTINATION ./bin
  LIBRARY DESTINATION ./lib
  ARCHIVE DESTINATION ./lib/static)

list(APPEND INSTALL_TARGETS ODE)
set(INSTALL_TARGETS ${INSTALL_TARGETS} PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting ODE subdirectory")
endif(${verbose_traversal})
