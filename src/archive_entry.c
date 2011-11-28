/*
 * Copyright 2011 - Etienne 'lenaing' GIRONDEL <lenaing@gmail.com>
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
#include "archive_entry.h"

OnsenArchiveEntry_t *
onsen_new_archive_entry()
{
    OnsenArchiveEntry_t *pEntry;

    pEntry = onsen_malloc(sizeof(OnsenArchiveEntry_t));
    pEntry->szFilename      = NULL;
    pEntry->iOffset         = 0;
    pEntry->iSize           = 0;
    pEntry->iCompressedSize = 0;
    pEntry->bEncrypted      = 0;
    pEntry->bCompressed     = 0;
    pEntry->a_szAddlFds     = NULL;
    pEntry->iAddlFdsCount   = 0;

    return pEntry;
}

void
onsen_free_archive_entry(OnsenArchiveEntry_t *pEntry)
{
    int i;

    if (NULL != pEntry) {

        if (NULL != pEntry->szFilename) {
            onsen_free(pEntry->szFilename);
        }

        if (NULL != pEntry->a_szAddlFds) {
            for (i = 0; i < pEntry->iAddlFdsCount; i++) {
                if (NULL != pEntry->a_szAddlFds[i]) {
                    onsen_free(pEntry->a_szAddlFds[i]);
                }
            }
            onsen_free(pEntry->a_szAddlFds);
        }

        onsen_free(pEntry);

    }
}
