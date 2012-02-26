/*
 * Copyright 2011, 2012 - Etienne 'lenaing' GIRONDEL <lenaing@gmail.com>
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
#ifndef __ONSEN_ARCHIVE_PLUGIN_H
#define __ONSEN_ARCHIVE_PLUGIN_H

#ifndef _XOPEN_SOURCE
    #define _XOPEN_SOURCE 500
#endif
#include "archive_entry.h"
#include "archive_info.h"
#include "file_utils.h"
#include "plugin.h"
#include <fcntl.h>
#include <sys/mman.h>

typedef void (*OnsenWriteFileCallback)(int , int , void *);

typedef struct _OnsenArchivePlugin_s OnsenArchivePlugin_t;
struct _OnsenArchivePlugin_s
{
    /* Mandatory archive functions.        */
    int (*getArchiveInfo)(int, void *, long, OnsenArchiveInfo_t *);
    int (*getFileInfo)(int, void *, long, char *,OnsenArchiveEntry_t *);

    /* Optional archive functions.        */
    int (*writeFile)(int, void *, long, int, void *, long,
                     OnsenWriteFileCallback, void *);
};

OnsenArchivePlugin_t *onsen_new_archive_plugin(void);
void onsen_free_archive_plugin(OnsenArchivePlugin_t *);

int onsen_load_archive_plugin(OnsenArchivePlugin_t *, const char *);
int onsen_unload_archive_plugin(OnsenArchivePlugin_t *);
int onsen_archive_plugin_load_funcs(OnsenPlugin_t *);

int onsen_write_file_raw(int, void *, long, int, void *, long,
                         OnsenWriteFileCallback, void *);

#endif /* __ONSEN_ARCHIVE_PLUGIN_H */
