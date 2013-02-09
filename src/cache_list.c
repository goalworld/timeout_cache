
struct ListItem
{
	struct Entry ety;
	struct ListItem* next;
	struct ListItem* pre;
};
struct List
{
	struct ListItem * head,*tail;
	unsigned minTimeout;
	unsigned times;
	int numItem;
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
static void *
listNew()
{
	struct List *list = malloc(sizeof(struct List));
	list->head = NULL;
	list->tail = NULL;
	list->numItem = 0;
	list->times = 0;
	list->minTimeout = -1;
	return list;
}
static void 
listDel(void *list)
{
	struct ListItem *next,*cut = ((struct List *)(list))->head;
	while(cut){
		next = cut->next;
		free(cut);
		cut = next;
	}
	free(list);
}
static void listRealInsertFromTail(struct List *list,struct ListItem*item);
static void listRealInsert(struct List *list,struct ListItem*item);
static void *
listInsert(void *arg,unsigned key,struct UserData data,unsigned timeout)
{
	struct List *list = (struct List *)(arg);
	struct ListItem *p = malloc(sizeof(struct ListItem));
	p->ety.key = key;
	p->ety.timeout = timeout+list->times;
	p->ety.data = data;
	p->next = NULL;
	p->pre = NULL;
	if(p->ety.timeout < list->minTimeout){
		list->minTimeout = p->ety.timeout;
	}
	if(list->head){
		unsigned t = list->head->ety.timeout + list->tail->ety.timeout;
		if(p->ety.timeout >= t/2){
			listRealInsertFromTail(list,p);
			return p;
		}
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
listRealInsertFromTail(struct List *list,struct ListItem*item)
{
	if(!list->head){
		list->head = item;
		list->tail = item;
		return;
	}
	struct ListItem *cut = list->tail;
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
listRemove(void *arg,void * item)
{
	struct List *list = (struct List *)(arg);
	struct ListItem * litem = (struct ListItem *)(item);
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
static int
listOnTimer(void *arg,unsigned times,remove_cb cb,void *cbarg)
{
	struct List *list = (struct List *)(arg);
	int num = 0;
	if( list->numItem > 0 ){
		list->times+=times;
		if(list->times >= list->minTimeout){
			while( list->head->ety.timeout <= list->times ){
				struct ListItem *tmp = list->head;
				list->head = tmp->next;
				listItemSub(list);
				num++;
				cb(cbarg,tmp->ety);
				free(tmp);
				if( !list->head ){
					break;
				}
			}
			if(list->head)
				list->minTimeout = list->head->ety.timeout;
		}
	}
	return num;

}
