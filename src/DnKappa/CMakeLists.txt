if(${verbose_traversal})
  message(STATUS "Entered DnKappa subdirectory")
endif(${verbose_traversal})

message(STATUS "blessed output in DnKappa: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_DNKAPPA_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/DnKappa)

set(DnKappa_src)

add_subdirectory(Subs)

add_library( DnKappa ${${projectName}_policy} ${DnKappa_src} )
set_property(TARGET DnKappa PROPERTY CXX_STANDARD 11)
set_property(TARGET DnKappa PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET DnKappa PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( DnKappa Io Math )

list( APPEND tests DnKappa TestDnKappa )

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} DnKappa)
 # add_test(${test}IdealGas ${test}IdealGas)
 # set(args "file ${EOSLIB_DATA_PATH}/test_data/IdealGasTest.data")
  add_test(NAME ${test}
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_DNKAPPA_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_DNKAPPA_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test})
  set_tests_properties(${test} PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting DnKappa subdirectory")
endif(${verbose_traversal})
