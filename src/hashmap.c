#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include "hashmap.h"


static struct wcHashMapTable *  _hmtNew(unsigned key,unsigned sz);
static void _hmtDelete(struct wcHashMap * hm,struct wcHashMapTable * hmt);
static void  _hmtReHash(struct wcHashMap * hm,int index);
static void _hmtDetach(struct wcHashMap *hm,int lindex,int rindex);
static struct wcHashMapEntry *  _hmtRemove(struct wcHashMap * hm,int lindex,unsigned key);
static struct wcHashMapEntry *  _hmtQuery(struct wcHashMap * hm,int lindex,unsigned key);
static int _hmtInsert(struct wcHashMap * hm,int index,unsigned key ,struct wcHashMapEntry * entry);





struct wcHashMap * 
wcHashMapNew(struct wcHashMapType hmt,void *hmtenv)
{
	struct wcHashMap * hm = malloc(sizeof(struct wcHashMap));
	hm->ktenv = hmtenv;
	hm->ktype = hmt;
	hm->tblen = 4;
	hm->tbs = malloc(sizeof(void*)*(hm->tblen));
	unsigned i = 0;
	unsigned df = 0xffffffff/hm->tblen;
	for(;i<hm->tblen;i++){
		hm->tbs[i] = _hmtNew((i+1)*df,32);
	}
	return hm;
}
void 	
wcHashMapDelete(struct wcHashMap * hm)
{
	int i = 0;
	for(;i<hm->tblen;i++){
		_hmtDelete(hm,hm->tbs[i]);
	}
	free(hm->tbs);
	free(hm);
}
int 	
wcHashMapInsert(struct wcHashMap * hm,const void *key,const void *value)
{
	struct wcHashMapEntry * entry = malloc(sizeof(struct wcHashMapEntry));
	entry->kv.key =   hm->ktype.keyClone   ? hm->ktype.keyClone(hm->ktenv,key):key;
	entry->kv.value = hm->ktype.valueClone ? hm->ktype.valueClone(hm->ktenv,value):value;
	entry->tkey = hm->ktype.hashFunc(hm->ktenv,key);
	entry->next = NULL;
	int i=0;
	for(;i<hm->tblen;i++){
		if(entry->tkey <= hm->tbs[i]->hashkey){
			_hmtInsert(hm,i,entry->tkey,entry);
			break;
		}
	}
	return 0;
}
void *
wcHashMapQuery(struct wcHashMap *hm,const void *key)
{
	struct wcHashMapEntry * entry;
	unsigned tkey = hm->ktype.hashFunc(hm->ktenv,key);
	int i=0;
	for(;i<hm->tblen;i++){
		if(tkey <= hm->tbs[i]->hashkey){
			entry = _hmtQuery(hm,i,tkey);
			break;
		}
	}
	if(entry){
		return (void *)(hm->ktype.valueClone ? hm->ktype.valueClone(hm->ktenv,entry->kv.value):entry->kv.value);
	}
	return NULL;
}
void *
wcHashMapRemove(struct wcHashMap *hm,const void *key)
{
	struct wcHashMapEntry * entry;
	unsigned tkey = hm->ktype.hashFunc(hm->ktenv,key);
	int i=0;
	void *value = NULL;
	for(;i<hm->tblen;i++){
		if(tkey <= hm->tbs[i]->hashkey){
			entry = _hmtRemove(hm,i,tkey);
			if(entry){
				value = (void *)(hm->ktype.valueClone ? hm->ktype.valueClone(hm->ktenv,entry->kv.value):entry->kv.value);
				free(entry);
			}
			break;
		}
	}
	return value;
}
unsigned
hmtHashFunc(struct wcHashMapTable *hmt,unsigned key)
{
	return (key * 7)%hmt->cap;
}

