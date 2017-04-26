# Takes (as CMAKE variables) the arguments:
# - test_cmd: test program
# - test_args: args for the test program
# - out_blessed: known "good" test output
# - err_blessed: known "good" test err
#
# ToDo: replace arguments out_blessed and err_blessed
#       with blessed directory containing files
#            Out, Err and EOSlog
#       Err may contain absolute path
#            error->Log(subroutine,__FILE__, . . . )
#           remove Err file from blessed directory to skip test
#       EOSlog should be optional
# ToDo: add argument for run directory
#         system tests in build/TEST
#         unit tests in local Tests subdirectory
#       in run directory make subdir for test output
#       if test is successful remove test output directory
#       otherwise output directory helps with debugging
#
if(NOT test_cmd)
  message(FATAL_ERROR "Variable test_cmd not defined")
endif(NOT test_cmd)

if(NOT out_blessed)
  message(FATAL_ERROR "Variable out_blessed not defined")
endif(NOT out_blessed)

if(NOT err_blessed)
  message(FATAL_ERROR "Variable err_blessed not defined")
endif(NOT err_blessed)

set(OUT_TEST out.test)
set(ERR_TEST err.test)

separate_arguments(test_args)
message(STATUS ${test_args})

# directory Testing created by ctest
# ToDo: mirror subdir structure in blessed_output directory
set(CWD ${CMAKE_CURRENT_LIST_DIR}/../Testing)

execute_process(
  COMMAND ${test_cmd} ${test_args}
  OUTPUT_FILE ${OUT_TEST}
  ERROR_FILE ${ERR_TEST}
  RESULT_VARIABLE test_failed_to_run
  WORKING_DIRECTORY ${CWD}
  )

if(test_failed_to_run)
  message(SEND_ERROR "${test_cmd} failed to run successfully!")
  return(test_failed_to_run)
endif(test_failed_to_run)

set(OUT_TEST ${CWD}/out.test)
if( EXISTS "${out_blessed}" )
  execute_process(
     COMMAND ${CMAKE_COMMAND} -E compare_files ${out_blessed} ${OUT_TEST}
     RESULT_VARIABLE test_not_successful
  )
  if(test_not_successful)
    message(SEND_ERROR "${OUT_TEST} does not match ${out_blessed}")
    return(test_failed_to_run)
  endif(test_not_successful)
endif( EXISTS "${out_blessed}" )

set(ERR_TEST ${CWD}/err.test)
if( EXISTS "${err_blessed}" )
  execute_process(
     COMMAND ${CMAKE_COMMAND} -E compare_files ${err_blessed} ${ERR_TEST}
     RESULT_VARIABLE test_not_successful
  )
  if(test_not_successful)
    message(SEND_ERROR "${ERR_TEST} does not match ${err_blessed}")
    return(test_failed_to_run)
  endif(test_not_successful)
endif( EXISTS "${err_blessed}" )

#set(EOSlog ${CWD}/EOSlog)
#set(${log_blessed} "??")
#if( EXISTS "${EOSlog}" )
#  execute_process(
#     COMMAND ${CMAKE_COMMAND} -E compare_files ${log_blessed} ${EOSlog}
#     RESULT_VARIABLE test_not_successful
#  )
#  if(test_not_successful)
#    message(SEND_ERROR "${EOSlog} does not match ${log_blessed}")
#    return(test_failed_to_run)
#  endif(test_not_successful)
#endif( EXISTS "${EOSlog}" )


#file(REMOVE ${OUT_TEST} ${ERR_TEST})


return(test_not_successful)
