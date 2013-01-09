/*
 * Copyright 2011-2013 - Etienne 'lenaing' GIRONDEL <lenaing@gmail.com>
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
#include "picture_importer_plugin.h"

OnsenPlugin_t *
onsen_new_plugin()
{
    OnsenPlugin_t *plugin;

    plugin = onsen_malloc(sizeof(OnsenPlugin_t));

    plugin->isLibraryOpened = 0;
    plugin->isLibraryLoaded = 0;
    plugin->library         = NULL;
    plugin->libraryError    = NULL;
    plugin->type            = ONSEN_PLUGIN_UNSUPPORTED;
    plugin->name            = NULL;
    plugin->version         = NULL;
    plugin->authors         = NULL;
    plugin->instance        = NULL;

    return plugin;
}

void
onsen_free_plugin(OnsenPlugin_t *plugin)
{
    assert(NULL != plugin);

    if (NULL != plugin) {
        onsen_unload_plugin(plugin);
        onsen_free(plugin);
    }
}

int
onsen_load_plugin(OnsenPlugin_t *plugin, const char *libFilename)
{
    int rc;

    void *fun;
    char api[4];
    char *pluginName;
    char *pluginVersion;
    char *pluginAuthors;
    char *pluginType;

    assert(NULL != plugin);
    assert(NULL != libFilename);

    /* Library already loaded. Unload it first. */
    if (1 == plugin->isLibraryLoaded) {
        onsen_err_warning("Plugin %s %salready loaded. Attempt to reload it...",
                          plugin->name, plugin->version);
        rc = onsen_unload_plugin(plugin);
        if (rc != 0) {
            onsen_err_ko("Failed to reload plugin %s.", plugin->name);
            errno = ONSEN_E_PLUGIN_RELOAD;
            return 1;
        }
    }

    /* Open plugin library. */
    plugin->library = dlopen(libFilename, RTLD_LAZY);
    plugin->libraryError = dlerror();
    if (NULL != plugin->libraryError) {
        onsen_err_ko("Failed to open library '%s'.", libFilename);
        errno = ONSEN_E_LIB_OPEN;
        return 1;
    }
    plugin->isLibraryOpened = 1;

    /* Load plugin onsen_get_plugin_infos function. */
    fun = dlsym(plugin->library, "onsen_get_plugin_info");
    plugin->libraryError = dlerror();
    if (NULL != plugin->libraryError) {
        onsen_err_ko("Failed to get plugin infos function from library '%s'.",
                     libFilename);
        errno = ONSEN_E_LIB_GET_PLUGIN_INFO;
        return 1;
    }
    memcpy(&(plugin->getPluginInfo), &fun, sizeof(plugin->getPluginInfo));

    /* Check plugin API version */
    if(plugin->getPluginInfo(0, api, 4)) {

        /* Check API version used */
        if ((api[0] != ONSEN_API_MAJOR) || (api[1] != ONSEN_API_MINOR)) {
            onsen_err_ko("Unknown Onsen API : '%c.%c' for library '%s'.",
                         api[0],
                         api[1],
                         libFilename);
            errno = ONSEN_E_WRONG_API;
            return 1;
        }

        /* Check plugin type */
        switch(api[2]) {
            case ONSEN_PLUGIN_ARCHIVE :
            case ONSEN_PLUGIN_PICTURE_IMPORTER :
            case ONSEN_PLUGIN_PICTURE_EXPORTER : {
                plugin->type = api[2];
            } break;
            default : {
                onsen_err_ko("Unknown plugin type : '0x%02X' for library '%s'.",
                             api[2],
                             libFilename);
                errno = ONSEN_E_PLUGIN_TYPE;
                return 1;
            }
        }

    } else {
        onsen_err_ko("Failed to retrieve plugin metadata from library '%s'.",
                     libFilename);
        errno = ONSEN_E_PLUGIN_METADATA;
        return 1;
    }

    /* Try to load generic plugin informations */
    pluginName = onsen_calloc(sizeof(char), ONSEN_PLUGIN_NAME_SIZE);
    rc = plugin->getPluginInfo(1, pluginName, ONSEN_PLUGIN_NAME_SIZE);
    if (0 == rc) {
        onsen_err_warning("Failed to load plugin name from library '%s'.",
                           libFilename);
        sprintf(pluginName, "%s", "Unknown plugin");
    }
    plugin->name = pluginName;

    pluginVersion = onsen_calloc(sizeof(char), ONSEN_PLUGIN_VERSION_SIZE);
    rc = plugin->getPluginInfo(2, pluginVersion, ONSEN_PLUGIN_VERSION_SIZE);
    if (0 == rc) {
        onsen_err_warning("Failed to load plugin version from library '%s'.",
                           libFilename);
        pluginVersion[0] = '\0';
    }
    plugin->version = pluginVersion;

    pluginAuthors = onsen_calloc(sizeof(char), ONSEN_PLUGIN_AUTHORS_SIZE);
    rc = plugin->getPluginInfo(3, pluginAuthors, ONSEN_PLUGIN_AUTHORS_SIZE);
    if (0 == rc) {
        onsen_err_warning("Failed to load plugin authors from library '%s'.",
                           libFilename);
        pluginAuthors[0] = '\0';
    }
    plugin->authors = pluginAuthors;

    /* Load plugin onsen_is_file_supported function. */
    fun = dlsym(plugin->library, "onsen_is_file_supported");
    plugin->libraryError = dlerror();
    if (NULL != plugin->libraryError) {
        onsen_err_ko("Failed to get plugin %s file support function.",
                     plugin->name);
        errno = ONSEN_E_LIB_IS_FILE_SUPPORTED;
        return 1;
    }
    memcpy(&(plugin->isFileSupported), &fun,
           sizeof(plugin->isFileSupported));

    /* Create instance of plugin */
    rc = onsen_new_plugin_instance(plugin);
    if (0 != rc) {
        /* Failed to instantiate plugin instance */
        onsen_err_ko("Failed to load plugin %s functions from library.",
                     pluginName);
        errno = ONSEN_E_PLUGIN_FUNCTIONS;
        return 1;
    }

    switch (plugin->type) {
        case ONSEN_PLUGIN_ARCHIVE : {
            pluginType = "Archive ";
        } break;
        case ONSEN_PLUGIN_PICTURE_IMPORTER : {
            pluginType = "Picture Importer ";
        } break;
        case ONSEN_PLUGIN_PICTURE_EXPORTER : {
            pluginType = "Picture eXporter ";
        } break;
        default : pluginType = "";
    }

    onsen_out_ok("Loaded %splugin %s %s%s.", pluginType, plugin->name,
                    plugin->version, plugin->authors);
    plugin->isLibraryLoaded = 1;

    return 0;
}

