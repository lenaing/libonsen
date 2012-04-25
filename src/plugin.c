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
#include "plugin.h"
#include "archive_plugin.h"

OnsenPlugin_t *
onsen_new_plugin()
{
    OnsenPlugin_t *pPlugin;

    pPlugin = onsen_malloc(sizeof(OnsenPlugin_t));

    pPlugin->bLibraryOpened = 0;
    pPlugin->bLibraryLoaded = 0;
    pPlugin->pLibrary = NULL;
    pPlugin->szLibraryError = NULL;

    pPlugin->iType = ONSEN_PLUGIN_UNSUPPORTED;

    pPlugin->szName = NULL;
    pPlugin->szVersion = NULL;
    pPlugin->szAuthors = NULL;

    pPlugin->pInstance = NULL;

    return pPlugin;
}

void
onsen_free_plugin(OnsenPlugin_t *pPlugin)
{
    if (NULL != pPlugin) {
        onsen_unload_plugin(pPlugin);
        onsen_free(pPlugin);
    }
}

int
onsen_load_plugin(OnsenPlugin_t *pPlugin, const char *szLibFilename)
{
    int rc;

    void *pFun;
    char api[4];
    char *szPluginName;
    char *szPluginVersion;
    char *szPluginAuthors;
    char *szPluginType;

    assert(NULL != pPlugin);
    assert(NULL != szLibFilename);

    /* Library already loaded. Unload it first. */
    if (1 == pPlugin->bLibraryLoaded) {
        onsen_err_warning("Plugin %s %salready loaded. Attempt to reload it...",
                          pPlugin->szName, pPlugin->szVersion);
        rc = onsen_unload_plugin(pPlugin);
        if (rc != 0) {
            onsen_err_ko("Failed to reload plugin %s.", pPlugin->szName);
            errno = EPLGRLD;
            return 1;
        }
    }

    /* Open plugin library. */
    pPlugin->pLibrary = dlopen(szLibFilename, RTLD_LAZY);
    pPlugin->szLibraryError = dlerror();
    if (NULL != pPlugin->szLibraryError) {
        onsen_err_ko("Failed to open library '%s'.", szLibFilename);
        errno = ELIBOPN;
        return 1;
    }
    pPlugin->bLibraryOpened = 1;

    /* Load plugin onsen_get_plugin_infos function. */
    pFun = dlsym(pPlugin->pLibrary, "onsen_get_plugin_info");
    pPlugin->szLibraryError = dlerror();
    if (NULL != pPlugin->szLibraryError) {
        onsen_err_ko("Failed to get plugin infos function from library '%s'.",
                     szLibFilename);
        errno = ELIBPLGGETINF;
        return 1;
    }
    memcpy(&(pPlugin->getPluginInfo), &pFun, sizeof(pPlugin->getPluginInfo));

    /* Check plugin API version */
    if(pPlugin->getPluginInfo(0, api, 4)) {

        /* Check API version used */
        if ((api[0] != ONSEN_API_MAJOR) || (api[1] != ONSEN_API_MINOR)) {
            onsen_err_ko("Unknown Onsen API : '%c.%c' for library '%s'.",
                         api[0],
                         api[1],
                         szLibFilename);
            errno = ELIBWRGAPI;
            return 1;
        }

        /* Check plugin type */
        switch(api[2]) {
            case ONSEN_PLUGIN_ARCHIVE :
            case ONSEN_PLUGIN_PICTURE_IMPORTER :
            case ONSEN_PLUGIN_PICTURE_EXPORTER : {
                pPlugin->iType = api[2];
            } break;
            default : {
                onsen_err_ko("Unknown plugin type : '0x%02X' for library '%s'.",
                             api[2],
                             szLibFilename);
                errno = EPLGINVTYP;
                return 1;
            }
        }

    } else {
        onsen_err_ko("Failed to retrieve plugin metadata from library '%s'.",
                     szLibFilename);
        errno = EPLGMETA;
        return 1;
    }

    /* Try to load generic plugin informations */
    szPluginName = onsen_calloc(sizeof(char), ONSEN_PLUGIN_NAME_SIZE);
    rc = pPlugin->getPluginInfo(1, szPluginName, ONSEN_PLUGIN_NAME_SIZE);
    if (0 == rc) {
        onsen_err_warning("Failed to load plugin name from library '%s'.",
                           szLibFilename);
        sprintf(szPluginName, "%s", "Unknown plugin");
    }
    pPlugin->szName = szPluginName;

    szPluginVersion = onsen_calloc(sizeof(char), ONSEN_PLUGIN_VERSION_SIZE);
    rc = pPlugin->getPluginInfo(2, szPluginVersion, ONSEN_PLUGIN_VERSION_SIZE);
    if (0 == rc) {
        onsen_err_warning("Failed to load plugin version from library '%s'.",
                           szLibFilename);
        szPluginVersion[0] = '\0';
    }
    pPlugin->szVersion = szPluginVersion;

    szPluginAuthors = onsen_calloc(sizeof(char), ONSEN_PLUGIN_AUTHORS_SIZE);
    rc = pPlugin->getPluginInfo(3, szPluginAuthors, ONSEN_PLUGIN_AUTHORS_SIZE);
    if (0 == rc) {
        onsen_err_warning("Failed to load plugin authors from library '%s'.",
                           szLibFilename);
        szPluginAuthors[0] = '\0';
    }
    pPlugin->szAuthors = szPluginAuthors;

    /* Load plugin onsen_is_file_supported function. */
    pFun = dlsym(pPlugin->pLibrary, "onsen_is_file_supported");
    pPlugin->szLibraryError = dlerror();
    if (NULL != pPlugin->szLibraryError) {
        onsen_err_ko("Failed to get plugin %s file support function.",
                     pPlugin->szName);
        errno = ELIBPLGFILSUP;
        return 1;
    }
    memcpy(&(pPlugin->isFileSupported), &pFun,
           sizeof(pPlugin->isFileSupported));

    /* Create instance of plugin */
    rc = onsen_new_plugin_instance(pPlugin);
    if (0 != rc) {
        /* Failed to instantiate plugin instance */
        onsen_err_ko("Failed to load plugin %s functions from library.",
                     szPluginName);
        errno = EPLGMANFUN;
        return 1;
    }

    switch (pPlugin->iType) {
        case ONSEN_PLUGIN_ARCHIVE : {
            szPluginType = "Archive ";
        } break;
        case ONSEN_PLUGIN_PICTURE_IMPORTER : {
            szPluginType = "Picture Importer ";
        } break;
        case ONSEN_PLUGIN_PICTURE_EXPORTER : {
            szPluginType = "Picture eXporter ";
        } break;
        default : szPluginType = "";
    }

    onsen_out_ok("Loaded %splugin %s %s%s.", szPluginType, pPlugin->szName,
                    pPlugin->szVersion, pPlugin->szAuthors);
    pPlugin->bLibraryLoaded = 1;

    return 0;
}

