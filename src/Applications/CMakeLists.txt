if(${verbose_traversal})
  message(STATUS "Entered Applications subdirectory")
endif(${verbose_traversal})


execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                                ${CMAKE_CURRENT_SOURCE_DIR}
                                ${CMAKE_CURRENT_BINARY_DIR}/SOURCE)

set(EOSLIB_APPLICATIONS_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/Applications)

list( APPEND dirs Local ExtEOS Eos DataBase LinkList Calc OneDFunction ODE )
list( APPEND dirs ExtEOStypes/HEburn EOStypes/PTequilibrium EOStypes/Porous)
foreach(dir ${dirs})
  include_directories(${CMAKE_SOURCE_DIR}/src/${dir})
endforeach(dir)

#add_subdirectory(LocalEOSdata)

add_library( Applications ${${projectName}_policy} "" )
set_property(TARGET Applications PROPERTY CXX_STANDARD 11)
set_property(TARGET Applications PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Applications PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
set_property(TARGET Applications PROPERTY LINKER_LANGUAGE CXX)

target_link_libraries( Applications Eos DataBase ODE Materials Calc Io )
target_link_libraries( Applications Math String ExtEOS Manager HEburn  )
target_link_libraries( Applications ${CMAKE_DL_LIBS})

list(APPEND utils EOSdata Wave Detonation Isotherm ImpedanceMatch)
list(APPEND utils Polar Deflagration DetPolar)
list(APPEND utils ZND-SURFplus HEburnReactionTime)
list(APPEND utils TestXmanager TestXRiemann TestXisentrope)
#list(APPEND utils  CJ_plot CJstate)

 foreach(util ${utils})
   add_executable(${util} ${util}.C)
   target_link_libraries(${util} Applications)
#   add_util(NAME ${util}
#     COMMAND ${CMAKE_COMMAND}
#     -D util_cmd=${CMAKE_CURRENT_BINARY_DIR}/${util}
#     -D util_args:string=${args}
#     -D out_blessed=${EOSLIB_APPLICATIONS_BLESSED_OUTPUT}/${util}Out.blessed
#     -D err_blessed=${EOSLIB_APPLICATIONS_BLESSED_OUTPUT}/${util}Err.blessed 
#     -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
#     )
#   message(STATUS Applications ${CMAKE_CURRENT_BINARY_DIR}/${util})
#   set_utils_properties(${util} PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
 endforeach(util)

if(${verbose_traversal})
  message(STATUS "Exiting Applications subdirectory")
endif(${verbose_traversal})

