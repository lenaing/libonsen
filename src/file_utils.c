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
#include "file_utils.h"

OnsenFile_t *
onsen_new_disk_file(const char *filename, OnsenFileMode mode,
                        long fileSize)
{
    OnsenFile_t *diskFile;
    int rc;
    int error = 0;
    int isMmaped = 0;
    int fd;
    int mmapProto;
    unsigned char *data = NULL;

    switch (mode) {
        case ONSEN_WRITE_ONLY : {
            fd = open(filename, O_RDWR|O_CREAT|O_TRUNC, 0644);
        } break;
        case ONSEN_READ_ONLY :
        default : {
            fd = open(filename, O_RDONLY);
        } break;
    }

    if (-1 == fd) {
        perror("open");
        onsen_err_ko("Failed to open file '%s'.\n", filename);
        return NULL;
    }

    switch (mode) {
        case ONSEN_WRITE_ONLY : {
            rc = pwrite(fd, "", 1, fileSize - 1);
            if (-1 == rc) {
                perror("pwrite");
                onsen_err_ko("Failed to grow file '%s' to size %ld.\n",
                                filename,
                                fileSize);
                error = 1;
            }
        } break;
        case ONSEN_READ_ONLY :
        default : {
            fileSize = lseek(fd, 0, SEEK_END);
            if (-1 == fileSize) {
                perror("lseek");
                onsen_err_ko("Failed to get file size of '%s'.\n", filename);
                error = 1;
            }
        } break;
    }

    if (0 == error) {
        if (fileSize <= ONSEN_MAX_MMAPED_FILE_SIZE) {
            isMmaped = 1;
            mmapProto = (mode == ONSEN_WRITE_ONLY) ? PROT_WRITE : PROT_READ;
            data = mmap(NULL, fileSize, mmapProto, MAP_SHARED, fd, 0);
            if (MAP_FAILED == data) {
                perror("mmap");
                onsen_err_ko("Failed to map file '%s' to memory.\n", filename);
                error = 1;
            }
        }
    }

    if (error) {
        rc = close(fd);
        if (-1 == rc) {
            perror("close");
            onsen_err_ko("Failed to close file '%s'.\n", filename);
        }
        return NULL;
    }

    diskFile = onsen_malloc(sizeof(OnsenFile_t));
    diskFile->filename = onsen_strdup(filename);
    diskFile->isMmaped = isMmaped;
    diskFile->fd       = fd;
    diskFile->fileSize = fileSize;
    diskFile->data     = data;
    return diskFile;
}

void
onsen_free_disk_file(OnsenFile_t *diskFile)
{
    int rc;
    if (NULL != diskFile) {

        if (NULL != diskFile->data) {
            /* File was mmaped */
            rc = munmap(diskFile->data, diskFile->fileSize);
            if (-1 == rc) {
                perror("munmap");
                onsen_err_ko("Failed to unmap file '%s' from memory.\n",
                                diskFile->filename);
            }
        }

        rc = close(diskFile->fd);
        if (-1 == rc) {
            perror("close");
            onsen_err_ko("Failed to close file '%s'.\n", diskFile->filename);
        }

        onsen_free(diskFile->filename);
        onsen_free(diskFile);
    }
}

int onsen_mkdir(const char *path)
{
    char buffer[256];
    char *pBuffer;
    char *failure = "Failed to create directory '%s'.";
    size_t len;
    int n;
    int rc = 0;

    assert(NULL != path);

    n = sizeof(buffer);
    strncpy(buffer, path, n);
    if (n > 0) {
        buffer[n - 1] = '\0';
    }
    len = strlen(buffer);

    if (buffer[len - 1] == '/') {
        buffer[len - 1] = '\0';
    }

    for (pBuffer = buffer; *pBuffer; pBuffer++) {
        if (*pBuffer == '/') {

            *pBuffer = '\0';

            if (strcmp("", buffer)) {
                rc = mkdir(buffer, S_IRWXU);
                if (-1 == rc && EEXIST != errno) {
                    perror("mkdir");
                    onsen_err_ko(failure, buffer);
                    return 0;
                }
            }

            *pBuffer = '/';
        }
    }

    rc = mkdir(buffer, S_IRWXU);
    if (-1 == rc && EEXIST != errno) {
        perror("mkdir");
        onsen_err_ko(failure, buffer);
        return 0;
    }

    return 1;
}
