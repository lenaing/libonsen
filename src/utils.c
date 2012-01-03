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
#include "utils.h"

void
onsen_out_ok(const char *szWhat, ...)
{
    va_list ap;

    fprintf(stdout, "[+] ");
    va_start(ap, szWhat);
    vfprintf(stdout, szWhat, ap);
    va_end(ap);
    fprintf(stdout,"\n");
}

void
onsen_err_ko(const char *szWhat, ...)
{
    va_list ap;

    fprintf(stderr, "[!] ");
    va_start(ap, szWhat);
    vfprintf(stderr, szWhat, ap);
    va_end(ap);
    fprintf(stderr,"\n");
}

void
onsen_err_warning(const char *szWhat, ...)
{
    va_list ap;

    fprintf(stderr, "[?] ");
    va_start(ap, szWhat);
    vfprintf(stderr, szWhat, ap);
    va_end(ap);
    fprintf(stderr,"\n");
}

void
onsen_err_critical(const char *szWhat, ...)
{
    va_list ap;

    fprintf(stderr, "[X] ");
    va_start(ap, szWhat);
    vfprintf(stderr, szWhat, ap);
    va_end(ap);
    fprintf(stderr,"\n[X_X] You divided by zero? OH SHI-\n");
    exit(EXIT_FAILURE);
}

void *
onsen_malloc(const size_t iSize)
{
    void *ptr;

    ptr = malloc(iSize);
    if (NULL == ptr) {
        onsen_err_critical("malloc failed.");
        perror("malloc");
    }

    return ptr;
}

void *
onsen_calloc(const size_t iCount, const size_t iSize)
{
    void *ptr;

    ptr = calloc(iCount, iSize);
    if (NULL == ptr) {
        onsen_err_critical("calloc failed.");
        perror("calloc");
    }

    return ptr;
}

void *
onsen_realloc(void *ptr, size_t iSize)
{
    void *nptr;

    if (!ptr) {
        nptr = onsen_malloc(iSize);
    } else {
        nptr = realloc(ptr, iSize);
    }

    if (NULL == nptr) {
        onsen_err_critical("realloc failed.");
        perror("realloc");
    }

    return nptr;
}
