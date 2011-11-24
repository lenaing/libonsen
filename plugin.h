#ifndef __ONSEN_PLUGIN_H
#define __ONSEN_PLUGIN_H

#define ONSEN_PLUGIN_UNSUPPORTED 0
#define ONSEN_PLUGIN_ARCHIVE 'A'
#define ONSEN_API_MAJOR '0'
#define ONSEN_API_MINOR '1'
#define ONSEN_PLUGIN_NAME_SIZE 64
#define ONSEN_PLUGIN_VERSION_SIZE 64
#define ONSEN_PLUGIN_AUTHORS_SIZE 64

#include "utils.h"
#include <dlfcn.h>

typedef struct _OnsenPlugin_s OnsenPlugin_t;

struct _OnsenPlugin_s
{
    int bLibraryloaded;                     /* Is plugin library loaded?      */
    void *pLibrary;                         /* Plugin library handle.         */
    const char *szLibraryError;             /* Plugin library last error.     */

    int iType;                              /* Plugin type.                   */
    char *szName;                           /* Plugin name.                   */
    char *szVersion;                        /* Plugin version.                */
    char *szAuthors;                        /* Plugin authors.                */

    void *pInstance;
    int (*getPluginInfos)(int, char *, int);
};

OnsenPlugin_t *onsen_new_plugin(void);
void onsen_free_plugin(OnsenPlugin_t *);

int onsen_load_plugin(OnsenPlugin_t *, const char *);
int onsen_unload_plugin(OnsenPlugin_t *);

int onsen_new_plugin_instance(OnsenPlugin_t *);
void onsen_free_plugin_instance(OnsenPlugin_t *);

#endif /* __ONSEN_PLUGIN_H */
