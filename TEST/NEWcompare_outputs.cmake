# Takes (as CMAKE variables) the arguments:
# -   test_name: test name (optional)
# -    test_cmd: command to run test
# -   test_args: args for the test program (optional)
# - blessed_dir: directory with blessed output (Out, Err, EOSlog)
# -     run_dir: directory to run test
#
#
# Note: Err output may contain absolute path
#            error->Log(subroutine,__FILE__, . . . )
#           remove Err file from blessed directory to skip test
#       EOSlog should be optional
#
#       in run directory make subdir for test output
#       if test is successful remove test output directory
#       otherwise output directory helps with debugging
#
if(NOT test_cmd)
  message(FATAL_ERROR "Variable test_cmd not defined")
endif(NOT test_cmd)


## DEBUG
  message(STATUS "test_cmd ${test_cmd}")




#
#  if runEOS UTILITY then test_name not test_cmd
#
if( NOT test_name )
    get_filename_component( test_name ${test_cmd} NAME )
endif( NOT test_name )

if(NOT blessed_dir)
  message(FATAL_ERROR "Variable blessed_dir not defined")
endif(NOT blessed_dir)

if(NOT run_dir)
  message(FATAL_ERROR "Variable run_dir not defined")
endif(NOT run_dir)
#
#  output in subdirectory of run_dir
#
set(RUN_TEST_DIR "${run_dir}/${test_name}")
if( NOT EXISTS ${RUN_TEST_DIR} )
    file(MAKE_DIRECTORY ${RUN_TEST_DIR})
endif(NOT EXISTS ${RUN_TEST_DIR})
#
#  run test
#
separate_arguments(test_args)
message(STATUS "${test_cmd} ${test_args}")

execute_process(
  COMMAND ${test_cmd} ${test_args}
  OUTPUT_FILE Out
  ERROR_FILE  Err
  RESULT_VARIABLE test_failed
  WORKING_DIRECTORY ${RUN_TEST_DIR}
  )

if(test_failed)
  message(SEND_ERROR "${test_name} failed to run!")
  return(test_failed)
endif(test_failed)
#
# check output files
#
file(GLOB output  RELATIVE "${blessed_dir}" "${blessed_dir}/*")
# exclude vi files '*~', previous saved file
file(GLOB output1  RELATIVE "${blessed_dir}" "${blessed_dir}/*~")
if( output1 )
   list(REMOVE_ITEM output ${output1})
endif( output1 )
# exclude vi files '.*', current edit buffer
file(GLOB output1  RELATIVE "${blessed_dir}" "${blessed_dir}/.*")
if( output1 )
   list(REMOVE_ITEM output ${output1})
endif( output1 )

foreach( file ${output} )
   set(OUT_BLESSED "${blessed_dir}/${file}")
   set(OUT_TEST "${RUN_TEST_DIR}/${file}")
   execute_process(
      COMMAND ${CMAKE_COMMAND} -E compare_files ${OUT_BLESSED} ${OUT_TEST}
      RESULT_VARIABLE test_failed
   )
   if(test_failed)
     message(SEND_ERROR "'${output}' files do not match")
     return(test_failed)
   endif(test_failed)
endforeach( file )
#
# test sucessful
#
file(REMOVE_RECURSE ${RUN_TEST_DIR})
return(test_failed)
