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
#include "archive_plugin.h"

OnsenArchivePlugin_t *
onsen_new_archive_plugin()
{
    OnsenArchivePlugin_t *pPlugin;

    pPlugin = onsen_malloc(sizeof(OnsenArchivePlugin_t));

    pPlugin->bLibraryloaded = 0;
    pPlugin->pLibrary = NULL;
    pPlugin->szLibraryError = NULL;

    pPlugin->szIDS = NULL;
    pPlugin->szName = NULL;
    pPlugin->szAuthors = NULL;
    pPlugin->szExtensions = NULL;
    pPlugin->szVersion = NULL;

    pPlugin->bArchiveOpened = 0;
    pPlugin->pArchiveFile = NULL;
    pPlugin->lArchiveFileSize = 0;
    pPlugin->a_pArchiveEntries = NULL;
    pPlugin->iArchiveEntriesCount = 0;

    pPlugin->openArchive = NULL;
    pPlugin->closeArchive = NULL;
    pPlugin->loadArchiveInfos = NULL;
    pPlugin->extractAllArchive = &onsen_dump_archive_raw;
    pPlugin->extractFromArchive = &onsen_dump_archive_entry_raw;

    return pPlugin;
}

void
onsen_free_archive_plugin(OnsenArchivePlugin_t *pPlugin)
{
    if (NULL != pPlugin) {
        /* TODO : UNLOAD? */
        onsen_free(pPlugin);
    }
}

int
onsen_load_archive_plugin(OnsenArchivePlugin_t *pPlugin,
                          const char *szLibFilename)
{
    int rc;

    assert(NULL != pPlugin);
    assert(NULL != szLibFilename);

    /* Library already loaded. Unload it first. */
    if (pPlugin->bLibraryloaded) {
        rc = onsen_unload_archive_plugin(pPlugin);
        if (rc != 0) {
            return 1;
        }
    }

    /* Open plugin library. */
    pPlugin->pLibrary = dlopen(szLibFilename, RTLD_LAZY);
    pPlugin->szLibraryError = dlerror();
    if (NULL != pPlugin->szLibraryError) {
        onsen_err_ko("Failed to load archive plugin from library '%s'.",
                     szLibFilename);
        return 1;
    }

    /* Load plugins required functions. */
    rc = onsen_archive_plugin_load_funcs(pPlugin);
    if (rc != 0) {
        onsen_err_ko("Failed to load archive plugin functions from library '%s'.",
                     szLibFilename);
        rc = dlclose(pPlugin->pLibrary);
        if (rc != 0) {
            onsen_err_ko("Failed to close library '%s'.", szLibFilename);
        }
        return 1;
    }

    /* Load plugins required infos. */
    pPlugin->loadArchiveInfos(pPlugin);
    if (NULL == pPlugin->szIDS
        || NULL == pPlugin->szName
        || NULL == pPlugin->szExtensions
        || NULL == pPlugin->szVersion) {
        onsen_err_ko("Failed to load archive plugin infos from library '%s'.",
              szLibFilename);
        rc = dlclose(pPlugin->pLibrary);
        if (rc != 0) {
            onsen_err_ko("Failed to close library '%s'.", szLibFilename);
        }
        return 1;
    }

    onsen_out_ok("Loaded plugin %s v%s.", pPlugin->szIDS, pPlugin->szVersion);
    pPlugin->bLibraryloaded = 1;

    return 0;
}

int
onsen_unload_archive_plugin(OnsenArchivePlugin_t *pPlugin)
{
    int rc;
    char *szIDS;
    char *szVersion;

    assert(NULL != pPlugin);

    if (pPlugin->bLibraryloaded) {
        szIDS = onsen_strdup(pPlugin->szIDS);
        szVersion = onsen_strdup(pPlugin->szVersion);
        rc = dlclose(pPlugin->pLibrary);
        if (rc) {
            onsen_err_ko("Failed to close plugin %s v%s.", szIDS, szVersion);
            onsen_free(szIDS);
            onsen_free(szVersion);
            return rc;
        }
        onsen_out_ok("Closed plugin %s v%s.", szIDS, szVersion);
        onsen_free(szIDS);
        onsen_free(szVersion);
        pPlugin->bLibraryloaded = 0;
    }

    return 0;
}

int
onsen_archive_plugin_load_funcs(OnsenArchivePlugin_t *pPlugin)
{
    void *pFun;

    assert(NULL != pPlugin);

    /* Library already loaded. */
    if (pPlugin->bLibraryloaded) {
        onsen_err_warning("Plugin %s already loaded.", pPlugin->szIDS);
        return 0;
    }

    pFun = dlsym(pPlugin->pLibrary, "onsen_load_infos");
    pPlugin->szLibraryError = dlerror();
    if (NULL != pPlugin->szLibraryError) {
        return 1;
    }
    memcpy(&(pPlugin->loadArchiveInfos), &pFun, sizeof(pPlugin->loadArchiveInfos));

    pFun = dlsym(pPlugin->pLibrary, "onsen_open_archive");
    pPlugin->szLibraryError = dlerror();
    if (NULL != pPlugin->szLibraryError) {
        return 1;
    }
    memcpy(&(pPlugin->openArchive), &pFun, sizeof(pPlugin->openArchive));

    pFun = dlsym(pPlugin->pLibrary, "onsen_close_archive");
    pPlugin->szLibraryError = dlerror();
    if (NULL != pPlugin->szLibraryError) {
        return 1;
    }
    memcpy(&(pPlugin->closeArchive), &pFun, sizeof(pPlugin->closeArchive));

    /* Optional functions */
    pFun = dlsym(pPlugin->pLibrary, "onsen_extract_all_archive");
    pPlugin->szLibraryError = dlerror();
    if (NULL == pPlugin->szLibraryError) {
        memcpy(&(pPlugin->extractAllArchive), &pFun, sizeof(pPlugin->extractAllArchive));
    }

    pFun = dlsym(pPlugin->pLibrary, "onsen_extract_from_archive");
    pPlugin->szLibraryError = dlerror();
    if (NULL == pPlugin->szLibraryError) {
        memcpy(&(pPlugin->extractFromArchive), &pFun, sizeof(pPlugin->extractFromArchive));
    }

    return 0;
}

