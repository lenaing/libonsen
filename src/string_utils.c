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
onsen_basedir(char *szSource)
{
    char *szTemp;
    char *szBasedir;

    szTemp = onsen_strdup(szSource);
    szBasedir = onsen_strdup(dirname(szTemp));
    onsen_free(szTemp);

    return szBasedir;
}

char *
onsen_build_filename(const char *szPath, const char *szFilename)
{
    return onsen_build_filename_with_extension(szPath, szFilename, NULL);
}

char *
onsen_build_filename_with_extension(const char *szPath, const char *szFilename, 
                                    const char *szFileExtension)
{
    char *szBuiltFilename;
    int iPathLen;
    int iFilenameLen;
    int iFileExtensionLen;
    int iPadding;

    assert(NULL != szPath);
    assert(NULL != szFilename);

    iPadding = 0;
    iPathLen = strlen(szPath);
    iFilenameLen = strlen(szFilename);

    if (NULL != szFileExtension) {
        iFileExtensionLen = strlen(szFileExtension);
        iPadding++;
    } else {
        iFileExtensionLen = 0;
    }

    if (szPath[strlen(szPath)-1] != '/') {
        iPadding++;
    }

    szBuiltFilename = onsen_calloc(iPathLen + iPadding + iFilenameLen
                                   + iFileExtensionLen +1, sizeof(char));

    /* Copy path. */
    strncpy(szBuiltFilename, szPath, iPathLen);

    /* Add final slash to path. */
    if (szPath[strlen(szPath)-1] != '/') {
        strncat(szBuiltFilename, "/", 1);
    }

    /* Copy filename. */
    strncat(szBuiltFilename, szFilename, iFilenameLen);
    
    /* Copy '.' and extension if extension is provided. */
    if (NULL != szFileExtension) {
        strncat(szBuiltFilename, ".", 1);
        strncat(szBuiltFilename, szFileExtension, iFileExtensionLen);
    }

    /* Sanitize built string. */
    szBuiltFilename[iPathLen + iFilenameLen + iFileExtensionLen + iPadding] = 0;
    return szBuiltFilename;
}

char
*onsen_strdup(const char *szSource)
{
    char *szDestination;
    int iSourceLen;

    assert(NULL != szSource);

    iSourceLen = strlen(szSource) + 1;
    szDestination = onsen_calloc(iSourceLen, sizeof(char));
    strncpy(szDestination, szSource, iSourceLen);

    return szDestination;
}

void
onsen_str_chr_replace(char *szSource, const char cOld, const char cNew)
{
    char *pSource = szSource;

    while (*pSource) {
        if (*pSource == cOld) {
            *pSource = cNew;
        }
        ++pSource;
    }
}

int
onsen_str_is_slashed(const char *str)
{
    char *lastChar;
    unsigned int pos;

    lastChar = strrchr(str, '/');
    pos = (unsigned int)(lastChar - str + 1);
    if ((NULL == lastChar) || (pos != strlen(str))) {
        return 0;
    }
    return 1;
}




