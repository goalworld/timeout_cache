#include "toentrytable.h"
#include <wod_hashmap.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stddef.h>
#define HASH_SIZE 10* 10240
typedef void(*remove_cb)(void *arg,struct TET_entry try) ;
struct ToCache
{
	void *(*new)();
	void (*delete)(void *);
	void *(*insert)(void *list,unsigned key,struct user_data data,unsigned timeout);
	void (*remove)(void *list,void * litem);
	int(* on_timer)(void *list,unsigned timeout,remove_cb cb,void *arg);
};
#define SET_TO_CACHE(tocache,type) \
				tocache->new = type##_new;\
				tocache->delete = type##_delete;\
				tocache->insert = type##_insert;\
				tocache->remove = type##_remove;\
				tocache->on_timer = type##_on_timer;
				
int initToCache( struct ToCache * p,int type);

struct TET_table
{
	struct wod_hash_map *hmap;
	void * timeoutCache;
	struct ToCache toCahe;
};
static unsigned 	
hashFunc(void *env,const void *key)
{
	return (unsigned)(ptrdiff_t)key * 7;
}

struct TET_table * 
TET_new( int type)
{
	struct TET_table *p = (struct TET_table *)malloc(sizeof(struct TET_table));
	if( initToCache(&p->toCahe,type) != 0){
		free(p);
		return NULL;
	}
	p->timeoutCache = p->toCahe.new();
	struct wod_hash_map_type whmt;
	memset(&whmt,0,sizeof(whmt));
	whmt.hash_func = hashFunc;
	p->hmap = wod_hashmap_new(whmt,NULL);
	return p;
}
void 
TET_delete(struct TET_table *txt)
{
	txt->toCahe.delete(txt->timeoutCache);
	wod_hashmap_delete(txt->hmap);
	free(txt);
}
static void
notifyListItemRemove(void *arg ,struct TET_entry ety)
{
	struct TET_table* tet =( struct TET_table* )arg;
	unsigned key = ety.key;
	wod_hashmap_remove(tet->hmap,(void *)(ptrdiff_t)key);
}

int 
TET_on_timer(struct TET_table *tet,unsigned times){
	return tet->toCahe.on_timer(tet->timeoutCache,times,notifyListItemRemove,tet);
}
int
TET_insert_entry(struct TET_table* tet, unsigned key,struct user_data data,unsigned timeout)
{
	void * item = tet->toCahe.insert(tet->timeoutCache,key,data,timeout);
	wod_hashmap_insert(tet->hmap,(void *)(ptrdiff_t)key,item);
	return 0;
}
int
TET_remove_entry(struct TET_table* tet ,unsigned key,struct user_data *data)
{
	void * item = wod_hashmap_remove(tet->hmap,(void *)(ptrdiff_t)key);
	if(item){
		*data = ((struct TET_entry *)item)->data;
		tet->toCahe.remove(tet->timeoutCache,item);
		return 0;
	}
	return -1;
}
int
TET_query_entry(struct TET_table* tet ,unsigned key,struct user_data *data)
{
	void * item = wod_hashmap_query(tet->hmap,(void *)(ptrdiff_t)key);
	if(item){
		*data = ((struct TET_entry *)item)->data;
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
		SET_TO_CACHE(p,hash_list);
		return 0;
	}
	return -1;
}