int
onsen_unload_plugin(OnsenPlugin_t *plugin)
{
    int rc = 0;

    assert(NULL != plugin);

    if (NULL != plugin) {
        if (NULL != plugin->name) {
            onsen_free(plugin->name);
        }
        if (NULL != plugin->version) {
            onsen_free(plugin->version);
        }
        if (NULL != plugin->authors) {
            onsen_free(plugin->authors);
        }

        if (1 == plugin->isLibraryLoaded) {
            if (NULL != plugin->instance) {
                onsen_free_plugin_instance(plugin);
            }
        }

        if (1 == plugin->isLibraryOpened) {
            if (NULL != plugin->library) {
                rc = dlclose(plugin->library);
                if (rc != 0) {
                    onsen_err_ko("Failed to close plugin library.");
                    return 1;
                }
            }
        }

        plugin->isLibraryLoaded = 0;
        plugin->isLibraryOpened = 0;
    }

    return 0;
}

int
onsen_new_plugin_instance(OnsenPlugin_t *plugin)
{
    void *instance;
    int rc;

    assert(NULL != plugin);

    switch(plugin->type) {
        case ONSEN_PLUGIN_ARCHIVE : {
            instance = onsen_new_archive_plugin();
            if (NULL == instance) {
                onsen_err_ko("Failed to instantiate plugin...");
                return 1;
            }
            plugin->instance = instance;
            rc = onsen_archive_plugin_load_funcs(plugin);
            if (0 != rc) {
                onsen_err_ko("Failed to load mandatory functions...");
                onsen_free_archive_plugin(instance);
                return 1;
            }
        } break;
        case ONSEN_PLUGIN_PICTURE_IMPORTER : {
            instance = onsen_new_picture_importer_plugin();
            if (NULL == instance) {
                onsen_err_ko("Failed to instantiate plugin...");
                return 1;
            }
            plugin->instance = instance;
            rc = onsen_picture_importer_plugin_load_funcs(plugin);
            if (0 != rc) {
                onsen_err_ko("Failed to load mandatory functions...");
                onsen_free_picture_importer_plugin(instance);
                return 1;
            }
        } break;

        default : {
            onsen_err_ko("Can't instantiate unknow plugin type '%c'...",
                         plugin->type);
            return 1;
        }
    }

    return 0;
}

void
onsen_free_plugin_instance(OnsenPlugin_t *plugin)
{
    assert(NULL != plugin);

    if (NULL != plugin->instance) {
        /* Check plugin type */
        switch(plugin->type) {
            case ONSEN_PLUGIN_ARCHIVE : {
                onsen_free_archive_plugin(plugin->instance);
            } break;
            case ONSEN_PLUGIN_PICTURE_IMPORTER : {
                onsen_free_picture_importer_plugin(plugin->instance);
            } break;
            default : {
                /* Should never happen. */
            };
        }
    }
}
