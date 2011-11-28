#include "plugin.h"
#include "archive_plugin.h"

OnsenPlugin_t *
onsen_new_plugin()
{
    OnsenPlugin_t *pPlugin;

    pPlugin = onsen_malloc(sizeof(OnsenPlugin_t));

    pPlugin->bLibraryloaded = 0;
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

        if (NULL != pPlugin->szName) {
            onsen_free(pPlugin->szName);
        }
        if (NULL != pPlugin->szVersion) {
            onsen_free(pPlugin->szVersion);
        }
        if (NULL != pPlugin->szAuthors) {
            onsen_free(pPlugin->szAuthors);
        }

        if (NULL != pPlugin->pLibrary && pPlugin->bLibraryloaded) {
            dlclose(pPlugin->pLibrary);
        }

        if (NULL != pPlugin->pInstance && pPlugin->bLibraryloaded) {
            onsen_free_plugin_instance(pPlugin);
        }

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


    assert(NULL != pPlugin);
    assert(NULL != szLibFilename);

    /* Library already loaded. Unload it first. */
    if (pPlugin->bLibraryloaded) {
        rc = onsen_unload_plugin(pPlugin);
        if (rc != 0) {
            /* TODO change return code */
            return 1;
        }
    }

    /* Open plugin library. */
    pPlugin->pLibrary = dlopen(szLibFilename, RTLD_LAZY);
    pPlugin->szLibraryError = dlerror();
    if (NULL != pPlugin->szLibraryError) {
        onsen_err_ko("Failed to open library '%s'.", szLibFilename);
        return 1;
    }

    /* Load plugin onsen_get_plugin_infos function. */
    pFun = dlsym(pPlugin->pLibrary, "onsen_get_plugin_info");
    pPlugin->szLibraryError = dlerror();
    if (NULL != pPlugin->szLibraryError) {
        onsen_err_ko("Failed to get plugin infos function from library '%s'.",
                     szLibFilename);
        return 2;
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
            return 3;
        }

        /* Check plugin type */
        switch(api[2]) {
            case ONSEN_PLUGIN_ARCHIVE : {
                pPlugin->iType = api[2];
            } break;
            default : {
                onsen_err_ko("Unknown plugin type : '0x%02X' for library '%s'.",
                             api[2],
                             szLibFilename);
                return 4;
            }
        }

    } else {
        onsen_err_ko("Unable to retrieve plugin infos from library '%s'.",
                     szLibFilename);
        return 5;
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
        return 6;
    }
    memcpy(&(pPlugin->isFileSupported), &pFun, sizeof(pPlugin->isFileSupported));

    /* Create instance of plugin */
    rc = onsen_new_plugin_instance(pPlugin);
    if (rc != 0) {
        /* Failed to instanciate plugin instance */
        onsen_err_ko("Failed to load plugin %s functions from library.",
                     szPluginName);
        rc = dlclose(pPlugin->pLibrary);
        if (rc != 0) {
            onsen_err_ko("Failed to close library '%s'.", szLibFilename);
        }

        return 7;
    }

    onsen_out_ok("Loaded plugin %s %s%s.", pPlugin->szName, pPlugin->szVersion, pPlugin->szAuthors);
    pPlugin->bLibraryloaded = 1;

    return 0;
}

int
onsen_unload_plugin(OnsenPlugin_t *pPlugin)
{
    /* TODO */
    onsen_free(pPlugin->szName);
    onsen_free(pPlugin->szVersion);
    onsen_free(pPlugin->szAuthors);
    return 0;
}

int
onsen_new_plugin_instance(OnsenPlugin_t *pPlugin)
{
    void *pInstance;
    int rc;

    assert(NULL != pPlugin);

    /* Check plugin type */
    switch(pPlugin->iType) {
        case ONSEN_PLUGIN_ARCHIVE : {
            pInstance = onsen_new_archive_plugin();
            if (NULL == pInstance) {
                /* Failed to instanciate. */
                onsen_err_ko("Failed to instantiate plugin...");
                return 1;
            }
            pPlugin->pInstance = pInstance;
            rc = onsen_archive_plugin_load_funcs(pPlugin);
            if (0 != rc) {
                /* Failed to load necessary functions. */
                onsen_err_ko("Failed to load functions...");
                onsen_free_archive_plugin(pInstance);
                return 1;
            }
            
        } break;
        default : {
            /* Can't instanciate unknown plugin type. */
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
            default :
             ;
        }

    }
}
