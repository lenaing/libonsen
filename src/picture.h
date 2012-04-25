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
#ifndef __ONSEN_PICTURE_H
#define __ONSEN_PICTURE_H

#include "utils.h"

typedef struct _OnsenDIBHeader_s OnsenDIBHeader_t;
struct _OnsenDIBHeader_s
{
    int32_t  iBmpWidth;
    int32_t  iBmpHeight;
    uint16_t iNbColorPlanes;
    uint16_t iNbBitsPerPixel;
    uint32_t iCompressionType;
    uint32_t iBmpSize;
    int32_t  iHorizontalRes;
    int32_t  iVerticalRes;
    uint32_t iNbColorsInColorPalette;
    uint32_t iNbImportantColors;
    /* TODO　Bitfields */
    /* TODO  Color spaces */
};

typedef struct _OnsenPicture_s OnsenPicture_t;
struct _OnsenPicture_s
{
    OnsenDIBHeader_t *pDIBHeader;
    char **a_cColorMap;
};

OnsenDIBHeader_t *onsen_new_dib_header();
void onsen_free_dib_header(OnsenDIBHeader_t *);
OnsenPicture_t *onsen_new_picture();
void onsen_free_picture(OnsenPicture_t *);

#endif /* __ONSEN_PICTURE_H */
