if(${verbose_traversal})
  message(STATUS "Entered TEST subdirectory")
endif(${verbose_traversal})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/compare_outputs.cmake
          DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/NEWcompare_outputs.cmake
          DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# sym link to blessed_output directory
execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                         ${CMAKE_CURRENT_SOURCE_DIR}/blessed_output
                         ${CMAKE_CURRENT_BINARY_DIR}/blessed_output)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Testing)
file(GLOB DIR  RELATIVE "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

set(BLESSED_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/blessed_output)
#
#  tests
#
list( APPEND tlist runEOShelp     runEOSWavehelp runEOSWave runEOSDetonation
                   runEOSIsotherm runEOSEOSdata  runEOSData
                   runEOSImpedanceMatch )

set(runEOShelp_arg       "help")
set(runEOSWavehelp_arg   "Wave help")
set(runEOSWave_arg       "Wave material HEburn::PBX9502.davis Ps 10")
set(runEOSDetonation_arg "Detonation material HEburn::PBX9502.davis P2 10")
set(runEOSIsotherm_arg   "Isotherm   material HEburn::PBX9502.davis Ps 10 P2 1")
set(runEOSEOSdata_arg    "EOSdata    material HEburn::PBX9502.davis all")
set(runEOSData_arg       "Data base EOS type HEburn name PBX9502.davis parameters")
set(runEOSImpedanceMatch_arg "ImpedanceMatch mat1 Hayes::Kel-F u1 2.7 mat2 HEburn::PBX9502.davis")


foreach( tname ${tlist} )
  add_test(NAME "${DIR}/${tname}" 
    COMMAND ${CMAKE_COMMAND}
      -D test_name=${tname}
      -D test_cmd=${runEOS}
      -D test_args:string=${${tname}_arg}
      -D blessed_dir=${BLESSED_OUTPUT_DIR}/${tname}
      -D run_dir=${CMAKE_CURRENT_BINARY_DIR}/Testing
      -P ${CMAKE_BINARY_DIR}/TEST/NEWcompare_outputs.cmake
    )
endforeach( tname )

if(${verbose_traversal})
  message(STATUS "Exiting TEST subdirectory")
endif(${verbose_traversal})
