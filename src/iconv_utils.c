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
#include "iconv_utils.h"

iconv_t
onsen_iconv_init(const char * szOutCharset, const char * szInCharset)
{
    iconv_t pIconv;

    assert(NULL != szOutCharset);
    assert(NULL != szInCharset);

    pIconv = iconv_open(szOutCharset, szInCharset);
    if (-1 == (intptr_t)pIconv) {
        if (EINVAL == errno) {
            onsen_err_ko("Conversion from '%s' to '%s' is not supported.",
                  szOutCharset, szInCharset);
        } else {
            onsen_err_ko("Iconv initialization failure: %s.", strerror(errno));
        }
        return NULL;
    }

    return pIconv;
}

int
onsen_iconv_cleanup(iconv_t pIconv)
{
    int rc;

    assert(NULL != pIconv);

    rc = iconv_close(pIconv);
    if (rc) {
        onsen_err_ko("Iconv closing failure");
        perror("iconv_close");
        return rc;
    }

    return rc;
}
