if(${verbose_traversal})
  message(STATUS "Entered ExtEOS subdirectory")
endif(${verbose_traversal})

message(STATUS "blessed output in ExtEOS: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_EXTEOS_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/ExtEOS)
set(ExtEOS_src)

add_subdirectory(Subs)

add_library( ExtEOS ${${projectName}_policy} ${ExtEOS_src} )
set_property(TARGET ExtEOS PROPERTY CXX_STANDARD 11)
set_property(TARGET ExtEOS PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ExtEOS PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( ExtEOS Io Math Eos DataBase ODE Calc Manager ${CMAKE_DL_LIBS})

list( APPEND tests TestHydro )

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} ExtEOS)
 # add_test(${test}IdealGas ${test}IdealGas)
 # set(args "file ${EOSLIB_DATA_PATH}/test_data/IdealGasTest.data")
  add_test(NAME ${test}
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_EXTEOS_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_EXTEOS_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test})
  set_tests_properties(${test} PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

list( APPEND tests_with_args TestXmanager )

set(args "file /home/woodscn/git-repos/eoslib/linux-build/DATA/EOS.data lib $EOSLIB_SHARED_LIBRARY_PATH")
foreach(test ${tests_with_args})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} ExtEOS)
  add_test(NAME ${test}
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_EXTEOS_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_EXTEOS_BLESSED_OUTPUT}/${test}Err.blessed
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test})
  set_tests_properties(${test} PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting ExtEOS subdirectory")
endif(${verbose_traversal})

