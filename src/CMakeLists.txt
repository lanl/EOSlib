if(${verbose_traversal})
  message(STATUS "Entered src subdirectory")
endif(${verbose_traversal})

#
add_subdirectory(CClib)
#
#
add_subdirectory(DataBase)
#
#
add_subdirectory(EOS)
add_subdirectory(EOStypes)

add_subdirectory(ExtEOS)
add_subdirectory(ExtEOStypes)

add_subdirectory(Applications)

add_subdirectory(Manager)

add_subdirectory(DnKappa)


set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

if(${verbose_traversal})
  message(STATUS "Exiting src subdirectory")
endif(${verbose_traversal})
