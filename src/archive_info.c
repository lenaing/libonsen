#include "archive_info.h"

OnsenArchiveInfo_t *
onsen_new_archive_info()
{
    OnsenArchiveInfo_t *pInfo;

    pInfo = onsen_malloc(sizeof(OnsenArchiveInfo_t));
    pInfo->lArchiveFileSize = 0;

    return pInfo;
}

void
onsen_free_archive_info(OnsenArchiveInfo_t *pInfo)
{
    int i;

    if (NULL != pInfo) {
        for (i = 0; i <= pInfo->iArchiveEntriesCount; i++) {
            onsen_free_archive_entry(pInfo->a_pArchiveEntries[i]);
        }
        onsen_free(pInfo->a_pArchiveEntries);
        pInfo->lArchiveFileSize = 0;
        onsen_free(pInfo);
    }
}
