struct JListItem
{
	struct Entry ety;
	struct JListItem * items;
	int items_len;
};

struct JListCache
{
	struct JListItem * head;
	unsigned times;
	unsigned minTimes;
};
 
void * jlistNew()
{
	struct JListCache * p = (struct JListCache *)malloc(sizeof(struct JListCache));
	p->times = 0;
	p->minTimes = -1;
	p->head = NULL;
	return p;
}
void jlistDel( void * env)
{

	free(env);
}
static void * 
jlistInsert(void *arg,int key,struct UserData data,unsigned timeout)
{

}
static void 
jlistRemove(void *arg,void * item)
{

}
static void 
jlistOnTimer(void *arg,unsigned times,void(*cb)(void *arg,struct Entry try),void *cbarg)
{
	
}