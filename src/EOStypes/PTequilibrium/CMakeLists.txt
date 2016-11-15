if(${verbose_traversal})
  message(STATUS "Entered PTequilibrium subdirectory")
endif(${verbose_traversal})

set(EOSLIB_PTEQUILIBRIUM_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/EOStypes/PTequilibrium)
set(PTequilibrium_src)

add_subdirectory(Subs)

add_library( PTequilibrium ${${projectName}_policy} ${PTequilibrium_src} )
set_property(TARGET PTequilibrium PROPERTY CXX_STANDARD 11)
set_property(TARGET PTequilibrium PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET PTequilibrium PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( PTequilibrium Eos DataBase ODE Calc ${CMAKE_DL_LIBS} )

# list( APPEND tests  )
# list( APPEND testsWithArgs Test2 Test2VT Test3 Test3VT TestHugoniot TestIsentrope TestStiffIdeal ) 
# 
# foreach(test ${tests})
#   add_executable(${test}PTequilibrium ${test}.C)
#   target_link_libraries(${test}PTequilibrium PTequilibrium)
#  # add_test(${test}IdealGas ${test}IdealGas)
#  # set(args "file ${EOSLIB_DATA_PATH}/test_data/IdealGasTest.data")
#   add_test(NAME ${test}PTequilibrium
#     COMMAND ${CMAKE_COMMAND}
#     -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}PTequilibrium
#     -D test_args:string=${args}
#     -D out_blessed=${EOSLIB_PTEQUILIBRIUM_BLESSED_OUTPUT}/${test}Out.blessed
#     -D err_blessed=${EOSLIB_PTEQUILIBRIUM_BLESSED_OUTPUT}/${test}Err.blessed 
#     -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
#     )
#   message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}PTequilibrium)
#   set_tests_properties(${test}PTequilibrium PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# endforeach(test)
# 
# #### use this if test(s) require file specification####
# 
# foreach(test ${testsWithArgs} )
#   add_executable(${test}PTequilibrium ${test}.C)
#   target_link_libraries(${test}PTequilibrium PTequilibrium )
#   set(args "file ${EOSLIB_DATA_PATH}/test_data/PTequilibriumTest.data")
#   add_test(NAME ${test}PTequilibrium
#     COMMAND ${CMAKE_COMMAND}
#     -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}PTequilibrium
#     -D test_args:string=${args}
#     -D out_blessed=${EOSLIB_PTEQUILIBRIUM_BLESSED_OUTPUT}/${test}Out.blessed
#     -D err_blessed=${EOSLIB_PTEQUILIBRIUM_BLESSED_OUTPUT}/${test}Err.blessed 
#     -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
#     )
#   message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}PTequilibrium)
#   set_tests_properties(${test}PTequilibrium PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# endforeach(test)

install(TARGETS PTequilibrium
  RUNTIME DESTINATION ./bin
  LIBRARY DESTINATION ./lib
  ARCHIVE DESTINATION ./lib/static)

list(APPEND INSTALL_TARGETS PTequilibrium)
set(INSTALL_TARGETS ${INSTALL_TARGETS} PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting PTequilibrium subdirectory")
endif(${verbose_traversal})

