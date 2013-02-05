#include "entrymaptimeout.h"
#include <stdlib.h>
#include <memory.h>
#define HASH_SIZE 10240
struct Entry{
	void *data;
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
static struct ListItem * listInsert(struct List *list,int key,void* data,unsigned timeout);
static void listRealInsert(struct List *list,struct ListItem*item);
static void listRemove(struct List *list,struct ListItem * litem);
static void listRemoveByTimeout(struct ToEntryTable *tet,unsigned timeout);

static void hashInit(struct HashMap *hmap);
static void hashInsert( struct HashMap *hmap,int key,struct ListItem *item );
static struct ListItem * hashQuery(struct HashMap *hmap, int key );
static struct ListItem * hashRemove( struct HashMap *hmap,int key );


struct ToEntryTable * ToEnterTableNew()
{
	struct ToEntryTable *p = malloc(sizeof(struct ToEntryTable));
	listInit(&p->list);
	hashInit(&p->hmap);
	return p;
}
static void
_onTimer(struct ToEntryTable *tet){
	if( tet->list.numItem > 0 ){
		tet->list.times++;
		if(tet->list.times >= tet->list.minTimeout){
			listRemoveByTimeout(tet,tet->list.times);
		}
	}
}
int
insertEntry(struct ToEntryTable* tet, int key,void* data,unsigned timeout)
{
	if(hashQuery(&tet->hmap,key)){
		return -1;
	}
	struct ListItem * item = listInsert(&tet->list,key,data,timeout);
	hashInsert(&tet->hmap,key,item);
	return 0;
}
void *
removeEntry(struct ToEntryTable* tet ,int key)
{
	struct ListItem * item = hashRemove(&tet->hmap,key);
	if(item){
		void * data = item->ety.data;
		listRemove(&tet->list,item);
		return data;
	}
	return NULL;
}
void *
queryEntry(struct ToEntryTable* tet ,int key)
{
	struct ListItem * item = hashQuery(&tet->hmap,key);
	void * data = item->ety.data;
	return data;
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

static struct ListItem *
listInsert(struct List *list,int key,void* data,unsigned timeout)
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
	while(cut){
		if(cut->ety.timeout > item->ety.timeout){
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
	for(;;){
		if( tet->list.head->ety.timeout <= timeout){
			struct ListItem *tmp = tet->list.head;
			tet->list.head = tmp->next;
			listItemSub(&tet->list);
			notifyListItemRemove(tet,tmp->ety);
			free(tmp);
			if( !tet->list.head ){
				tet->list.tail = NULL;
				break;
			}
		}
	}
}
static void
hashInit(struct HashMap *hmap)
{
	memset(hmap->items,0,HASH_SIZE*sizeof(struct HashItem*));
}
static void
hashInsert( struct HashMap *hmap,int key,struct ListItem *item )
{
	int hash = hash_func(key);
	struct HashItem * hitem = malloc(sizeof(struct HashItem));
	hitem->host = item;
	struct HashItem **pp = &hmap->items[hash];
	struct HashItem *p =*pp;
	hitem->next = p;
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
	struct HashItem *pre,*cut = hmap->items[hash];
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
