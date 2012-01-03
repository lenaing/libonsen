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

FILE *
onsen_open_file(const char *szFilename, const char *szMode)
{
    FILE *pFile;

    assert(NULL != szFilename);
    assert(NULL != szMode);

    if (NULL == szFilename) {
        onsen_err_ko("Attempted to open a file with a NULL filename.");
        return NULL;
    }

    if (NULL == szMode) {
        onsen_err_ko("Attempted to open a file with a NULL mode.");
        return NULL;
    }

    pFile = fopen(szFilename, szMode);

    if (NULL == pFile) {
        onsen_err_ko("Failed to open '%s'.", szFilename);
        perror("fopen");
    }

    return pFile;
}

int
onsen_close_file(FILE *pFile)
{
    int rc = 0;

    assert(NULL != pFile);

    if (NULL == pFile) {
        onsen_err_ko("Attempted to close an invalid file pointer.");
        return -1;
    }

    rc = fclose(pFile);
    if (EOF == rc) {
        onsen_err_ko("Failed to close file.");
        perror("fclose");
    }

    return rc;
}

char
onsen_file_read_byte(FILE *pFile)
{
    char c;
    size_t rc;

    assert(NULL != pFile);

    if (NULL == pFile) {
        onsen_err_ko("Attempted to read a byte through invalid file pointer.");
    }

    rc = fread(&c, sizeof(char), 1, pFile);
    if (rc != 1) {
        onsen_err_warning("Failed to read a byte in file.");
        perror("fread");
    }

    return c;
}

int
onsen_file_read_int(FILE *pFile)
{
    int i;
    size_t rc;

    assert(NULL != pFile);

    if (NULL == pFile) {
        onsen_err_ko("Attempted to read an int through invalid file pointer.");
    }

    rc = fread(&i, sizeof(int), 1, pFile);
    if (rc != 1) {
        onsen_err_warning("Failed to read an int in file.");
        perror("fread");
    }

    return i;
}

short
onsen_file_read_short(FILE *pFile)
{
    short s;
    size_t rc;

    assert(NULL != pFile);

    if (NULL == pFile) {
        onsen_err_ko("Attempted to read a short through invalid file pointer.");
    }

    rc = fread(&s, sizeof(short), 1, pFile);
    if (rc != 1) {
        onsen_err_warning("Failed to read a short in file.");
        perror("fread");
    }

    return s;
}

long
onsen_file_read_long(FILE *pFile)
{
    long l;
    size_t rc;

    assert(NULL != pFile);

    if (NULL == pFile) {
        onsen_err_ko("Attempted to read a long through invalid file pointer.");
    }

    rc = fread(&l, sizeof(long), 1, pFile);
    if (rc != 1) {
        onsen_err_warning("Failed to read a long in file.");
        perror("fread");
    }

    return l;
}


long
onsen_get_file_size(FILE *pFile)
{
    long lSize;
    int rc = 0;

    assert(NULL != pFile);

    if (NULL == pFile) {
        onsen_err_ko("Attempted to get the size through invalid file pointer.");
        return -1;
    }

    rc = fseek(pFile, 0, SEEK_END);
    if (0 != rc) {
        onsen_err_ko("Failed to seek to end of file.");
        perror("fseek");
        return -1;
    }

    lSize = ftell(pFile);
    if (-1 == lSize) {
        onsen_err_ko("Failed to get file size.");
        perror("ftell");
        return -1;
    }

    return lSize;
}

void
onsen_file_rewind(FILE *pFile)
{
    rewind(pFile);
}

int
onsen_file_goto(FILE *pFile, int iPos)
{
    int rc = 0;

    assert(NULL != pFile);
    assert(0 <= iPos);

    if (NULL == pFile) {
        onsen_err_ko("Attempted to seek through invalid file pointer.");
        return 1;
    }

    if (0 > iPos) {
        onsen_err_ko("Attempted to seek to a negative position.");
        return 1;
    }

    rc = fseek(pFile, iPos, SEEK_SET);
    if (0 != rc) {
        onsen_err_warning("Failed to seek to pos %d (0x%04X) in file.", iPos);
        perror("fseek");
    }

    return rc;
}

int
onsen_file_skip(FILE *pFile, int iCount)
{
    int rc = 0;

    assert(NULL != pFile);
    assert(0 < iCount);

    if (NULL == pFile) {
        onsen_err_ko("Attempted to skip bytes through invalid file pointer.");
        return 1;
    }

    if (0 < iCount) {
        onsen_err_ko("Attempted to skip a negative amount of bytes.");
        return 1;
    }

    rc = fseek(pFile, iCount, SEEK_CUR);
    if (0 != rc) {
        onsen_err_warning("Failed to skip %d bytes.", iCount);
        perror("fseek");
    }

    return rc;
}

int onsen_mkdir(const char *szPath)
{
    char aBuffer[256];
    char *p_aBuffer;
    size_t len;
    int rc = 0;

    strncpy(aBuffer, szPath, sizeof(aBuffer));
    len = strlen(aBuffer);

    if(aBuffer[len - 1] == '/') {
        aBuffer[len - 1] = '\0';
    }

    for(p_aBuffer = aBuffer; *p_aBuffer; p_aBuffer++) {
        if(*p_aBuffer == '/') {
            *p_aBuffer = '\0';
            if(strcmp("", aBuffer) && access(aBuffer, F_OK)) {
                rc = mkdir(aBuffer, S_IRWXU);
                if (0 != rc) {
                    onsen_err_ko("Failed to create directory '%s'.", aBuffer);
                    perror("mkdir");
                }
            }
            *p_aBuffer = '/';
        }
    }

    if(access(aBuffer, F_OK)) {
        rc = mkdir(aBuffer, S_IRWXU);
        if (0 != rc) {
            onsen_err_ko("Failed to create directory '%s'.", aBuffer);
            perror("mkdir");
        }
    }

    return rc;
}
