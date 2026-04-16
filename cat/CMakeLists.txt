#
# SPDX-License-Identifier: BSD-2-Clause
#
############################################
#
# How to build bsdcat
#
############################################
IF(ENABLE_CAT)

  SET(bsdcat_SOURCES
    bsdcat.c
    bsdcat.h
    bsdcat_platform.h
    cmdline.c
    ../libarchive_fe/lafe_err.c
    ../libarchive_fe/lafe_err.h
    ../libarchive_fe/lafe_platform.h
  )
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../libarchive_fe)

  # bsdcat documentation
  SET(bsdcat_MANS bsdcat.1)

  # How to build bsdcat
  ADD_EXECUTABLE(bsdcat ${bsdcat_SOURCES})
  TARGET_LINK_LIBRARIES(bsdcat LibArchive::LibArchive ${ADDITIONAL_LIBS})

  # Installation rules
  IF(ENABLE_INSTALL)
    INSTALL(TARGETS bsdcat RUNTIME DESTINATION bin)
    INSTALL_MAN(${bsdcat_MANS})
  ENDIF(ENABLE_INSTALL)
ENDIF(ENABLE_CAT)

add_subdirectory(test)
