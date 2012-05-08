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
#include "archive_plugin.h"

OnsenArchivePlugin_t *
onsen_new_archive_plugin()
{
    OnsenArchivePlugin_t *pPlugin;

    pPlugin = onsen_malloc(sizeof(OnsenArchivePlugin_t));
    pPlugin->getArchiveInfo = NULL;
    pPlugin->getFileInfo = NULL;
    pPlugin->writeFile = &onsen_write_file_raw;

    return pPlugin;
}

void
onsen_free_archive_plugin(OnsenArchivePlugin_t *pPlugin)
{
    if (NULL != pPlugin) {
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
    if (pPlugin->bLibraryLoaded) {
        onsen_err_warning("Plugin %s already loaded.", pPlugin->szName);
        return 0;
    }

    pInstance = pPlugin->pInstance;
    if (NULL == pInstance) {
        return 1;
    }

    pFun = dlsym(pPlugin->pLibrary, "onsen_get_archive_info");
    pPlugin->szLibraryError = dlerror();
    if (NULL != pPlugin->szLibraryError) {
        return 2;
    }
    memcpy(&(pInstance->getArchiveInfo), &pFun,
                sizeof(pInstance->getArchiveInfo));

    pFun = dlsym(pPlugin->pLibrary, "onsen_get_file_info");
    pPlugin->szLibraryError = dlerror();
    if (NULL != pPlugin->szLibraryError) {
        return 3;
    }
    memcpy(&(pInstance->getFileInfo), &pFun, sizeof(pInstance->getFileInfo));

    /* Optional function */
    pFun = dlsym(pPlugin->pLibrary, "onsen_write_file");
    pPlugin->szLibraryError = dlerror();
    if (NULL == pPlugin->szLibraryError) {
        memcpy(&(pInstance->writeFile), &pFun, sizeof(pInstance->writeFile));
    }

    return 0;
}

int
onsen_write_file_raw(int iSrcType, void *szSrcFile, long lSrcOffset,
                 int iDstType, void *szDstFile, long lDstFileSize,
                 OnsenWriteFileCallback pCallback, void *pData)
{
    OnsenFile_t *pSrcDiskFile;
    OnsenFile_t *pDstDiskFile;
    int bError = 0;
    unsigned char *pSrcData;
    unsigned char *pDstData;

    if (0 == iSrcType) {
        /* Disk file */
        pSrcDiskFile = onsen_new_disk_file((const char *)szSrcFile, RDONLY, 0);
        if (NULL == pSrcDiskFile) {
            bError = 1;
        }
        pSrcData = pSrcDiskFile->pData;
    } else {
        /* Memory file */
        pSrcData = szSrcFile;
    }

    if (0 == iDstType) {
        /* Disk file */
        pDstDiskFile = onsen_new_disk_file((const char *)szDstFile, WRONLY, lDstFileSize);
        if (NULL == pDstDiskFile) {
            bError = 1;
        }
        pDstData = pDstDiskFile->pData;
    } else {
        /* Memory file */
        pDstData = szDstFile;
    }

    if (0 == bError) {
        if (NULL != pCallback) {
            pCallback(100, 0, pData);
        }
        /* TODO : do buffered copy if disk files aren't mmaped */
        memcpy(pDstData, pSrcData + lSrcOffset, lDstFileSize);
        if (NULL != pCallback) {
            pCallback(100, 100, pData);
        }
    }

    /* Close disk files */
    if (0 == iSrcType) {
        onsen_free_disk_file(pSrcDiskFile);
    }
    if (0 == iDstType) {
        onsen_free_disk_file(pDstDiskFile);
    }

    return bError;
}
