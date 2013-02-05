#ifndef _ENTRY_MAP_TIMEOUT_H_
#define _ENTRY_MAP_TIMEOUT_H_
struct ToEntryTable;
struct ToEntryTable *ToEnterTableNew();
int  insertEntry(struct ToEntryTable* tet, int key,void* data,unsigned  timeout);
void *removeEntry(struct ToEntryTable* tet ,int key);
void * queryEntry(struct ToEntryTable* tet ,int key);
#endif
