if(${verbose_traversal})
  message(STATUS "Entered EOStypes subdirectory")
endif(${verbose_traversal})

list(APPEND subdirectories_types Davis GenHayes Hayes HayesBM IdealGas JWL MieGruneisen Porous PTequilibrium Sesame StiffenedGas )

foreach( subdirectory ${subdirectories_types})
  add_subdirectory(${subdirectory})
  set( blessed_files )
  file(GLOB blessed_files ./${subdirectory}/*.blessed)
  foreach( blessed_file ${blessed_files} )
    configure_file(${blessed_file} ${CMAKE_CURRENT_BINARY_DIR}/${subdirectory}/ COPYONLY)
  endforeach(blessed_file)
endforeach(subdirectory)



if(${verbose_traversal})
  message(STATUS "Exiting EOStypes subdirectory")
endif(${verbose_traversal})
