if(${verbose_traversal})
  message(STATUS "Entered ExtEOStypes subdirectory")
endif(${verbose_traversal})

#message(STATUS "blessed output in ExtEOStypes: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_EXTEOSTYPES_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/ExtEOStypes)
set(ExtEOStypes_src dummy.cpp)

list(APPEND ExtEOStypes_libs ArrheniusHE Elastic1D ElasticPlastic )
#HEburn ) - excluded ->contains numerous Sesame references
foreach(ExtEOStype ${ExtEOStypes_libs})
  add_subdirectory(${ExtEOStype})
endforeach(ExtEOStype)
#add_subdirectory(ArrheniusHE)
#add_subdirectory(Elastic1D)
#add_subdirectory(ElasticPlastic)
#add_subdirectory(HEburn)


add_library( ExtEOStypes ${${projectName}_policy} ${ExtEOStypes_src} )
set_property(TARGET ExtEOStypes PROPERTY CXX_STANDARD 11)
set_property(TARGET ExtEOStypes PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ExtEOStypes PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
target_link_libraries( ExtEOStypes ODE Manager Materials Eos DataBase ExtEOS ${ExtEOStypes_libs} )

list( APPEND tests TestHE )

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} ExtEOStypes)
  add_test(NAME ${test}
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_EXTEOSTYPES_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_EXTEOSTYPES_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test})
  set_tests_properties(${test} PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting ExtEOStypes subdirectory")
endif(${verbose_traversal})
