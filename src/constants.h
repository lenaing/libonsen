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
#ifndef __ONSEN_CONSTANTS_H
#define __ONSEN_CONSTANTS_H

#define ONSEN_API_MAJOR '0'
#define ONSEN_API_MINOR '1'

#define ONSEN_PLUGIN_UNSUPPORTED        0
#define ONSEN_PLUGIN_ARCHIVE            'A'
#define ONSEN_PLUGIN_PICTURE_IMPORTER   'I'
#define ONSEN_PLUGIN_PICTURE_EXPORTER   'X'

#define ONSEN_PLUGIN_NAME_SIZE      65
#define ONSEN_PLUGIN_VERSION_SIZE   65
#define ONSEN_PLUGIN_AUTHORS_SIZE   65

#define ONSEN_MAX_MMAPED_FILE_SIZE 104857600  /* 100 Mo */
#define ONSEN_IO_BUFFER_SIZE 4096

enum _OnsenLibraryError_e {
    ONSEN_E_LIB_OPEN = 1,           /* Failed to open library.                */
    ONSEN_E_LIB_GET_PLUGIN_INFO,    /* Failed to find onsen_get_plugin_info 
                                       function in library.                   */
    ONSEN_E_LIB_IS_FILE_SUPPORTED,  /* Failed to find onsen_is_file_supported 
                                       function in library.                   */
    ONSEN_E_WRONG_API,              /* Library uses a wrong or unknown API
                                       version.                               */
    ONSEN_E_PLUGIN_TYPE,            /* Library has an invalid plugin type.    */
    ONSEN_E_PLUGIN_METADATA,        /* Failed to read plugin metadata from
                                       library.                               */
    ONSEN_E_PLUGIN_FUNCTIONS,       /* Failed to find mandatory functions in 
                                       library.                               */
    ONSEN_E_PLUGIN_RELOAD           /* Failed to reload plugin. */
};
typedef enum _OnsenLibraryError_e OnsenLibraryError;

enum _OnsenFileMode_e {
    ONSEN_READ_ONLY,
    ONSEN_WRITE_ONLY
};
typedef enum _OnsenFileMode_e OnsenFileMode;

enum _OnsenFileType_e {
    ONSEN_DISK_FILE,
    ONSEN_MEMORY_FILE
};
typedef enum _OnsenFileType_e OnsenFileType;

#endif /* __ONSEN_CONSTANTS_H */
