if(${verbose_traversal})
  message(STATUS "Entered blessed_output subdirectory")
endif(${verbose_traversal})

set(EOSLIB_BLESSED_OUTPUT ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

list(APPEND subdirectories Applications Calc DataBase
  DnKappa EOStypes Eos ExtEOS ExtEOStypes LinkList Local
  OneDFunction Spline)

foreach( subdirectory ${subdirectories})
  set( blessed_files )
  file(GLOB blessed_files ./${subdirectory}/*.blessed)
  foreach( blessed_file ${blessed_files} )
    configure_file(${blessed_file} ${CMAKE_CURRENT_BINARY_DIR}/${subdirectory} COPYONLY)
  endforeach(blessed_file)
endforeach(subdirectory)



if(${verbose_traversal})
  message(STATUS "Exiting blessed_output subdirectory")
endif(${verbose_traversal})
