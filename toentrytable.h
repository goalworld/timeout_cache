#ifndef _ENTRY_MAP_TIMEOUT_H_
#define _ENTRY_MAP_TIMEOUT_H_
struct ToEntryTable;

struct UserData{
	void* data;
	unsigned sz;
};
struct Entry{
	struct UserData data;
	int key;
	unsigned timeout;
};
enum  {
	TET_LIST,
	TET_HASH
};
struct ToEntryTable *TET_new( int type);
void TET_del(struct ToEntryTable *txt);
int TET_insertEntry(struct ToEntryTable* tet, int key,struct UserData data,unsigned  timeout);
int TET_removeEntry(struct ToEntryTable* tet ,int key,struct UserData *data);
int TET_queryEntry(struct ToEntryTable* tet ,int key,struct UserData *data);
int TET_onTimer(struct ToEntryTable *tet,unsigned times);
#endif
