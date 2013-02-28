/*
 * test.c
 *
 *  Created on: 2013-2-5
 *      Author: Administrator
 */

#include "./toentrytable.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wod_time.h>
#include <stddef.h>
int 
main(int argc,char * argv[])
{
	if(argc != 2){
		puts("please input num entry add like : ./test 10000");
		return 1;
	}
	int len = atoi(argv[1]);
	if(len <= 0){
		puts("please input number like : ./test 10000");
		return 1;
	}

	struct TET_table * tet = TET_new(TET_HASH);
	if(!tet){
		printf("tet create fail!");
		return 1;
	}
	int i;
	srand(time(NULL));
	struct user_data data;
	long long pre  = wod_time_usecond(),now,df,begin;
	begin= pre;

	printf("(1)INSERT: adding entry max number : %d please wait\n",len);
	for(i=1;i<=len;i++){
		data.data = (void *)(ptrdiff_t)i;
		data.sz = 0;
		TET_insert_entry(tet,i,data,rand()%10240);
		if(i%(len/10) == 0 && i!=0){
			now = wod_time_usecond();
			printf("added %d ....difclock:%ld us one_difclock:%f us\n",i,now-pre,(double)(now-pre)/(len/10));
			pre = now;
		}
	}
	df = wod_time_usecond()-begin;
	printf("\n\n\nINSERT-RESULT:added entry max number : %d all:%ld us  one:%f us\n\n\n ",len,df,(double)(df)/len);

	puts("(2):QUERY: wait a moment now is testing query"); 
	int ret =0;
	pre = wod_time_usecond();
	int numq = 100;
	for(i=0;i<numq;i++){
		int d = rand()%len;
		if( (ret = TET_query_entry(tet,d,&data)) == 0){
			printf("[%8d->%8d] %s",d,(int)(ptrdiff_t)data.data,i%4 == 0?"\n":"");
		}else{
			printf("[%8d->%8s] %s",d,"nil",i%4 == 0?"\n":"");
		}

	}
	df = wod_time_usecond()-pre;
	printf("\n\n\nQUERY-RESULT : num:%d clock:%ld us oneclock:%f us\n\n\n",numq,df,(double)(df)/(numq));

	puts("\n\n\n (3):TIMEOUT: wait a moment now is testing timeout-remove"); 
	int num = 0;
	int ernum=0;
	begin = pre = wod_time_usecond();
	for(i=0;i<30;i++){
		num += (ernum = TET_on_timer(tet,1));
		now = wod_time_usecond();
		df = now-pre;
		printf("TIMEOUT-RESULT: TimePass : %d s :DelNum:%d clock:%ld us oneclock:%f us\n",i,ernum,df,(double)(df)/(ernum));
		pre = now;
	}
	df = wod_time_usecond()-begin;
	printf("\n\n\nTIMEOUT-RESULT: TimePass : %d s :DelNum:%d clock:%ld us oneclock:%f us\n\n\n",i,num,df,(double)(df)/(num));

	puts("\n\n\n(4)REMOVE: wait a moment now is testing remove");
	pre = wod_time_usecond();
	int numd = 100;
	for(i=0;i<numd;i++){
		int d = rand()%len;
		if( (ret = TET_remove_entry(tet,d,&data)) == 0 ){
			printf("[%8d->%8d] %s",d,(int)(ptrdiff_t)data.data,i%4 == 0?"\n":"");
		}else{
			printf("[%8d->%8s] %s",d,"nil",i%4 == 0?"\n":"");
		}
	}
	df = wod_time_usecond()-pre;
	printf("\n\n\nREMOVE-RESULT : num:%d clock:%ld us oneclock:%f us\n\n\n",numd,df,(double)(df)/(numd));
	TET_delete(tet);
	return 0;
}
