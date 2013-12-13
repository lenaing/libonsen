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
#include "archive_plugin.h"

OnsenArchivePlugin_t *
onsen_new_archive_plugin()
{
    OnsenArchivePlugin_t *plugin;

    plugin = onsen_malloc(sizeof(OnsenArchivePlugin_t));
    plugin->getArchiveInfo = NULL;
    plugin->getFileInfo    = NULL;
    plugin->writeFile      = &onsen_write_file_raw;

    return plugin;
}

void
onsen_free_archive_plugin(OnsenArchivePlugin_t *plugin)
{
    assert(NULL != plugin);

    if (NULL != plugin) {
        onsen_free(plugin);
    }
}

int
onsen_archive_plugin_load_funcs(OnsenPlugin_t *plugin)
{
    OnsenArchivePlugin_t *instance;
    void *fun;

    assert(NULL != plugin);

    /* Library already loaded. */
    if (plugin->isLibraryLoaded) {
        onsen_err_warning("Plugin %s already loaded.", plugin->name);
        return 0;
    }

    instance = plugin->instance;
    if (NULL == instance) {
        return 1;
    }

    fun = dlsym(plugin->library, "onsen_get_archive_info");
    plugin->libraryError = dlerror();
    if (NULL != plugin->libraryError) {
        return 2;
    }
    memcpy(&(instance->getArchiveInfo), &fun,
                sizeof(instance->getArchiveInfo));

    fun = dlsym(plugin->library, "onsen_get_file_info");
    plugin->libraryError = dlerror();
    if (NULL != plugin->libraryError) {
        return 3;
    }
    memcpy(&(instance->getFileInfo), &fun, sizeof(instance->getFileInfo));

    /* Optional function */
    fun = dlsym(plugin->library, "onsen_write_file");
    plugin->libraryError = dlerror();
    if (NULL == plugin->libraryError) {
        memcpy(&(instance->writeFile), &fun, sizeof(instance->writeFile));
    }

    return 0;
}

int
onsen_write_file_raw(int srcType, void *srcFile, long srcOffset,
                 int dstType, void *dstFile, long dstFileSize,
                 OnsenWriteFileCallback callback, void *data)
{
    OnsenFile_t *dstDiskFile = NULL;
    int i = 0;
    int error = 1;
    int nbBytesRead = 0;
    int nbBytesToCopy = 0;
    int srcFd = 0;
    int rc;
    long offset;
    unsigned char *srcData = NULL;
    unsigned char *dstData = NULL;
    unsigned char buffer[ONSEN_IO_BUFFER_SIZE];

    assert(NULL != srcFile);
    assert(NULL != dstFile);

    if (ONSEN_DISK_FILE == srcType) {
        /* Disk file */
        srcFd = *(int *)srcFile;
    } else {
        /* Memory file */
        srcData = srcFile;
    }

    if (ONSEN_DISK_FILE == dstType) {
        /* Disk file */
        dstDiskFile = onsen_new_disk_file((const char *)dstFile,
                                                            ONSEN_WRITE_ONLY,
                                                            dstFileSize);
        if (NULL == dstDiskFile) {
            error = 0;
        } else {
            dstData = dstDiskFile->data;
        }
    } else {
        /* Memory file */
        dstData = dstFile;
    }

    if (1 == error) {

        if (NULL != callback) {
            callback(100, 0, data);
        }

        if (NULL != srcData && NULL != dstData) {
            /* Memory -> Memory */
            memcpy(dstData, srcData + srcOffset, dstFileSize);
        } else if (NULL == srcData && NULL == dstData) {
            /* Disk -> Disk */

            rc = lseek(srcFd, srcOffset, SEEK_SET);
            if (-1 == rc) {
                perror("lseek");
                onsen_err_ko("Seek failed in source file.");
                error = 0;
            }

            rc = lseek(dstDiskFile->fd, 0, SEEK_SET);
            if (-1 == rc) {
                perror("lseek");
                onsen_err_ko("Seek failed in destination file.");
                error = 0;
            }

            if (1 == error) {
                offset = 0;
                while ((nbBytesRead = read(srcFd,
                                            &buffer,
                                            ONSEN_IO_BUFFER_SIZE)) > 0) {

                    nbBytesToCopy = (dstFileSize < (offset + nbBytesRead))
                                        ? dstFileSize - offset
                                        : nbBytesRead;

                    rc = write(dstDiskFile->fd, &buffer, nbBytesToCopy);
                    if (-1 == rc) {
                        perror("write");
                        onsen_err_ko("Write failed to destination file.");
                        error = 0;
                        break;
                    }

                    offset += nbBytesRead;
                    if (offset > dstFileSize) {
                        break;
                    }

                    if (NULL != callback) {
                        callback(dstFileSize, offset, data);
                    }
                }

                if (-1 == nbBytesRead) {
                    perror("read");
                    onsen_err_ko("Read failed from source file.");
                    error = 0;
                }
            }
        } else if (NULL == srcData) {
            /* Disk -> Memory */

            rc = lseek(srcFd, srcOffset, SEEK_SET);
            if (-1 == rc) {
                perror("lseek");
                onsen_err_ko("Seek failed in source file.");
                error = 0;
            }

            if (1 == error) {
                offset = 0;
                while ((nbBytesRead = read(srcFd,
                                            &buffer,
                                            ONSEN_IO_BUFFER_SIZE)) > 0) {

                    nbBytesToCopy = (dstFileSize < (offset + nbBytesRead))
                                        ? dstFileSize - offset
                                        : nbBytesRead;

                    memcpy(dstData + offset, &buffer, nbBytesToCopy);

                    offset += nbBytesRead;
                    if (offset > dstFileSize) {
                        break;
                    }

                    if (NULL != callback) {
                        callback(dstFileSize, offset, data);
                    }
                }

                if (-1 == nbBytesRead) {
                    perror("read");
                    onsen_err_ko("Read failed from source file.");
                    error = 0;
                }
            }
        } else if (NULL == dstData) {
            /* Memory -> Disk */

            i = 0;
            while (i < dstFileSize) {
                if (NULL != callback) {
                    callback(dstFileSize, i, data);
                }

                nbBytesToCopy = (dstFileSize < (i + ONSEN_IO_BUFFER_SIZE))
                                    ? dstFileSize - i
                                    : ONSEN_IO_BUFFER_SIZE;

                rc = write(dstDiskFile->fd,
                                srcData + srcOffset + i,
                                nbBytesToCopy);
                if (-1 == rc) {
                    perror("write");
                    onsen_err_ko("Write failed to destination file");
                    error = 0;
                    break;
                }

                i += ONSEN_IO_BUFFER_SIZE;
            };
        }

        if (NULL != callback) {
            callback(100, 100, data);
        }
    }

    /* Close destination disk file */
    if (ONSEN_DISK_FILE == dstType) {
        onsen_free_disk_file(dstDiskFile);
    }

    return error;
}
