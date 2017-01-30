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
#
# old version, replace with new version then delete
#
function(TEST_BASH test_name test_args)
  add_test(NAME ${test_name} 
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${BASH_PROGRAM}
    -D test_args:string=${test_args}
    -D out_blessed=${EOSLIB_BLESSED_OUTPUT}/${test_name}Out.blessed
    -D err_blessed=${EOSLIB_BLESSED_OUTPUT}/${test_name}Err.blessed
    -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
    )
endfunction(TEST_BASH)

if(BASH_PROGRAM)
  TEST_BASH(runEOShelp       "${runEOS} help")
  TEST_BASH(runEOSWavehelp   "${runEOS} Wave help")
  #TEST_BASH(runEOSWave       "${runEOS} Wave       material HEburn::PBX9502.davis Ps 10")
  TEST_BASH(runEOSDetonation "${runEOS} Detonation material HEburn::PBX9502.davis P2 10")
  TEST_BASH(runEOSIsotherm   "${runEOS} Isotherm   material HEburn::PBX9502.davis Ps 10 P2 1")
  TEST_BASH(runEOSEOSdata    "${runEOS} EOSdata    material HEburn::PBX9502.davis all")
  TEST_BASH(runEOSData       "${runEOS} Data base EOS type HEburn name PBX9502.davis parameters")
  TEST_BASH(runEOSImpedanceMatch "${runEOS} ImpedanceMatch mat1 Hayes::Kel-F u1 2.7 mat2 HEburn::PBX9502.davis")
endif(BASH_PROGRAM)
#
# new version
#
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Testing)
file(GLOB DIR  RELATIVE "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

set(BLESSED_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/blessed_output)
set(runEOSWave_arg "Wave material HEburn::PBX9502.davis Ps 10")

  add_test(NAME "${DIR}/runEOSWave" 
    COMMAND ${CMAKE_COMMAND}
    -D test_name=runEOSWave
    -D test_cmd=${runEOS}
    -D test_args:string=${runEOSWave_arg}
    -D blessed_dir=${BLESSED_OUTPUT_DIR}/runEOSWave
    -D run_dir=${CMAKE_CURRENT_BINARY_DIR}/Testing
    -P ${CMAKE_BINARY_DIR}/TEST/NEWcompare_outputs.cmake
    )


if(${verbose_traversal})
  message(STATUS "Exiting TEST subdirectory")
endif(${verbose_traversal})
