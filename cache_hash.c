#define HASH_SET_S 10240
struct _ListItem
{
	struct Entry ety;
	struct _ListItem* next;
	struct _ListItem* pre;
};
struct _List
{
	struct _ListItem * head,*tail;
};
static void _listInit(struct _List *list);
static void _listDestroy(struct _List *list);
static struct _ListItem * _listInsert(struct _List *list,int key,struct UserData data,unsigned timeout);
static void _listRemove(struct _List *list,struct _ListItem * litem);
static int _listOnTimer(struct _List *list,unsigned times,remove_cb cb,void *cbarg);
static void _listRealInsert(struct _List *list,struct _ListItem*item);
static void _listRealInsertFromTail(struct _List *list,struct _ListItem*item);
struct HashList{
	struct _List lists[HASH_SET_S];
	unsigned times;
	int numItem;

};
#define HASH_FUNC(x)(((x)*21)%HASH_SET_S)
void * 
hashListNew()
{
	struct HashList * hl= malloc(sizeof(struct HashList));
	int i;
	for(;i<HASH_SET_S;i++){
		_listInit(&hl->lists[i]);
	}
	return hl;
}
void 
hashListDel(void *env)
{
	struct HashList * hl= (struct HashList *)env;
	int i;
	for(;i<HASH_SET_S;i++){
		_listDestroy(&hl->lists[i]);
	}
}
void *
hashListInsert(void *arg,int key,struct UserData data,unsigned timeout)
{
	struct HashList * hl= (struct HashList *)arg;
	unsigned t = timeout+hl->times;
	int hash = HASH_FUNC(t);
	void *ret = _listInsert(&hl->lists[hash],key,data,t);
	if(ret){
		if(hl->numItem == 0){
		//startTimer();
		}
		hl->numItem ++;
	}
	return ret;
}

static void
hashListRemove(void *arg,void *item)
{
	struct _ListItem *litem = (struct _ListItem*)item;
	struct HashList * hl= (struct HashList *)arg;
	int hash = HASH_FUNC(litem->ety.timeout);
	_listRemove(&hl->lists[hash],litem);
	if(--hl->numItem == 0){
		hl->times = 0;
		//stopTimer();
	}
}
static int
hashListOnTimer(void *arg,unsigned times,remove_cb cb,void *cbarg)
{
	struct HashList * hl= (struct HashList *)arg;
	if( hl->numItem > 0 ){
		hl->times+=times;
		int hash = HASH_FUNC(hl->times);
		int num = _listOnTimer(&hl->lists[hash],hl->times,cb,cbarg);
		hl->numItem -= num;
		if(hl->numItem == 0){
			hl->times = 0;
			//stopTimer();
		}
		return num;
	}
	return 0;
}
//---------------------------------------------------------------------------------
static struct Entry 
hashListGetEntryByItem( void *item)
{
	struct _ListItem * litem = (struct _ListItem *)(item);
	return litem->ety;
}
static void 
_listInit(struct _List *list)
{
	list->head = NULL;
	list->tail = NULL;
}
static void 
_listDestroy(struct _List *list)
{
	struct _ListItem *next,*cut = list->head;
	while(cut){
		next = cut->next;
		free(cut);
		cut = next;
	}
}


static struct _ListItem *
_listInsert(struct _List *list,int key,struct UserData data,unsigned timeout)
{
	struct _ListItem *p = malloc(sizeof(struct _ListItem));
	p->ety.key = key;
	p->ety.timeout = timeout;
	p->ety.data = data;
	p->next = NULL;
	p->pre = NULL;
	if(list->head){
		unsigned t = list->head->ety.timeout + list->tail->ety.timeout;
		if(timeout >= t/2){
			_listRealInsertFromTail(list,p);
			return p;
		}
	}
	_listRealInsert(list,p);
	return p;
}
static void
_listRealInsert(struct _List *list,struct _ListItem*item)
{
	if(!list->head){
		list->head = item;
		list->tail = item;
		return;
	}
	struct _ListItem *cut = list->head;
	while(1){
		//optimize 1 cpu idle case itemptr to array 2binary search timeout average chose tial or head
		if(cut->ety.timeout >= item->ety.timeout){
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
_listRealInsertFromTail(struct _List *list,struct _ListItem*item)
{
	if(!list->head){
		list->head = item;
		list->tail = item;
		return;
	}
	struct _ListItem *cut = list->tail;
	while(1){
		//optimize 1 cpu idle case itemptr to array 2binary search timeout average chose tial or head
		if(cut->ety.timeout <= item->ety.timeout){
			if(!cut->next){
				list->tail = item;
			}else{
				cut->next->pre = item;
				item->next = cut->next;
			}
			item->pre = cut;
			cut->next = item;
			break;
		}
		if(!cut->pre){
			cut->pre = item;
			item->next = cut;
			list->head = item;
			break;
		}
		cut = cut->pre;
	}
}
static void
_listRemove(struct _List *list,struct _ListItem * litem)
{
	if(litem->pre)litem->pre->next = litem->next;
	if(litem->next)litem->next->pre = litem->pre;
	if(litem == list->head){
		list->head = litem->next;
	}else if(litem == list->tail){
		list->tail = litem->pre;
	}
	free(litem);
}



//
static int
_listOnTimer(struct _List *list,unsigned times,remove_cb cb,void *cbarg)
{
	int num = 0;
	if( list->head){
		while( list->head->ety.timeout <= times ){
			struct _ListItem *tmp = list->head;
			list->head = tmp->next;
			cb(cbarg,tmp->ety);
			free(tmp);
			num++;
			if( !list->head ){
				break;
			}
		}
	}
	return num;
}
