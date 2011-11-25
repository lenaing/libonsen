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

    pPlugin->bArchiveOpened = 0;
    pPlugin->pArchiveFile = NULL;
    pPlugin->lArchiveFileSize = 0;
    pPlugin->a_pArchiveEntries = NULL;
    pPlugin->iArchiveEntriesCount = 0;

    pPlugin->openArchive = NULL;
    pPlugin->closeArchive = NULL;
    pPlugin->extractAllArchive = &onsen_dump_archive_raw;
    pPlugin->extractFromArchive = &onsen_dump_archive_entry_raw;

    return pPlugin;
}

void
onsen_free_archive_plugin(OnsenArchivePlugin_t *pPlugin)
{
    if (NULL != pPlugin) {
        /* TODO : UNLOAD should be done by the plugin, but double check here. */
        onsen_free(pPlugin);
    }
}

int
onsen_archive_plugin_load_funcs(OnsenPlugin_t *pPlugin)
{
    void *pFun;
    OnsenArchivePlugin_t *pInstance;


    assert(NULL != pPlugin);

    /* Library already loaded. */
    if (pPlugin->bLibraryloaded) {
        onsen_err_warning("Plugin %s already loaded.", pPlugin->szName);
        return 0;
    }

    pInstance = pPlugin->pInstance;

    onsen_err_ko("Trying functions : onsen_open_archive...");

    pFun = dlsym(pPlugin->pLibrary, "onsen_open_archive");
    pPlugin->szLibraryError = dlerror();
    if (NULL != pPlugin->szLibraryError) {
        onsen_err_ko("^#~{@^~# : %s", pPlugin->szLibraryError);
        return 1;
    }
    if (NULL == pInstance) {
        onsen_err_ko("arch");
    }
    memcpy(&(pInstance->openArchive), &pFun, sizeof(pInstance->openArchive));

    onsen_err_ko("Trying functions : onsen_close_archive...");
    pFun = dlsym(pPlugin->pLibrary, "onsen_close_archive");
    pPlugin->szLibraryError = dlerror();
    if (NULL != pPlugin->szLibraryError) {
        return 1;
    }
    memcpy(&(pInstance->closeArchive), &pFun, sizeof(pInstance->closeArchive));

    /* Optional functions */
    onsen_err_ko("Trying functions : onsen_extract_all_archive...");
    pFun = dlsym(pPlugin->pLibrary, "onsen_extract_all_archive");
    pPlugin->szLibraryError = dlerror();
    if (NULL == pPlugin->szLibraryError) {
        memcpy(&(pInstance->extractAllArchive), &pFun, sizeof(pInstance->extractAllArchive));
    }

    onsen_err_ko("Trying functions : onsen_extract_from_archive...");
    pFun = dlsym(pPlugin->pLibrary, "onsen_extract_from_archive");
    pPlugin->szLibraryError = dlerror();
    if (NULL == pPlugin->szLibraryError) {
        memcpy(&(pInstance->extractFromArchive), &pFun, sizeof(pInstance->extractFromArchive));
    }

    return 0;
}

