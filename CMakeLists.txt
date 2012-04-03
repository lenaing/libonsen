#
# Copyright 2011, 2012 - Etienne 'lenaing' GIRONDEL <lenaing@gmail.com>
# 
# libonsen
# --------
# This library is a collection of resources whose purpose is to offer an easy
# to use framework to open and manipulate (mostly games) data archives through
# an extensive plugins usage.
# 
# This software is governed by the CeCILL-C license under French law and
# abiding by the rules of distribution of free software.  You can  use, 
# modify and/ or redistribute the software under the terms of the CeCILL-C
# license as circulated by CEA, CNRS and INRIA at the following URL
# "http://www.cecill.info". 
# 
# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability. 
# 
# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or 
# data to be ensured and,  more generally, to use and operate it in the 
# same conditions as regards security. 
# 
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL-C license and that you accept its terms.
#

# CMake requirements
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# libonsen ---------------------------------------------------------------------
PROJECT(LibOnsen)

SET(
    libonsen_SRCS
        src/onsen.c
        src/archive_entry.c
        src/archive_info.c
        src/archive_plugin.c
        src/encodings.c
        src/file_utils.c
        src/iconv_utils.c
        src/plugin.c
        src/shift_jis_utils.c
        src/string_utils.c
        src/utils.c
)

SET(
    libonsen_INCS
        src/onsen.h
        src/archive_entry.h
        src/archive_info.h
        src/archive_plugin.h
        src/encodings.h
        src/file_utils.h
        src/iconv_utils.h
        src/plugin.h
        src/shift_jis_utils.h
        src/string_utils.h
        src/utils.h
        src/globals.h
        src/verbosity.h
        src/config.h
)

SET(ONSEN_VERSION_MAJOR 0)
SET(ONSEN_VERSION_MINOR 1)
SET(ONSEN_VERSION_RELEASE "b")

CONFIGURE_FILE(
    "${PROJECT_SOURCE_DIR}/src/config.h.in"
    "${PROJECT_SOURCE_DIR}/src/config.h"
)

ADD_DEFINITIONS(-W -Wall -ansi -pedantic -fPIC -g3)
ADD_LIBRARY(onsen STATIC ${libonsen_SRCS})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

INSTALL(FILES ${libonsen_INCS} DESTINATION "include/libonsen")
INSTALL(TARGETS onsen DESTINATION lib)
