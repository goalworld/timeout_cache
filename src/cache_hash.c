#define HASH_SET_S 10240
struct _list_item
{
	struct TET_entry ety;
	struct _list_item* next;
	struct _list_item* pre;
};
struct _list
{
	struct _list_item * head,*tail,*preInsert;
};
static void _list_init(struct _list *list);
static void _list_destroy(struct _list *list);
static struct _list_item * _list_insert(struct _list *list,unsigned key,struct user_data data,unsigned timeout);
static void _list_remove(struct _list *list,struct _list_item * litem);
static int _list_on_timer(struct _list *list,unsigned times,remove_cb cb,void *cbarg);
//static void _listRealInsert(struct _List *list,struct _ListItem*item);
//static void _listRealInsertFromTail(struct _List *list,struct _ListItem*item);
static void 
_list_real_insert_up_a2b(struct _list *list,struct _list_item *A,struct _list_item *B,struct _list_item*item);
static void 
_list_real_insert_down_a2b(struct _list *list,struct _list_item *A,struct _list_item *B,struct _list_item*item);
struct _hash_list{
	struct _list lists[HASH_SET_S];
	unsigned times;
	int numItem;

};
#define HASH_FUNC(x) ( ((x)*7)%HASH_SET_S )//multiply prime number make more hash
//(((int)( (((x)*618%1000)/1000)*HASH_SET_S))) 
void * 
hash_list_new()
{
	struct _hash_list * hl= malloc(sizeof(struct _hash_list));
	int i=0;
	for(;i<HASH_SET_S;i++){
		_list_init(&hl->lists[i]);
	}
	return hl;
}
void 
hash_list_delete(void *env)
{
	struct _hash_list * hl= (struct _hash_list *)env;
	int i=0;
	for(;i<HASH_SET_S;i++){
		_list_destroy(&hl->lists[i]);
	}
}
void *
hash_list_insert(void *arg,unsigned key,struct user_data data,unsigned timeout)
{
	struct _hash_list * hl= (struct _hash_list *)arg;
	unsigned t = timeout+hl->times;
	int hash = HASH_FUNC(t);
	void *ret = _list_insert(&hl->lists[hash],key,data,t);
	if(ret){
		if(hl->numItem == 0){
		//startTimer();
		}
		hl->numItem ++;
	}
	return ret;
}

static void
hash_list_remove(void *arg,void *item)
{
	struct _list_item *litem = (struct _list_item*)item;
	struct _hash_list * hl= (struct _hash_list *)arg;
	int hash = HASH_FUNC(litem->ety.timeout);
	_list_remove(&hl->lists[hash],litem);
	if(--hl->numItem == 0){
		hl->times = 0;
		//stopTimer();
	}
}
static int
hash_list_on_timer(void *arg,unsigned times,remove_cb cb,void *cbarg)
{
	struct _hash_list * hl= (struct _hash_list *)arg;
	if( hl->numItem > 0 ){
		hl->times+=times;
		int hash = HASH_FUNC(hl->times);
		int num = _list_on_timer(&hl->lists[hash],hl->times,cb,cbarg);
		hl->numItem -= num;
		if(hl->numItem == 0){
			hl->times = 0;
			//stopTimer();
		}
		return num;
	}
	return 0;
}
static void 
_list_init(struct _list *list)
{
	list->head = NULL;
	list->tail = NULL;
	list->preInsert = NULL;
}
static void 
_list_destroy(struct _list *list)
{
	struct _list_item *next,*cut = list->head;
	while(cut){
		next = cut->next;
		free(cut);
		cut = next;
	}
}


static struct _list_item *
_list_insert(struct _list *list,unsigned key,struct user_data data,unsigned timeout)
{
	struct _list_item *p = malloc(sizeof(struct _list_item));
	p->ety.key = key;
	p->ety.timeout = timeout;
	p->ety.data = data;
	p->next = NULL;
	p->pre = NULL;
	if(list->head){
		if(list->preInsert->ety.timeout > timeout){
			unsigned t = list->head->ety.timeout + list->preInsert->ety.timeout;
			if(timeout <= t/2){
				_list_real_insert_up_a2b(list,list->head,list->preInsert,p);
			}else{
				_list_real_insert_down_a2b(list,list->head,list->preInsert,p);
			}
		}else{
			unsigned t = list->tail->ety.timeout + list->preInsert->ety.timeout;
			if(timeout <= t/2){
				_list_real_insert_up_a2b(list,list->preInsert,list->tail,p);
			}else{
				_list_real_insert_down_a2b(list,list->preInsert,list->tail,p);
			}
		}
	}else{
		list->head = p;
		list->tail = p;
		list->preInsert = p;
	}
	list->preInsert = p;
	return p;
}
static void 
_list_real_insert_up_a2b(struct _list *list,struct _list_item *A,struct _list_item *B,struct _list_item*item)
{
	struct _list_item *cut = A;
	do{
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
	}while(cut != B->next);
}
static void 
_list_real_insert_down_a2b(struct _list *list,struct _list_item *A,struct _list_item *B,struct _list_item*item)
{
	struct _list_item *cut = B;
	do{
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
	}while(cut != A->pre);
}
/*static void
_listRealInsert(struct _List *list,struct _ListItem*item)
{
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
}*/
static void
_list_remove(struct _list *list,struct _list_item * litem)
{
	int a = 0;
	if(litem->pre){
		litem->pre->next = litem->next;
	}else{
		a = 1;
		list->head = litem->next;
	}
	if(litem->next){
		litem->next->pre = litem->pre;
	}else{
		list->tail = litem->pre;
	}
	if(list->preInsert == litem){
		list->preInsert = a?litem->next:litem->pre;
	}
	free(litem);
}



//
static int
_list_on_timer(struct _list *list,unsigned times,remove_cb cb,void *cbarg)
{
	int num = 0;
	if( list->head){
		while( list->head->ety.timeout <= times ){
			struct _list_item *tmp = list->head;
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
