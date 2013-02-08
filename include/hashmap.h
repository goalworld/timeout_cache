#ifndef _HASH_MAP_
#define _HASH_MAP_
struct wcHashMapEntry
{
	struct{
		const void *key;
		const void *value;
	}kv;
	unsigned tkey;
	struct wcHashMapEntry *next;
};
struct wcHashMapTable
{
	struct wcHashMapEntry** etys;
	unsigned used;
	unsigned cap;
	unsigned hashkey;
};
struct wcHashMapType
{
	unsigned 	(*hashFunc)(void *env,const void *key);
	int 		(*keyCampre)(void *env,const void *key1,const void *key2);
	void *		(*keyClone)(void *env,const void *key);
	void *		(*valueClone)(void *env,const void *value);
	void 		(*keyDestroy)(void *env,const void *key);
	void 		(*valueDestroy)(void *env,const void *value);
};
struct wcHashMap
{
	struct wcHashMapType ktype;
	void * ktenv;
	struct wcHashMapTable** tbs;
	unsigned tblen;
};
struct wcHashMap * wcHashMapNew(struct wcHashMapType hmt,void *hmtenv);
void 	wcHashMapDelete(struct wcHashMap * hm);
int 	wcHashMapInsert(struct wcHashMap * hm,const void *key,const void *value);
void * 	wcHashMapQuery(struct wcHashMap *hm,const void *key);
void * 	wcHashMapRemove(struct wcHashMap *hm,const void *key);
#endif
