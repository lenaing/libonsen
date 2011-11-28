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

#define BUFFER_SIZE 4096

OnsenArchivePlugin_t *
onsen_new_archive_plugin()
{
    OnsenArchivePlugin_t *pPlugin;

    pPlugin = onsen_malloc(sizeof(OnsenArchivePlugin_t));

    pPlugin->writeFile = &onsen_write_file_raw;

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

    pFun = dlsym(pPlugin->pLibrary, "onsen_get_archive_info");
    pPlugin->szLibraryError = dlerror();
    if (NULL != pPlugin->szLibraryError) {
        onsen_err_ko("^#~{@^~# : %s", pPlugin->szLibraryError);
        return 1;
    }
    if (NULL == pInstance) {
        onsen_err_ko("arch");
    }
    memcpy(&(pInstance->getArchiveInfo), &pFun, sizeof(pInstance->getArchiveInfo));

    pFun = dlsym(pPlugin->pLibrary, "onsen_get_file_info");
    pPlugin->szLibraryError = dlerror();
    if (NULL != pPlugin->szLibraryError) {
        return 1;
    }
    memcpy(&(pInstance->getFileInfo), &pFun, sizeof(pInstance->getFileInfo));

    /* Optional functions */
    pFun = dlsym(pPlugin->pLibrary, "onsen_write_file");
    pPlugin->szLibraryError = dlerror();
    if (NULL == pPlugin->szLibraryError) {
        memcpy(&(pInstance->writeFile), &pFun, sizeof(pInstance->writeFile));
    }

    return 0;
}

int
onsen_write_file_raw(void *szSrcFile, int iSrcType, long lSrcOffset,
                     int iSrcSize, void *szDstFile, int iDstType,
                     writecallback pCallback, void *pData)
{
    FILE *pSrcFile;
    FILE *pDstFile;

    unsigned char *aBuffer;
    int iToRead = 0;
    int iCount = 0;
    int iRemaining = 0;
    int rc = 0;


    if (0 == iSrcType) {
        /* Disk file */
        pSrcFile = onsen_open_file((char *)szSrcFile, "rb");
        aBuffer = onsen_calloc(sizeof(char), BUFFER_SIZE);
    } else {
        /* Memory file */
        /* TODO */
    }


    if (0 == iDstType) {
        /* Disk file */
        pDstFile = onsen_open_file((char *)szDstFile, "wb");
        
    } else {
        /* Memory file */
        /* TODO */
    }

    if (NULL != pDstFile) {
        onsen_file_goto(pSrcFile, lSrcOffset);
        pCallback(iSrcSize, 0, pData);
        if (iSrcSize < BUFFER_SIZE) {
            fread(aBuffer, iSrcSize, 1, pSrcFile);
            fwrite(aBuffer, iSrcSize, 1, pDstFile);
            pCallback(iSrcSize, iSrcSize, pData);
        } else {
            while (iCount < iSrcSize) {
                iRemaining = iSrcSize - iCount;
                if (iRemaining < BUFFER_SIZE) {
                    iToRead = iRemaining;
                } else {
                    iToRead = BUFFER_SIZE;
                }
                fread(aBuffer, iToRead, 1, pSrcFile);
                fwrite(aBuffer, iToRead, 1, pDstFile);
                iCount += iToRead;
                pCallback(iSrcSize, iCount, pData);
            }
        }

        
    } else {
        rc = 1;
    }

    onsen_close_file(pDstFile);
    onsen_close_file(pSrcFile);
    onsen_free(aBuffer);

    return rc;
}
