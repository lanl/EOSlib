if(${verbose_traversal})
  message(STATUS "Entered DataBase/Tests subdirectory")
endif(${verbose_traversal})
#
# test of dynamic linking
#
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
#
#
file(GLOB Test_c  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Test*.C")
foreach( test_c ${Test_c} )
  get_filename_component( test ${test_c} NAME_WE )
  add_executable(${test} ${test_c})
  target_link_libraries( ${test} DataBase )
endforeach( test_c )
#
#
add_library( shared_obj ${${projectName}_policy} shared_obj.C)
target_link_libraries( shared_obj Math )
#
# sym link to blessed_output directory
execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                         ${CMAKE_CURRENT_SOURCE_DIR}/blessed_output
                         ${CMAKE_CURRENT_BINARY_DIR}/blessed_output)

# sym link to DATA directory
execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                         ${CMAKE_CURRENT_SOURCE_DIR}/DATA
                         ${CMAKE_CURRENT_BINARY_DIR}/DATA)


file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Testing)
file(GLOB DIR  RELATIVE "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

set(BLESSED_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/blessed_output)
set(DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/DATA)
#
# To Do: add tests of TestDB Testdl TestEnvironmentVar
#

#
#  test utilities (Data, Convert) in ..
#
set( bin "${CMAKE_BINARY_DIR}/bin" )
set(FILE "file ${DATA_DIR}/DataBaseTest.data")
#
#  test of ../Convert
#
set( tname Convert )
set( Convert_arg "${FILE} type hydro from std to HiP e u u^2 P V P*V" )
  add_test(NAME "${DIR}/${tname}" 
    COMMAND ${CMAKE_COMMAND}
      -D test_name=${tname}
      -D test_cmd=${bin}/Convert
      -D test_args:string=${${tname}_arg}
      -D blessed_dir=${BLESSED_OUTPUT_DIR}/${tname}
      -D run_dir=${CMAKE_CURRENT_BINARY_DIR}/Testing
      -P ${CMAKE_BINARY_DIR}/TEST/NEWcompare_outputs.cmake
    )
    set_tests_properties("${DIR}/${tname}" PROPERTIES ENVIRONMENT
                         "${CTEST_ENVIRONMENT}")
#
#  tests of ../Data
#
list( APPEND tlist Data0  Data1  Data2  Data4 Data6 Data7 Data8 Data9 )
list( APPEND tlist Data10 Data11 Data12 )

set(Data0_arg  "${FILE} base Units type hydro name std use")
set(Data1_arg  "${FILE}")
set(Data2_arg  "${FILE} base EOS")
set(Data4_arg  "${FILE} type SharedLibs parameters")
set(Data6_arg  "${FILE} type UnitsNames name std parameters")
set(Data7_arg  "${FILE} type UnitsNames name std use")
set(Data8_arg  "${FILE} base Units type hydro name std use")
set(Data9_arg  "${FILE} base EOS type Hayes name HMX")
set(Data10_arg "${FILE} base Units type hydro name std parameters")
set(Data11_arg "${FILE} base Units type hydro name std use")
set(Data12_arg "${FILE} base EOS type Hayes name HMX-YC parameters")

foreach( tname ${tlist} )
  add_test(NAME "${DIR}/${tname}" 
    COMMAND ${CMAKE_COMMAND}
      -D test_name=${tname}
      -D test_cmd=${bin}/Data
      -D test_args:string=${${tname}_arg}
      -D blessed_dir=${BLESSED_OUTPUT_DIR}/${tname}
      -D run_dir=${CMAKE_CURRENT_BINARY_DIR}/Testing
      -P ${CMAKE_BINARY_DIR}/TEST/NEWcompare_outputs.cmake
    )
    set_tests_properties("${DIR}/${tname}" PROPERTIES ENVIRONMENT
                         "${CTEST_ENVIRONMENT}")
endforeach( tname )

if(${verbose_traversal})
  message(STATUS "Exiting DataBase/Tests subdirectory")
endif(${verbose_traversal})
