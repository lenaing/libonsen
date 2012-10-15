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
#include "file_utils.h"

OnsenFile_t *
onsen_new_disk_file(const char *szFilename, OnsenFileMode eMode,
                        long lFileSize)
{
    OnsenFile_t *pDiskFile;
    int rc;
    int bError = 0;
    int bIsMmaped = 0;
    int iFd;
    int iMmapProto;
    unsigned char *pData = NULL;

    switch (eMode) {
        case ONSEN_WRITE_ONLY : {
            iFd = open(szFilename, O_RDWR|O_CREAT|O_TRUNC, 0644);
        } break;
        case ONSEN_READ_ONLY :
        default : {
            iFd = open(szFilename, O_RDONLY);
        } break;
    }

    if (-1 == iFd) {
        perror("open");
        onsen_err_ko("Failed to open file '%s'.\n", szFilename);
        return NULL;
    }

    switch (eMode) {
        case ONSEN_WRITE_ONLY : {
            rc = pwrite(iFd, "", 1, lFileSize - 1);
            if (-1 == rc) {
                perror("pwrite");
                onsen_err_ko("Failed to grow file '%s' to size %ld.\n",
                                szFilename,
                                lFileSize);
                bError = 1;
            }
        } break;
        case ONSEN_READ_ONLY :
        default : {
            lFileSize = lseek(iFd, 0, SEEK_END);
            if (-1 == lFileSize) {
                perror("lseek");
                onsen_err_ko("Failed to get file size of '%s'.\n", szFilename);
                bError = 1;
            }
        } break;
    }

    if (0 == bError) {
        if (lFileSize <= ONSEN_MAX_MMAPED_FILE_SIZE) {
            bIsMmaped = 1;
            iMmapProto = (eMode == ONSEN_WRITE_ONLY) ? PROT_WRITE : PROT_READ;
            pData = mmap(NULL, lFileSize, iMmapProto, MAP_SHARED, iFd, 0);
            if (MAP_FAILED == pData) {
                perror("mmap");
                onsen_err_ko("Failed to map file '%s' to memory.\n",
                                szFilename);
                bError = 1;
            }
        }
    }

    if (bError) {
        rc = close(iFd);
        if (-1 == rc) {
            perror("close");
            onsen_err_ko("Failed to close file '%s'.\n", szFilename);
        }
        return NULL;
    }

    pDiskFile = onsen_malloc(sizeof(OnsenFile_t));
    pDiskFile->szFilename = onsen_strdup(szFilename);
    pDiskFile->bIsMmaped  = bIsMmaped;
    pDiskFile->iFd        = iFd;
    pDiskFile->lFileSize  = lFileSize;
    pDiskFile->pData      = pData;
    return pDiskFile;
}

void
onsen_free_disk_file(OnsenFile_t *pDiskFile)
{
    int rc;
    if (NULL != pDiskFile) {

        if (NULL != pDiskFile->pData) {
            /* File was mmaped */
            rc = munmap(pDiskFile->pData, pDiskFile->lFileSize);
            if (-1 == rc) {
                perror("munmap");
                onsen_err_ko("Failed to unmap file '%s' from memory.\n",
                                pDiskFile->szFilename);
            }
        }

        rc = close(pDiskFile->iFd);
        if (-1 == rc) {
            perror("close");
            onsen_err_ko("Failed to close file '%s'.\n", pDiskFile->szFilename);
        }

        onsen_free(pDiskFile->szFilename);
        free(pDiskFile);
    }
}

int onsen_mkdir(const char *szPath)
{
    char aBuffer[256];
    char *p_aBuffer;
    char *szFailure = "Failed to create directory '%s'.";
    size_t len;
    int n;
    int rc = 0;

    assert(NULL != szPath);

    n = sizeof(aBuffer);
    strncpy(aBuffer, szPath, n);
    if (n > 0) {
        aBuffer[n - 1] = '\0';
    }
    len = strlen(aBuffer);

    if (aBuffer[len - 1] == '/') {
        aBuffer[len - 1] = '\0';
    }

    for (p_aBuffer = aBuffer; *p_aBuffer; p_aBuffer++) {
        if (*p_aBuffer == '/') {

            *p_aBuffer = '\0';

            if (strcmp("", aBuffer)) {
                rc = mkdir(aBuffer, S_IRWXU);
                if (-1 == rc && EEXIST != errno) {
                    perror("mkdir");
                    onsen_err_ko(szFailure, aBuffer);
                    return 0;
                }
            }

            *p_aBuffer = '/';
        }
    }

    rc = mkdir(aBuffer, S_IRWXU);
    if (-1 == rc && EEXIST != errno) {
        perror("mkdir");
        onsen_err_ko(szFailure, aBuffer);
        return 0;
    }

    return 1;
}
