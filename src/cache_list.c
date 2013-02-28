
struct list_item
{
	struct TET_entry ety;
	struct list_item* next;
	struct list_item* pre;
};
struct list
{
	struct list_item * head,*tail;
	unsigned minTimeout;
	unsigned times;
	int numItem;
};
static inline void
list_item_add(struct list *list){
	if(list->numItem == 0){
		//startTimer();
	}
	list->numItem ++;
}
static inline void
list_item_sub(struct list *list){
	list->numItem--;
	if(list->numItem == 0){
		list->times = 0;
		list->minTimeout = -1;
		//stopTimer();
	}
}
static void *
list_new()
{
	struct list *list = malloc(sizeof(struct list));
	list->head = NULL;
	list->tail = NULL;
	list->numItem = 0;
	list->times = 0;
	list->minTimeout = -1;
	return list;
}
static void 
list_delete(void *list)
{
	struct list_item *next,*cut = ((struct list *)(list))->head;
	while(cut){
		next = cut->next;
		free(cut);
		cut = next;
	}
	free(list);
}
static void list_real_insert_from_tail(struct list *list,struct list_item*item);
static void list_real_insert(struct list *list,struct list_item*item);
static void *
list_insert(void *arg,unsigned key,struct user_data data,unsigned timeout)
{
	struct list *list = (struct list *)(arg);
	struct list_item *p = malloc(sizeof(struct list_item));
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
			list_real_insert_from_tail(list,p);
			return p;
		}
	}
	list_real_insert(list,p);
	list_item_add(list);
	return p;
}
static void
list_real_insert(struct list *list,struct list_item*item)
{
	if(!list->head){
		list->head = item;
		list->tail = item;
		return;
	}
	struct list_item *cut = list->head;
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
list_real_insert_from_tail(struct list *list,struct list_item*item)
{
	if(!list->head){
		list->head = item;
		list->tail = item;
		return;
	}
	struct list_item *cut = list->tail;
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
list_remove(void *arg,void * item)
{
	struct list *list = (struct list *)(arg);
	struct list_item * litem = (struct list_item *)(item);
	if(litem->pre)litem->pre->next = litem->next;
	if(litem->next)litem->next->pre = litem->pre;
	if(litem == list->head){
		list->head = litem->next;
		if(list->head)list->minTimeout = list->head->ety.timeout;
		else list->minTimeout = -1;
	}else if(litem == list->tail){
		list->tail = litem->pre;
	}
	list_item_sub(list);
	free(litem);
}
static int
list_on_timer(void *arg,unsigned times,remove_cb cb,void *cbarg)
{
	struct list *list = (struct list *)(arg);
	int num = 0;
	if( list->numItem > 0 ){
		list->times+=times;
		if(list->times >= list->minTimeout){
			while( list->head->ety.timeout <= list->times ){
				struct list_item *tmp = list->head;
				list->head = tmp->next;
				list_item_sub(list);
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
