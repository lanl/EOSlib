if(${verbose_traversal})
  message(STATUS "Entered Applications subdirectory")
endif(${verbose_traversal})

message(STATUS "Applications")
#
# headers/SOURCE
#
execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                                ${CMAKE_CURRENT_SOURCE_DIR}
                                ${CMAKE_CURRENT_BINARY_DIR}/SOURCE)
#
# link libraries
#
add_library( Applications ${${projectName}_policy} "" )
set_property(TARGET Applications PROPERTY CXX_STANDARD 11)
set_property(TARGET Applications PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Applications PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
set_property(TARGET Applications PROPERTY LINKER_LANGUAGE CXX)

target_link_libraries( Applications ExtEOS HEburn Materials )
#
# Utilities
#
list(APPEND utils EOSdata Wave Detonation Isotherm ImpedanceMatch)
list(APPEND utils Polar Deflagration DetPolar)
list(APPEND utils ZND-SURFplus ZND-SURFplus-dx HEburnReactionTime)

foreach(util ${utils})
   add_executable(${util} ${util}.C)
   target_link_libraries(${util} Applications)
endforeach(util)
#
# tests in build/TEST using runEOS
#
set(BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/Applications)

#   add_util(NAME ${util}
#     COMMAND ${CMAKE_COMMAND}
#     -D util_cmd=${CMAKE_CURRENT_BINARY_DIR}/${util}
#     -D util_args:string=${args}
#     -D out_blessed=${BLESSED_OUTPUT}/${util}Out.blessed
#     -D err_blessed=${BLESSED_OUTPUT}/${util}Err.blessed 
#     -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
#     )
#   message(STATUS Applications ${CMAKE_CURRENT_BINARY_DIR}/${util})
#   set_utils_properties(${util} PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")


if(${verbose_traversal})
  message(STATUS "Exiting Applications subdirectory")
endif(${verbose_traversal})
