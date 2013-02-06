#include "entrymaptimeout.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#define HASH_SIZE 4 * 10240
struct Entry{
	struct UserData data;
	int key;
	unsigned timeout;
};
struct ListItem{
	struct Entry ety;
	struct ListItem* next;
	struct ListItem* pre;
};

struct HashItem{
	struct ListItem *host;
	struct HashItem *next;
};
int 
hash_func(int key){
	return key%HASH_SIZE;
}
struct List{
	struct ListItem * head,*tail;
	unsigned minTimeout;
	unsigned times;
	int numItem;
};
struct HashMap{
	struct HashItem* items[HASH_SIZE];
};
struct ToEntryTable{
	struct HashMap hmap;
	struct List list;
};
static inline void
listItemAdd(struct List *list){
	if(list->numItem == 0){
		//startTimer();
	}
	list->numItem ++;
}
static inline void
listItemSub(struct List *list){
	list->numItem--;
	if(list->numItem == 0){
		list->times = 0;
		list->minTimeout = -1;
		//stopTimer();
	}
}

static void listInit(struct List *list);
static void listDestroy(struct List *list);
static struct ListItem * listInsert(struct List *list,int key,struct UserData data,unsigned timeout);
static void listRealInsert(struct List *list,struct ListItem*item);
static void listRemove(struct List *list,struct ListItem * litem);
static void listRemoveByTimeout(struct ToEntryTable *tet,unsigned timeout);

static void hashInit(struct HashMap *hmap);
static void hashDestroy(struct HashMap *hmap);
static void hashInsert( struct HashMap *hmap,int key,struct ListItem *item );
static struct ListItem * hashQuery(struct HashMap *hmap, int key );
static struct ListItem * hashRemove( struct HashMap *hmap,int key );


struct ToEntryTable * 
TET_new()
{
	struct ToEntryTable *p = (struct ToEntryTable *)malloc(sizeof(struct ToEntryTable));
	listInit(&p->list);
	hashInit(&p->hmap);
	return p;
}
void 
TET_del(struct ToEntryTable *txt)
{
	listDestroy(&txt->list);
	hashDestroy(&txt->hmap);
	free(txt);
}
void 
TET_onTimer(struct ToEntryTable *tet,unsigned times){
	if( tet->list.numItem > 0 ){
		tet->list.times+=times;
		printf("cut times %d : min timeout%d numitem:%d\n",tet->list.times,tet->list.minTimeout,tet->list.numItem);
		if(tet->list.times >= tet->list.minTimeout){
			listRemoveByTimeout(tet,tet->list.times);
			if(tet->list.head)
				tet->list.minTimeout = tet->list.head->ety.timeout;
		}
	}
}
int
TET_insertEntry(struct ToEntryTable* tet, int key,struct UserData data,unsigned timeout)
{
	if(hashQuery(&tet->hmap,key)){
		return -1;
	}
	struct ListItem * item = listInsert(&tet->list,key,data,timeout);
	hashInsert(&tet->hmap,key,item);
	return 0;
}
int
TET_removeEntry(struct ToEntryTable* tet ,int key,struct UserData *data)
{
	struct ListItem * item = hashRemove(&tet->hmap,key);
	if(item){
		*data = item->ety.data;
		listRemove(&tet->list,item);
		return 0;
	}
	return -1;
}
int
TET_queryEntry(struct ToEntryTable* tet ,int key,struct UserData *data)
{
	struct ListItem * item = hashQuery(&tet->hmap,key);
	if(item){
		*data = item->ety.data;
		return 0;
	}
	return -1;
}

static void
notifyListItemRemove(struct ToEntryTable* tet,struct Entry ety)
{
	hashRemove(&tet->hmap,ety.key);
}
static void
listInit(struct List *list)
{
	list->head = NULL;
	list->tail = NULL;
	list->numItem = 0;
	list->times = 0;
	list->minTimeout = -1;
}
static void 
listDestroy(struct List *list)
{
	struct ListItem *next,*cut = list->head;
	while(cut){
		next = cut->next;
		free(cut);
		cut = next;
	}
}
static struct ListItem *
listInsert(struct List *list,int key,struct UserData data,unsigned timeout)
{

	struct ListItem *p = malloc(sizeof(struct ListItem));
	p->ety.key = key;
	p->ety.timeout = timeout+list->times;
	p->ety.data = data;
	p->next = NULL;
	p->pre = NULL;
	if(p->ety.timeout < list->minTimeout){
		list->minTimeout = p->ety.timeout;
	}
	listRealInsert(list,p);
	listItemAdd(list);
	return p;
}
static void
listRealInsert(struct List *list,struct ListItem*item)
{
	if(!list->head){
		list->head = item;
		list->tail = item;
		return;
	}
	struct ListItem *cut = list->head;
	while(1){
		if(cut->ety.timeout >= item->ety.timeout){//optimize：cpu idle case itemptr to array 1：,binary search 。2：by timeout average chose tial or head 
			if(!cut->pre){
				list->head = item;
			}else{
				cut->pre->next = item;
				item->pre = cut->pre;
			}
			item->next = cut;
			cut->pre = item;
			break;
		}
		if(!cut->next){
			cut->next = item;
			item->pre = cut;
			list->tail = item;
			break;
		}
		cut = cut->next;
	}
}
static void
listRemove(struct List *list,struct ListItem * litem)
{
	if(litem->pre)litem->pre->next = litem->next;
	if(litem->next)litem->next->pre = litem->pre;
	if(litem == list->head){
		list->head = litem->next;
		if(list->head)list->minTimeout = list->head->ety.timeout;
		else list->minTimeout = -1;
	}else if(litem == list->tail){
		list->tail = litem->pre;
	}
	listItemSub(list);
	free(litem);
}

static void
listRemoveByTimeout(struct ToEntryTable *tet,unsigned timeout)
{
	while( tet->list.head->ety.timeout <= timeout ){
		struct ListItem *tmp = tet->list.head;
		tet->list.head = tmp->next;
		listItemSub(&tet->list);
		notifyListItemRemove(tet,tmp->ety);
		free(tmp);
		if( !tet->list.head ){
			break;
		}
	}
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
hashInsert( struct HashMap *hmap,int key,struct ListItem *item )
{
	int hash = hash_func(key);
	struct HashItem * hitem = (struct HashItem *)malloc(sizeof(struct HashItem));
	hitem->host = item;
	struct HashItem **pp = &hmap->items[hash];
	hitem->next = *pp;
	*pp = hitem;
}
static struct ListItem *
hashQuery(struct HashMap *hmap, int key )
{
	int hash = hash_func(key);
	struct HashItem *p = hmap->items[hash];
	while(p){
		if(p->host->ety.key == key){
			return p->host;
		}
		p = p->next;
	}
	return NULL;
}
static struct ListItem *
hashRemove( struct HashMap *hmap,int key )
{
	int hash = hash_func(key);
	struct HashItem *pre = NULL,*cut = hmap->items[hash];
	while(cut){
		if(cut->host->ety.key == key){
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
