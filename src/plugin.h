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
#ifndef __ONSEN_PLUGIN_H
#define __ONSEN_PLUGIN_H

#define ONSEN_API_MAJOR '0'
#define ONSEN_API_MINOR '1'

#define ONSEN_PLUGIN_UNSUPPORTED 0
#define ONSEN_PLUGIN_ARCHIVE 'A'
#define ONSEN_PLUGIN_PICTURE_IMPORTER 'I'
#define ONSEN_PLUGIN_PICTURE_EXPORTER 'X'

#define ONSEN_PLUGIN_NAME_SIZE 65
#define ONSEN_PLUGIN_VERSION_SIZE 65
#define ONSEN_PLUGIN_AUTHORS_SIZE 65

#define ONSEN_DISK_FILE 0
#define ONSEN_MEMORY_FILE 1

#define ELIBOPN         1   /* Failed to open library. */
#define ELIBPLGGETINF   2   /* Failed to find onsen_get_plugin_info function in
                             * library. */
#define ELIBPLGFILSUP   3   /* Failed to find onsen_is_file_supported function 
                             * in library. */
#define ELIBWRGAPI      4   /* Library uses a wrong or unknown API version. */
#define EPLGINVTYP      5   /* Library provide an unknown plugin type. */
#define EPLGMETA        6   /* Failed to read plugin metadata in library. */
#define EPLGMANFUN      7   /* Failed to find mandatory functions in library. */
#define EPLGRLD         8   /* Failed to reload plugin. */


#include "utils.h"
#include <errno.h>
#include <dlfcn.h>

typedef struct _OnsenPlugin_s OnsenPlugin_t;
struct _OnsenPlugin_s
{
    int bLibraryOpened;                     /* Is plugin library opened?      */
    int bLibraryLoaded;                     /* Is plugin library loaded?      */
    void *pLibrary;                         /* Plugin library handle.         */
    char *szLibraryError;                   /* Plugin library last error.     */

    int iType;                              /* Plugin type.                   */
    char *szName;                           /* Plugin name.                   */
    char *szVersion;                        /* Plugin version.                */
    char *szAuthors;                        /* Plugin authors.                */

    void *pInstance;                        /* Plugin specific instance.      */

    /* Mandatory plugin functions. */
    int (*getPluginInfo)(int, char *, int);
    int (*isFileSupported)(int, char *, void *, long);
};

OnsenPlugin_t *onsen_new_plugin(void);
void onsen_free_plugin(OnsenPlugin_t *);

int onsen_load_plugin(OnsenPlugin_t *, const char *);
int onsen_unload_plugin(OnsenPlugin_t *);

int onsen_new_plugin_instance(OnsenPlugin_t *);
void onsen_free_plugin_instance(OnsenPlugin_t *);

#endif /* __ONSEN_PLUGIN_H */
