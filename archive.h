/*
 * Copyright 2011 - Etienne 'lenaing' GIRONDEL <lenaing@gmail.com>
 * 
 * libonsen
 * --------
 * This library is a collection of resources whose purpose is to offer an easy
 * to use framework to open and manipulate (mostly games) data archives through
 * an extensive plugins usage.
 * 
 * This software is governed by the CeCILL-C license under French law and
 * abiding by the rules of distribution of free software.  You can  use, 
 * modify and/ or redistribute the software under the terms of the CeCILL-C
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info". 
 * 
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability. 
 * 
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or 
 * data to be ensured and,  more generally, to use and operate it in the 
 * same conditions as regards security. 
 * 
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-C license and that you accept its terms.
 */
#ifndef __ONSEN_ARCHIVE_H
#define __ONSEN_ARCHIVE_H

#include "string_utils.h"
#include "file_utils.h"

typedef struct _OnsenArchivePlugin_s OnsenArchivePlugin_t;
typedef struct _OnsenArchiveEntry_s OnsenArchiveEntry_t;

struct _OnsenArchivePlugin_s
{
    int bLibraryloaded;                     /* Is plugin library loaded?      */
    void *pLibrary;                         /* Plugin library handle.         */
    const char *szLibraryError;             /* Plugin library last error.     */

    char *szIDS;                            /* Plugin Identification String.  */
    char *szName;                           /* Plugin name.                   */
    char *szVersion;                        /* Plugin version.                */
    char *szExtensions;                     /* Plugin recognized extensions.  */
    char *szAuthors;                        /* Plugin authors.                */

    int bArchiveOpened;                     /* Is archive file opened?        */
    FILE *pArchiveFile;                     /* Opened archive file handle.    */
    long lArchiveFileSize;                  /* Archive file size.             */
    OnsenArchiveEntry_t **a_pArchiveEntries;/* Archive file entries.          */
    int iArchiveEntriesCount;               /* Archive file entries count.    */

    /* Open archive function.        */
    int (*openArchive)(OnsenArchivePlugin_t *, FILE *);
    /* Close archive function.       */
    void (*closeArchive)(OnsenArchivePlugin_t *);
    /* Load archive infos function.  */
    void (*loadArchiveInfos)(OnsenArchivePlugin_t *); 

    /* Optional functions. */
    /* Load archive infos function.  */
    int (*extractAllArchive)(OnsenArchivePlugin_t *, const char*);
    int (*extractFromArchive)(OnsenArchivePlugin_t *, const char*, int); 
};

struct _OnsenArchiveEntry_s
{
    char *szFilename;                       /* Entry filename.                */
    int iOffset;                            /* Entry file offset in archive.  */
    int iSize;                              /* Entry full file size.          */
    int iCompressedSize;                    /* Entry compressed file size.    */
    int bEncrypted;                         /* Is file encrypted?             */
    int bCompressed;                        /* Is file compressed?            */
    char **a_szAddlFds;                     /* Additional fields.             */
    int iAddlFdsCount;                      /* Additional fields count.       */
};

int onsen_dump_archive_raw(OnsenArchivePlugin_t *, const char *);
int onsen_dump_archive_entry_raw(OnsenArchivePlugin_t *, const char *, int);
#endif /* __ONSEN_ARCHIVE_H */
