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
    pPlugin->getFileInfo    = NULL;
    pPlugin->writeFile      = &onsen_write_file_raw;

    return pPlugin;
}

void
onsen_free_archive_plugin(OnsenArchivePlugin_t *pPlugin)
{
    assert(NULL != pPlugin);

    if (NULL != pPlugin) {
        onsen_free(pPlugin);
    }
}

int
onsen_archive_plugin_load_funcs(OnsenPlugin_t *pPlugin)
{
    OnsenArchivePlugin_t *pInstance;
    void *pFun;

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
    OnsenFile_t *pDstDiskFile;
    int i = 0;
    int bError = 1;
    int iNbBytesRead = 0;
    int iNbBytesToCopy = 0;
    int iSrcFd;
    int rc;
    long lOffset;
    unsigned char *pSrcData = NULL;
    unsigned char *pDstData = NULL;
    unsigned char aBuffer[ONSEN_IO_BUFFER_SIZE];

    assert(NULL != szSrcFile);
    assert(NULL != szDstFile);

    if (ONSEN_DISK_FILE == iSrcType) {
        /* Disk file */
        iSrcFd = *(int *)szSrcFile;
    } else {
        /* Memory file */
        pSrcData = szSrcFile;
    }

    if (ONSEN_DISK_FILE == iDstType) {
        /* Disk file */
        pDstDiskFile = onsen_new_disk_file((const char *)szDstFile,
                                                            ONSEN_WRITE_ONLY,
                                                            lDstFileSize);
        if (NULL == pDstDiskFile) {
            bError = 0;
        } else {
            pDstData = pDstDiskFile->pData;
        }
    } else {
        /* Memory file */
        pDstData = szDstFile;
    }

    if (1 == bError) {

        if (NULL != pCallback) {
            pCallback(100, 0, pData);
        }

        if (NULL != pSrcData && NULL != pDstData) {
            /* Memory -> Memory */
            memcpy(pDstData, pSrcData + lSrcOffset, lDstFileSize);
        } else if (NULL == pSrcData && NULL == pDstData) {
            /* Disk -> Disk */

            rc = lseek(iSrcFd, lSrcOffset, SEEK_SET);
            if (-1 == rc) {
                perror("lseek");
                onsen_err_ko("Seek failed in source file.");
                bError = 0;
            }

            rc = lseek(pDstDiskFile->iFd, 0, SEEK_SET);
            if (-1 == rc) {
                perror("lseek");
                onsen_err_ko("Seek failed in destination file.");
                bError = 0;
            }

            if (1 == bError) {
                lOffset = 0;
                while ((iNbBytesRead = read(iSrcFd,
                                            &aBuffer,
                                            ONSEN_IO_BUFFER_SIZE)) > 0) {

                    iNbBytesToCopy = (lDstFileSize < (lOffset + iNbBytesRead))
                                        ? lDstFileSize - lOffset
                                        : iNbBytesRead;

                    rc = write(pDstDiskFile->iFd, &aBuffer, iNbBytesToCopy);
                    if (-1 == rc) {
                        perror("write");
                        onsen_err_ko("Write failed to destination file.");
                        bError = 0;
                        break;
                    }

                    lOffset += iNbBytesRead;
                    if (lOffset > lDstFileSize) {
                        break;
                    }

                    if (NULL != pCallback) {
                        pCallback(lDstFileSize, lOffset, pData);
                    }
                }

                if (-1 == iNbBytesRead) {
                    perror("read");
                    onsen_err_ko("Read failed from source file.");
                    bError = 0;
                }
            }
        } else if (NULL == pSrcData) {
            /* Disk -> Memory */

            rc = lseek(iSrcFd, lSrcOffset, SEEK_SET);
            if (-1 == rc) {
                perror("lseek");
                onsen_err_ko("Seek failed in source file.");
                bError = 0;
            }

            if (1 == bError) {
                lOffset = 0;
                while ((iNbBytesRead = read(iSrcFd,
                                            &aBuffer,
                                            ONSEN_IO_BUFFER_SIZE)) > 0) {

                    iNbBytesToCopy = (lDstFileSize < (lOffset + iNbBytesRead))
                                        ? lDstFileSize - lOffset
                                        : iNbBytesRead;

                    memcpy(pDstData + lOffset, &aBuffer, iNbBytesToCopy);

                    lOffset += iNbBytesRead;
                    if (lOffset > lDstFileSize) {
                        break;
                    }

                    if (NULL != pCallback) {
                        pCallback(lDstFileSize, lOffset, pData);
                    }
                }

                if (-1 == iNbBytesRead) {
                    perror("read");
                    onsen_err_ko("Read failed from source file.");
                    bError = 0;
                }
            }
        } else if (NULL == pDstData) {
            /* Memory -> Disk */

            i = 0;
            while (i < lDstFileSize) {
                if (NULL != pCallback) {
                    pCallback(lDstFileSize, i, pData);
                }

                iNbBytesToCopy = (lDstFileSize < (i + ONSEN_IO_BUFFER_SIZE))
                                    ? lDstFileSize - i
                                    : ONSEN_IO_BUFFER_SIZE;

                rc = write(pDstDiskFile->iFd,
                                pSrcData + lSrcOffset + i,
                                iNbBytesToCopy);
                if (-1 == rc) {
                    perror("write");
                    onsen_err_ko("Write failed to destination file");
                    bError = 0;
                    break;
                }

                i += ONSEN_IO_BUFFER_SIZE;
            };
        }

        if (NULL != pCallback) {
            pCallback(100, 100, pData);
        }
    }

    /* Close destination disk file */
    if (ONSEN_DISK_FILE == iDstType) {
        onsen_free_disk_file(pDstDiskFile);
    }

    return bError;
}
