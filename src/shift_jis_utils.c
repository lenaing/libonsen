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
#include "shift_jis_utils.h"

int
onsen_are_shift_jis_bytes(unsigned char cFrst, unsigned char cScnd)
{
    /* JIS X 0208:1997,
     * 
     * Two bytes char :
     * - First char : 0x81 to 0x9f and 0xe0 to 0xef
     * - Second char : 0x40 to 0x7e and 0x80 to 0xfc
     * 
     * Micro$oft code page.
     */
    if ((cFrst >= 0x81 && cFrst <= 0x9f) || (cFrst >= 0xe0 && cFrst <= 0xef)) {
        return (((cScnd >= 0x40) && (cScnd <= 0x7e))
                || ((cScnd >= 0x80) && (cScnd <= 0xfc)));
    }

    /* JIS X 0201:1997, Half kana (0xa1 to 0xdf) */
    if ((cFrst >= 0xa1 && cFrst <= 0xdf) || (cScnd >= 0xa1 && cScnd <=0xdf)) {
        return 1;
    }

    return 0;
}

int
onsen_is_shift_jis(const unsigned char *szMaybeShiftJIS)
{
    int iStatus;
    int iCount;
    int iLen;
    unsigned char cFirst; 
    unsigned char cSecond; 

    assert(NULL != szMaybeShiftJIS);

    iStatus = 1;
    iCount = 0;
    iLen = strlen((char *)szMaybeShiftJIS);

    do {
        cFirst = szMaybeShiftJIS[iCount];
        cSecond = szMaybeShiftJIS[iCount + 1];
        iStatus = onsen_are_shift_jis_bytes(cFirst, cSecond);
        iCount++;
    } while (!iStatus && (iCount < iLen));

    return iStatus;
}

char *
onsen_shift_jis2utf8 (iconv_t pIconv, unsigned char *szShiftJIS)
{
    size_t rc;
    size_t iLen;
    size_t iUTF8len;
    char *szUTF8tmp;
    char *szUTF8;
    
    assert(NULL != pIconv);
    assert(NULL != szShiftJIS);

    iLen = strlen((char *)szShiftJIS);
    if (!iLen) {
        onsen_err_warning("Iconv: Input string is empty.");
        return '\0';
    }

    /* Assign enough space to put the UTF-8 string. */
    iUTF8len = 2*iLen;
    szUTF8tmp = onsen_calloc(iUTF8len, 1);
    szUTF8 = szUTF8tmp;

    rc = iconv(pIconv, (char **)&szShiftJIS, &iLen, &szUTF8tmp, &iUTF8len);
    if (-1 == (int)rc) {
        onsen_err_ko("Iconv: Input: '%s', length %d. Output: '%s', length %d.",
              szShiftJIS, iLen, szUTF8, iUTF8len);
        switch (errno) {
            case EILSEQ:
                onsen_err_ko("Invalid multibyte sequence encountered.");
                break;
            case EINVAL:
                onsen_err_ko("Incomplete multibyte sequence encountered.");
                break;
            case E2BIG:
                onsen_err_ko("No more room in output buffer.");
                break;
            default:
                onsen_err_ko("Error: %s.", strerror(errno));
        }
        memcpy(szUTF8, "?", iLen);
    }

    return szUTF8;
}
