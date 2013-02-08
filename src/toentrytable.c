#include "toentrytable.h"
#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#define HASH_SIZE 10* 10240
typedef void(*remove_cb)(void *arg,struct Entry try) ;
struct ToCache
{
	void *(*New)();
	void (*Del)(void *);
	void *(*Insert)(void *list,unsigned key,struct UserData data,unsigned timeout);
	void (*Remove)(void *list,void * litem);
	int(* OnTimer)(void *list,unsigned timeout,remove_cb cb,void *arg);
};
#define SET_TO_CACHE(tocache,type) \
				tocache->New = type##New;\
				tocache->Del = type##Del;\
				tocache->Insert = type##Insert;\
				tocache->Remove = type##Remove;\
				tocache->OnTimer = type##OnTimer;



struct ToEntryTable
{
	struct wcHashMap *hmap;
	void * timeoutCache;
	struct ToCache toCahe;
};
unsigned 	hashFunc(void *env,const void *key)
{
	return (unsigned)key;
}

struct ToEntryTable * 
TET_new( int type)
{
	struct ToEntryTable *p = (struct ToEntryTable *)malloc(sizeof(struct ToEntryTable));
	if( initToCache(&p->toCahe,type) != 0){
		free(p);
		return NULL;
	}
	p->timeoutCache = p->toCahe.New();
	struct wcHashMapType whmt;
	memset(&whmt,0,sizeof(whmt));
	whmt.hashFunc = hashFunc;
	p->hmap = wcHashMapNew(whmt,NULL);
	return p;
}
void 
TET_del(struct ToEntryTable *txt)
{
	txt->toCahe.Del(txt->timeoutCache);
	wcHashMapDelete(txt->hmap);
	free(txt);
}
static void
notifyListItemRemove(void *arg ,struct Entry ety)
{
	struct ToEntryTable* tet =( struct ToEntryTable* )arg;
	unsigned key = ety.key;
	wcHashMapRemove(tet->hmap,(void *)key);
}

int 
TET_onTimer(struct ToEntryTable *tet,unsigned times){
	return tet->toCahe.OnTimer(tet->timeoutCache,times,notifyListItemRemove,tet);
}
int
TET_insertEntry(struct ToEntryTable* tet, unsigned key,struct UserData data,unsigned timeout)
{
	void * item = tet->toCahe.Insert(tet->timeoutCache,key,data,timeout);
	wcHashMapInsert(tet->hmap,(void *)key,item);
	return 0;
}
int
TET_removeEntry(struct ToEntryTable* tet ,unsigned key,struct UserData *data)
{
	void * item = wcHashMapRemove(tet->hmap,(void *)key);
	if(item){
		*data = ((struct Entry *)item)->data;
		tet->toCahe.Remove(tet->timeoutCache,item);
		return 0;
	}
	return -1;
}
int
TET_queryEntry(struct ToEntryTable* tet ,unsigned key,struct UserData *data)
{
	void * item = wcHashMapQuery(tet->hmap,(void *)key);
	if(item){
		*data = ((struct Entry *)item)->data;
		return 0;
	}
	return -1;
}
#include "cache_list.c"
#include "cache_hash.c"
int 
initToCache( struct ToCache * p,int type)
{
	if(TET_LIST == type){
		SET_TO_CACHE(p,list);
		return 0;
	}else if(TET_HASH == type){
		SET_TO_CACHE(p,hashList);
		return 0;
	}
	return -1;
}
