if(${verbose_traversal})
  message(STATUS "Entered ExtEOStypes subdirectory")
endif(${verbose_traversal})

message(STATUS "ExtEOStypes\n")

list(APPEND ExtEOStypes_libs ArrheniusHE Elastic1D ElasticPlastic HEburn )

set( tmpfile "${CMAKE_CURRENT_BINARY_DIR}/ExtEOStypes.cmake" )
file(WRITE ${tmpfile} "add_library( ExtEOStypes ${${projectName}_policy}\n")
foreach(ExtEOStype ${ExtEOStypes_libs})
  add_subdirectory(${ExtEOStype})
  file(APPEND ${tmpfile} "    $<TARGET_OBJECTS:${ExtEOStype}-obj>\n")
endforeach(ExtEOStype)

file(APPEND ${tmpfile}
  "    )\n"
  "  set_property(TARGET ExtEOStypes PROPERTY CXX_STANDARD_REQUIRED ON)\n"
  "  set_property(TARGET ExtEOStypes PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)\n"
  "  set_property(TARGET ExtEOStypes PROPERTY LINKER_LANGUAGE CXX)\n"
  )
include(${tmpfile})

add_library( ExtHE ${${projectName}_policy}
                   $<TARGET_OBJECTS:ArrheniusHE-obj>
                   $<TARGET_OBJECTS:HEburn-obj> )
set_property(TARGET ExtHE PROPERTY CXX_STANDARD 11)
set_property(TARGET ExtHE PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ExtHE PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
set_property(TARGET ExtHE PROPERTY LINKER_LANGUAGE CXX)
target_link_libraries( ExtHE ExtEOS PTequilibrium )

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
