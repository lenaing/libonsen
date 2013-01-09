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
#include "string_utils.h"

char *
onsen_build_filename(const char *path, const char *filename)
{
    assert(NULL != path);
    assert(NULL != filename);

    return onsen_build_filename_with_extension(path, filename, NULL);
}

char *
onsen_build_filename_with_extension(const char *path, const char *filename, 
                                    const char *fileExtension)
{
    char *builtFilename;
    int pathLen;
    int filenameLen;
    int fileExtensionLen;
    int padding;

    assert(NULL != path);
    assert(NULL != filename);

    padding = 0;
    pathLen = strlen(path);
    filenameLen = strlen(filename);

    if (NULL != fileExtension) {
        fileExtensionLen = strlen(fileExtension);
        padding++;
    } else {
        fileExtensionLen = 0;
    }

    if (path[strlen(path)-1] != '/') {
        padding++;
    }

    builtFilename = onsen_calloc(pathLen + padding + filenameLen
                                   + fileExtensionLen + 1, sizeof(char));

    /* Copy path. */
    strncpy(builtFilename, path, pathLen);

    /* Add final slash to path. */
    if (path[strlen(path)-1] != '/') {
        strncat(builtFilename, "/", 1);
    }

    /* Copy filename. */
    strncat(builtFilename, filename, filenameLen);
    
    /* Copy '.' and extension if extension is provided. */
    if (NULL != fileExtension) {
        strncat(builtFilename, ".", 1);
        strncat(builtFilename, fileExtension, fileExtensionLen);
    }

    /* Sanitize built string. */
    builtFilename[pathLen + filenameLen + fileExtensionLen + padding] = 0;
    return builtFilename;
}

char *
onsen_strdup(const char *source)
{
    char *destination;
    int sourceLen;

    assert(NULL != source);

    sourceLen = strlen(source) + 1;
    destination = onsen_calloc(sourceLen, sizeof(char));
    strncpy(destination, source, sourceLen);

    return destination;
}

void
onsen_str_chr_replace(char *source, const char old, const char new)
{
    char *pSource = source;

    assert(NULL != source);

    while (*pSource) {
        if (*pSource == old) {
            *pSource = new;
        }
        ++pSource;
    }
}

int
onsen_str_is_slashed(const char *str)
{
    char *lastChar;
    unsigned int pos;

    assert(NULL != str);

    lastChar = strrchr(str, '/');
    pos = (unsigned int)(lastChar - str + 1);
    if ((NULL == lastChar) || (pos != strlen(str))) {
        return 0;
    }
    return 1;
}
