if(${verbose_traversal})
  message(STATUS "Entered ExtEOStypes subdirectory")
endif(${verbose_traversal})

message(STATUS "ExtEOStypes\n")

list(APPEND ExtEOStypes_libs ArrheniusHE Elastic1D ElasticPlastic HEburn )
foreach(ExtEOStype ${ExtEOStypes_libs})
  add_subdirectory(${ExtEOStype})
endforeach(ExtEOStype)


add_library( ExtEOStypes ${${projectName}_policy} "" )
set_property(TARGET ExtEOStypes PROPERTY CXX_STANDARD 11)
set_property(TARGET ExtEOStypes PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ExtEOStypes PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
set_property(TARGET ExtEOStypes PROPERTY LINKER_LANGUAGE CXX)
target_link_libraries( ExtEOStypes ${ExtEOStypes_libs} )

add_library( ExtHE ${${projectName}_policy} "" )
set_property(TARGET ExtHE PROPERTY CXX_STANDARD 11)
set_property(TARGET ExtHE PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ExtHE PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
set_property(TARGET ExtHE PROPERTY LINKER_LANGUAGE CXX)
target_link_libraries( ExtHE ExtEOStypes ArrheniusHE HEburn PTequilibrium )

set(BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/ExtEOStypes)

# list( APPEND tests TestHE )
# 
# foreach(test ${tests})
#   add_executable(${test} ${test}.C)
#   target_link_libraries(${test} ExtEOStypes)
#   add_test(NAME ${test}
#     COMMAND ${CMAKE_COMMAND}
#     -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}
#     -D test_args:string=${args}
#     -D out_blessed=${BLESSED_OUTPUT}/${test}Out.blessed
#     -D err_blessed=${BLESSED_OUTPUT}/${test}Err.blessed 
#     -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
#     )
#   message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test})
#   set_tests_properties(${test} PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# endforeach(test)


if(${verbose_traversal})
  message(STATUS "Exiting ExtEOStypes subdirectory")
endif(${verbose_traversal})
