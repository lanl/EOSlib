if(${verbose_traversal})
  message(STATUS "Entered TEST subdirectory")
endif(${verbose_traversal})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/compare_outputs.cmake
          DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Note: do not need to copy blessed_output
#       if set EOSLIB_BLESSED_OUTPUT uses
#       ${CMAKE_CURRENT_SOURCE_DIR} rather than ${CMAKE_CURRENT_BINARY_DIR}
#add_subdirectory(blessed_output)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/blessed_output
          DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(EOSLIB_BLESSED_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/blessed_output)
set(EOSLIB_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT} PARENT_SCOPE)


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
  TEST_BASH(runEOSWave       "${runEOS} Wave       material HEburn::PBX9502.davis Ps 10")
  TEST_BASH(runEOSDetonation "${runEOS} Detonation material HEburn::PBX9502.davis P2 10")
  TEST_BASH(runEOSIsotherm   "${runEOS} Isotherm   material HEburn::PBX9502.davis Ps 10 P2 1")
  TEST_BASH(runEOSEOSdata    "${runEOS} EOSdata    material HEburn::PBX9502.davis all")
  TEST_BASH(runEOSData       "${runEOS} Data base EOS type HEburn name PBX9502.davis parameters")
  TEST_BASH(runEOSImpedanceMatch "${runEOS} ImpedanceMatch mat1 Hayes::Kel-F u1 2.7 mat2 HEburn::PBX9502.davis")
endif(BASH_PROGRAM)

if(${verbose_traversal})
  message(STATUS "Exiting TEST subdirectory")
endif(${verbose_traversal})
