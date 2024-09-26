#pragma once

#include <api/service/api_service.h>
extern api_service *serviceManager;
#define WASABI_API_SVC serviceManager

#include "../winlamp/api_winlamp.h"
extern api_winlamp *winlampApi;
#define AGAVE_API_WINLAMP winlampApi