#ifndef NULLSOFT_RSSPARSEH
#define NULLSOFT_RSSPARSEH

#include "../xml/XMLNode.h"
#include "ChannelSync.h"

void ReadRSS(const XMLNode *rss, ChannelSync *sync, bool doWinLAMPSpecificTags, const wchar_t *url);

#endif