#pragma once
#include "..\..\General\gen_ml\ml.h"

extern winlampMediaLibraryPlugin plugin;

void ConvertItemRecordListToISO(const itemRecordList *list);
void ConvertFilenamesToISO(const char *filenames);
void ConvertPlaylistToISO(const mlPlaylist *playlist);
void ConvertPlaylistsToISO(const mlPlaylist **playlist);
void ConvertUnicodeItemRecordListToISO(const itemRecordListW *list);
void ConvertUnicodeFilenamesToISO(const wchar_t *filenames);

