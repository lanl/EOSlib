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

list( APPEND tests  CJ_plot CJstate CriticalPoint CV-rotation Deflagration Deflagration-0  Detonation DetPolar Dissipation EOSdata HotSpot ImpedanceMatch Isobar Isotherm IsothermVT ShockLocus Polar PVstate TestXisentrope TestXmanager TestXRiemann ThreeStepEvolution ThreeStepReactionTime Wave )
#tests that do not compile - DetonationProfile FFprofile HEburnRate HEburnReactionTime HEburnEvolution HotSpotDeflagration ImpedanceMatch IntegrateDeflagrationODEs PopPlotTrajectory ViscousProfile ZNDprofile ZND-SURFplus ZND-SURFplus-0 
#tests that depend on Sesame - SesameVT 

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

