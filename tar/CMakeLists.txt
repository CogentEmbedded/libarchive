############################################
#
# How to build bsdtar
#
############################################
IF(ENABLE_TAR)

  SET(bsdtar_SOURCES
    bsdtar.c
    bsdtar.h
    bsdtar_platform.h
    cmdline.c
    creation_set.c
    read.c
    subst.c
    util.c
    write.c
    ../libarchive_fe/err.c
    ../libarchive_fe/err.h
    ../libarchive_fe/lafe_platform.h
    ../libarchive_fe/line_reader.c
    ../libarchive_fe/line_reader.h
    ../libarchive_fe/passphrase.c
    ../libarchive_fe/passphrase.h
  )
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../libarchive_fe)
  IF(WIN32 AND NOT CYGWIN)
    LIST(APPEND bsdtar_SOURCES bsdtar_windows.c)
    LIST(APPEND bsdtar_SOURCES bsdtar_windows.h)
  ENDIF(WIN32 AND NOT CYGWIN)

  # bsdtar documentation
  SET(bsdtar_MANS bsdtar.1)

  # How to build bsdtar
  ADD_EXECUTABLE(bsdtar ${bsdtar_SOURCES})
  TARGET_LINK_LIBRARIES(bsdtar archive LibArchive::LibArchive)

  # Installation rules
  INSTALL(TARGETS bsdtar RUNTIME DESTINATION bin)
  INSTALL_MAN(${bsdtar_MANS})
ENDIF(ENABLE_TAR)

add_subdirectory(test)
