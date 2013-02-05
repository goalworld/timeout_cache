#ifndef _ENTRY_MAP_TIMEOUT_H_
#define _ENTRY_MAP_TIMEOUT_H_
struct ToEntryTable;

struct UserData{
	void* data;
	unsigned sz;
};
struct ToEntryTable *TET_new();
void TET_del(struct ToEntryTable *txt);
int TET_insertEntry(struct ToEntryTable* tet, int key,struct UserData data,unsigned  timeout);
int TET_removeEntry(struct ToEntryTable* tet ,int key,struct UserData *data);
int TET_queryEntry(struct ToEntryTable* tet ,int key,struct UserData *data);
void TET_onTimer(struct ToEntryTable *tet,unsigned times);
#endif
