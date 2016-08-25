if(${verbose_traversal})
  message(STATUS "Entered HEburn subdirectory")
endif(${verbose_traversal})

message(STATUS "blessed output in HEburn: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_HEBURN_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/ExtEOStypes/HEburn)

set(HEburn_src)

add_subdirectory(Subs)

add_library( HEburn ${${projectName}_policy} ${HEburn_src} )
set_property(TARGET HEburn PROPERTY CXX_STANDARD 11)
set_property(TARGET HEburn PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET HEburn PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( HEburn Io Math Eos ExtEOS ODE Materials ${CMAKE_DL_LIBS})

list( APPEND tests CJtest FFprofile HE_Cv HE_Gamma PopPlotTrajectory ReactionTime TestAllocation TestHEburn2CC TestHEburn TestHEburnCC TestHEeos TestHErate TestIntegrate TestTimeStep ViscousProfile ZNDprofile)

foreach(test ${tests})
  add_executable(${test}HEburn ${test}.C)
  target_link_libraries(${test}HEburn HEburn)
  # add_test(NAME ${test}HEburn
  #   COMMAND ${CMAKE_COMMAND}
  #   -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}HEburn
  #   -D test_args:string=${args}
  #   -D out_blessed=${EOSLIB_HEBURN_BLESSED_OUTPUT}/${test}Out.blessed
  #   -D err_blessed=${EOSLIB_HEBURN_BLESSED_OUTPUT}/${test}Err.blessed 
  #   -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  #   )
  # message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}HEburn)
  # set_tests_properties(${test}HEburn PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

install(TARGETS HEburn
  RUNTIME DESTINATION ./bin
  LIBRARY DESTINATION ./lib
  ARCHIVE DESTINATION ./lib/static)

list(APPEND INSTALL_TARGETS HEburn)
set(INSTALL_TARGETS ${INSTALL_TARGETS} PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting HEburn subdirectory")
endif(${verbose_traversal})