int
onsen_unload_plugin(OnsenPlugin_t *pPlugin)
{
    int rc = 0;

    if (NULL != pPlugin) {
        if (NULL != pPlugin->szName) {
            onsen_free(pPlugin->szName);
        }
        if (NULL != pPlugin->szVersion) {
            onsen_free(pPlugin->szVersion);
        }
        if (NULL != pPlugin->szAuthors) {
            onsen_free(pPlugin->szAuthors);
        }

        if (1 == pPlugin->bLibraryLoaded) {
            if (NULL != pPlugin->pInstance) {
                onsen_free_plugin_instance(pPlugin);
            }
        }

        if (1 == pPlugin->bLibraryOpened) {
            if (NULL != pPlugin->pLibrary) {
                rc = dlclose(pPlugin->pLibrary);
                if (rc != 0) {
                    onsen_err_ko("Failed to close plugin library.");
                    return 1;
                }
            }
        }

        pPlugin->bLibraryLoaded = 0;
        pPlugin->bLibraryOpened = 0;
    }

    return 0;
}

int
onsen_new_plugin_instance(OnsenPlugin_t *pPlugin)
{
    void *pInstance;
    int rc;

    assert(NULL != pPlugin);

    switch(pPlugin->iType) {
        case ONSEN_PLUGIN_ARCHIVE : {
            pInstance = onsen_new_archive_plugin();
            if (NULL == pInstance) {
                onsen_err_ko("Failed to instantiate plugin...");
                return 1;
            }
            pPlugin->pInstance = pInstance;
            rc = onsen_archive_plugin_load_funcs(pPlugin);
            if (0 != rc) {
                onsen_err_ko("Failed to load mandatory functions...");
                onsen_free_archive_plugin(pInstance);
                return 1;
            }
            
        } break;

        default : {
            onsen_err_ko("Can't instantiate unknow plugin type '%c'...",
                         pPlugin->iType);
            return 1;
        }
    }

    return 0;
}

void
onsen_free_plugin_instance(OnsenPlugin_t *pPlugin)
{
    assert(NULL != pPlugin);

    if (NULL != pPlugin->pInstance) {
        /* Check plugin type */
        switch(pPlugin->iType) {
            case ONSEN_PLUGIN_ARCHIVE : {
                onsen_free_archive_plugin(pPlugin->pInstance);
            } break;
            default : {
                /* Should never happen. */
            };
        }
    }
}
