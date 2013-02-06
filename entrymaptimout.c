#include "entrymaptimeout.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#define HASH_SIZE 4 * 10240
struct ToCache
{
	void *(*New)();
	void (*Del)(void *);
	void *(*Insert)(void *list,int key,struct UserData data,unsigned timeout);
	void (*Remove)(void *list,void * litem);
	void(* OnTimer)(void *list,unsigned timeout,void(*cb)(void *arg,struct Entry try),void *arg);
};
struct ToCacheItem
{
	struct Entry (*GetEntryByItem)(void * item);
};
#define SET_TO_CACHE(tocache,type) \
				tocache->New = type##New;\
				tocache->Del = type##Del;\
				tocache->Insert = type##Insert;\
				tocache->Remove = type##Remove;\
				tocache->OnTimer = type##OnTimer;

#define SET_TO_CACHE_ITEM(tci,type) tci->GetEntryByItem = type##GetEntryByItem;\

struct HashItem
{
	void *host;
	struct HashItem *next;
};
int 
hash_func(int key)
{
	return key%HASH_SIZE;
}
struct HashMap
{
	struct ToCacheItem toCaIem;
	struct HashItem* items[HASH_SIZE];
};
struct ToEntryTable
{
	struct HashMap hmap;
	void * timeoutCache;
	struct ToCache toCahe;
};


static void hashInit(struct HashMap *hmap);
static void hashDestroy(struct HashMap *hmap);
static void hashInsert( struct HashMap *hmap,int key,void *item );
static void * hashQuery(struct HashMap *hmap, int key );
static void * hashRemove( struct HashMap *hmap,int key );


struct ToEntryTable * 
TET_new( int type)
{
	struct ToEntryTable *p = (struct ToEntryTable *)malloc(sizeof(struct ToEntryTable));
	if( initToCache(&p->toCahe,&p->hmap.toCaIem,type) != 0){
		free(p);
		return NULL;
	}
	p->timeoutCache = p->toCahe.New();
	hashInit(&p->hmap);
	return p;
}
void 
TET_del(struct ToEntryTable *txt)
{
	txt->toCahe.Del(txt->timeoutCache);
	hashDestroy(&txt->hmap);
	free(txt);
}
static void
notifyListItemRemove(void *arg ,struct Entry ety)
{
	struct ToEntryTable* tet =( struct ToEntryTable* )arg;
	hashRemove(&tet->hmap,ety.key);
}

void 
TET_onTimer(struct ToEntryTable *tet,unsigned times){
	tet->toCahe.OnTimer(tet->timeoutCache,times,notifyListItemRemove,tet);
}
int
TET_insertEntry(struct ToEntryTable* tet, int key,struct UserData data,unsigned timeout)
{
	if(hashQuery(&tet->hmap,key)){
		return -1;
	}
	void * item = tet->toCahe.Insert(tet->timeoutCache,key,data,timeout);
	hashInsert(&tet->hmap,key,item);
	return 0;
}
int
TET_removeEntry(struct ToEntryTable* tet ,int key,struct UserData *data)
{
	void * item = hashRemove(&tet->hmap,key);
	if(item){
		*data = tet->hmap.toCaIem.GetEntryByItem(item).data;
		tet->toCahe.Remove(tet->timeoutCache,item);
		return 0;
	}
	return -1;
}
int
TET_queryEntry(struct ToEntryTable* tet ,int key,struct UserData *data)
{
	void * item = hashQuery(&tet->hmap,key);
	if(item){
		*data = tet->hmap.toCaIem.GetEntryByItem(item).data;
		return 0;
	}
	return -1;
}



static void
hashInit(struct HashMap *hmap)
{
	memset(hmap->items,0,HASH_SIZE*sizeof(struct HashItem*));
}
static void 
hashDestroy(struct HashMap *hmap)
{
	int i = 0;
	struct HashItem *next,*cut;
	for( ;i<HASH_SIZE;i++){
		cut = hmap->items[i];
		while(cut){
			next = cut->next;
			free(cut);
			cut = next;
		}
	}
	
}
static void
hashInsert( struct HashMap *hmap,int key,void *item )
{
	int hash = hash_func(key);
	struct HashItem * hitem = (struct HashItem *)malloc(sizeof(struct HashItem));
	hitem->host = item;
	struct HashItem **pp = &hmap->items[hash];
	hitem->next = *pp;
	*pp = hitem;
}
static void *
hashQuery(struct HashMap *hmap, int key )
{
	int hash = hash_func(key);
	int retkey;
	struct HashItem *p = hmap->items[hash];
	while(p){
		retkey = hmap->toCaIem.GetEntryByItem(p->host).key;
		if(retkey == key){
			return p->host;
		}
		p = p->next;
	}
	return NULL;
}
static void *
hashRemove( struct HashMap *hmap,int key )
{
	int hash = hash_func(key);
	int retkey;
	struct HashItem *pre = NULL,*cut = hmap->items[hash];
	while(cut){
		retkey = hmap->toCaIem.GetEntryByItem(cut->host).key;
		if(retkey == key){
			if(pre){
				pre->next = cut->next;
			}else{
				hmap->items[hash] = cut->next;
			}
			struct ListItem * item = cut->host;
			free(cut);
			return item;
		}
		pre = cut;
		cut = cut->next;
	}
	return NULL;
}

#include "cache_list.c"
int 
initToCache( struct ToCache * p,struct ToCacheItem * pitem,int type)
{
	if(TET_LIST == type){
		SET_TO_CACHE(p,list);
		SET_TO_CACHE_ITEM(pitem,list);
		return 0;
	}
	return -1;

	
}
