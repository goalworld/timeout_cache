#ifndef _ENTRY_MAP_TIMEOUT_H_
#define _ENTRY_MAP_TIMEOUT_H_
struct TET_table;

struct user_data{
	void* data;
	unsigned sz;
};
struct TET_entry{
	struct user_data data;
	unsigned key;
	unsigned timeout;
};
enum  {
	TET_LIST,
	TET_HASH
};
struct TET_table *TET_new( int type);
void TET_delete(struct TET_table *txt);
int TET_insert_entry(struct TET_table* tet, unsigned key,struct user_data data,unsigned  timeout);
int TET_remove_entry(struct TET_table* tet ,unsigned key,struct user_data *data);
int TET_query_entry(struct TET_table* tet ,unsigned key,struct user_data *data);
int TET_on_timer(struct TET_table *tet,unsigned times);
#endif
