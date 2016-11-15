if(${verbose_traversal})
  message(STATUS "Entered ArrheniusHE subdirectory")
endif(${verbose_traversal})

message(STATUS "blessed output in ArrheniusHE: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_ARRHENIUSHE_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/ExtEOStypes/ArrheniusHE)

set(ArrheniusHE_src)

add_subdirectory(Subs)

add_library( ArrheniusHE ${${projectName}_policy} ${ArrheniusHE_src} )
set_property(TARGET ArrheniusHE PROPERTY CXX_STANDARD 11)
set_property(TARGET ArrheniusHE PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ArrheniusHE PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( ArrheniusHE Io Math ODE DataBase Eos ExtEOS Materials ${CMAKE_DL_LIBS} )

# #tests excluded for Sesame dependency - Hug 
# list( APPEND tests AvStiffIdealOverdriven CJprofile IdealOverdriven Match Overdriven TestArrhenius TestCopy TestIdealHE TestIdealHE_Def TestIdealHE_Det TestIdealHug TestIntegrate TestIntegrateStiffIdeal TestProfile TestRate TestRiemann TestStiffIdealHEav TestStiffIdealHE )#Tinduction )
# #ZNDprofile - ZNDprofile is designed to take file, material, etc as input, not be hardcoded to a test.data file
# 
# foreach(test ${tests})
#   add_executable(${test}ArrheniusHE ${test}.C)
#   target_link_libraries(${test}ArrheniusHE ArrheniusHE)
#   add_test(NAME ${test}ArrheniusHE
#     COMMAND ${CMAKE_COMMAND}
#     -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}ArrheniusHE
#     -D test_args:string=${args}
#     -D out_blessed=${EOSLIB_ARRHENIUSHE_BLESSED_OUTPUT}/${test}Out.blessed
#     -D err_blessed=${EOSLIB_ARRHENIUSHE_BLESSED_OUTPUT}/${test}Err.blessed 
#     -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
#     )
#   message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}ArrheniusHE)
#   set_tests_properties(${test}ArrheniusHE PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# endforeach(test)

install(TARGETS ArrheniusHE
  RUNTIME DESTINATION ./bin
  LIBRARY DESTINATION ./lib
  ARCHIVE DESTINATION ./lib/static)

list(APPEND INSTALL_TARGETS ArrheniusHE)
set(INSTALL_TARGETS ${INSTALL_TARGETS} PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting ArrheniusHE subdirectory")
endif(${verbose_traversal})
