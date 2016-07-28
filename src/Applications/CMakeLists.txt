if(${verbose_traversal})
  message(STATUS "Entered Applications subdirectory")
endif(${verbose_traversal})

message(STATUS "blessed output in Applications: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_APPLICATIONS_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/Applications)
set(Applications_src dummy.cpp )

list( APPEND dirs Local ExtEOS Eos DataBase LinkList Calc OneDFunction ODE ExtEOStypes/HEburn EOStypes/PTequilibrium EOStypes/Porous)
foreach(dir ${dirs})
  include_directories(${CMAKE_SOURCE_DIR}/src/${dir})
endforeach(dir)

add_subdirectory(LocalEOSdata)

add_library( Applications ${${projectName}_policy} ${Applications_src} )
set_property(TARGET Applications PROPERTY CXX_STANDARD 11)
set_property(TARGET Applications PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Applications PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( Applications Eos DataBase ODE Materials Calc Io Math String ExtEOS Manager ${CMAKE_DL_LIBS})

#tests in this list have a hardcoded input file
list( APPEND tests  CJ_plot Deflagration Polar PVstate TestXRiemann )
#the tests not listed do not have hardcoded input files
#####################################################
#tests that depend on Sesame - TestXisentrope TestXmanager
####################################################
#####problems to fix, if these files need to be included##########
#FFprofile.C:81: undefined reference to `typeinfo for HEburn'
#FFprofile.C:84: undefined reference to `typeinfo for FFrate'
#FFprofile.C:84: undefined reference to `typeinfo for HErate'
#FFprofile.C:87: undefined reference to `ViscousProfile::ViscousProfile(FFrate*, double, double)'
#HotSpotDeflagration.C:55: undefined reference to `typeinfo for HEburn'
#ImpedanceMatch.C:173: undefined reference to `typeinfo for EqPorous'
#ZND-SURFplus.C:68: undefined reference to `typeinfo for HotSpotRateCC'
###################################################################

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} Applications)
  add_test(NAME ${test}
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_APPLICATIONS_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_APPLICATIONS_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS Applications ${CMAKE_CURRENT_BINARY_DIR}/${test})
  set_tests_properties(${test} PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting Applications subdirectory")
endif(${verbose_traversal})

