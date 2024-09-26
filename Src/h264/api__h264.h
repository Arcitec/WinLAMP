#ifndef NULLSOFT_APIH
#define NULLSOFT_APIH

#include <api/service/api_service.h>

#include <api/memmgr/api_memmgr.h>
extern api_memmgr *memmgrApi;
#define WASABI_API_MEMMGR memmgrApi

#include "../winlamp/api_winlamp.h"
extern api_winlamp *winlampApi;
#define AGAVE_API_WINLAMP winlampApi
#endif