static struct wcHashMapTable *
_hmtNew(unsigned key,unsigned sz)
{
	struct wcHashMapTable * hmt = malloc(sizeof(struct wcHashMapTable));
	hmt->etys = malloc(sz*sizeof(void *));
	memset(hmt->etys,0,sz*sizeof(void *));
	hmt->used = 0;
	hmt->cap = sz;
	hmt->hashkey = key;
	return hmt;
}
static void
_hmtDelete(struct wcHashMap *hm,struct wcHashMapTable * hmt)
{
	int i=0;
	struct wcHashMapEntry *tmp,*cut;
	for(;i<hmt->cap;i++){
		cut = hmt->etys[i];
		while(cut){
			tmp = cut->next;
			if(hm->ktype.keyDestroy)hm->ktype.keyDestroy(hm->ktenv,cut->kv.key);
			if(hm->ktype.keyDestroy)hm->ktype.valueDestroy(hm->ktenv,cut->kv.value);
			free(cut);
			cut = tmp;
		}
	}
	free(hmt->etys);
	free(hmt);
}
static int
_hmtInsert(struct wcHashMap * hm,int index,unsigned key ,struct wcHashMapEntry * entry)
{
	struct wcHashMapTable *hmt = hm->tbs[index];
	if(hmt->cap == hmt->used){
		_hmtReHash(hm,index);
		hmt = hm->tbs[index];
	}
	int i = hmtHashFunc(hmt,key);
	entry->next = hmt->etys[i];
	hmt->etys[i] = entry;
	hmt->used++;
	return 0;
}
static struct wcHashMapEntry *  
_hmtQuery(struct wcHashMap * hm,int index,unsigned key)
{
	struct wcHashMapTable * hmt = hm->tbs[index];
	struct wcHashMapEntry * cut;
	unsigned i = hmtHashFunc(hmt,key);
	cut = hmt->etys[i];
	while(cut){
		if( cut->tkey == key ){
			return cut;
		}
		cut = cut->next;
	}
	return NULL;
}
static struct wcHashMapEntry *  
_hmtRemove(struct wcHashMap * hm,int index,unsigned key)
{
	struct wcHashMapTable * hmt = hm->tbs[index];
	struct wcHashMapEntry * cut,*pre;
	int i = hmtHashFunc(hmt,key);
	cut = hmt->etys[i]; pre = NULL;
	while(cut){
		if( cut->tkey == key ){
			if(pre) pre->next = cut->next;
			else 	hmt->etys[i]=cut->next;
			hmt->used--;
			return cut;
		}
		pre = cut;
		cut = cut->next;
	}
	return NULL;
}

static void
_hmtDetach(struct wcHashMap *hm,int lindex,int rindex)
{
	struct wcHashMapTable * hmtLft = hm->tbs[lindex];
	struct wcHashMapTable * hmtRht = hm->tbs[rindex];
	struct wcHashMapEntry * cut,*pre,*tmp;
	int i;
	for(i=0; i<hmtRht->cap;i++){
		cut = hmtRht->etys[i]; pre = NULL;
		while(cut){
			if( cut->tkey <= hmtLft->hashkey ){
				tmp = cut;
				cut = cut->next;
				_hmtInsert(hm,lindex,tmp->tkey,tmp);
				if(pre) pre->next = cut;
				else 	hmtRht->etys[i]=cut;
				continue;
			}
			pre = cut;
			cut = cut->next;
		}
	}
}

static void  
_hmtReHash(struct wcHashMap * hm,int index)
{
	struct wcHashMapEntry * cut,*pre,*tmp;
	struct wcHashMapTable * hmt = hm->tbs[index];
	hm->tbs[index] = _hmtNew(hmt->hashkey,hmt->cap*2);
	int i;
	for(i=0; i<hmt->cap;i++){
		cut = hmt->etys[i]; pre = NULL;
		while(cut){
			tmp = cut;
			cut = cut->next;
			_hmtInsert(hm,index,tmp->tkey,tmp);
		}
	}
	free(hmt->etys);
	free(hmt);
}
