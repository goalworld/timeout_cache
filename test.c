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
/*
#ifdef WIN32
#include <windows.h>
#else
#include <sys/select.h>
#endif
void
test_sleep(double delay)
{
#ifdef WIN32
	Sleep(delay*1000);
#else
	struct timeval tv;
	tv.tv_sec  = (time_t)delay;
	tv.tv_usec = (long)((delay - (double)(tv.tv_sec)) * 1e6);
	select (0, 0, 0, 0, &tv);
#endif
}*/
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


	struct ToEntryTable * tet = TET_new(TET_HASH);
	int i;
	srand(time(NULL));
	struct UserData data;
	clock_t pre  =  clock(),df,begin;
	begin= pre;

	printf("(1)INSERT: adding entry max number : %d please wait\n",len);
	for(i=1;i<=len;i++){
		data.data = (void *)i;
		data.sz = 0;
		TET_insertEntry(tet,i,data,(rand()*7)%10240);
		if(i%(len/10) == 0 && i!=0){
			clock_t now = clock();
			printf("added %d ....difclock:%ld ms\n",i,now-pre );
			pre = now;
		}
	}
	df = clock()-begin;
	printf("\n\n\nINSERT-RESULT:added entry max number : %d all:%ld ms  one:%f ms\n\n\n ",len,df,(double)(df)/len);

	puts("(2):QUERY: wait a moment now is testing query"); 
	int ret =0;
	pre = clock();
	int numq = 100;
	for(i=0;i<numq;i++){
		int d = rand()%len;
		if( (ret = TET_queryEntry(tet,d,&data)) == 0){
			printf("[%8d->%8d] %s",d,(int)data.data,i%4 == 0?"\n":"");
		}else{
			printf("[%8d->%8s] %s",d,"nil",i%4 == 0?"\n":"");
		}

	}
	df = clock()-pre;
	printf("\n\n\nQUERY-RESULT : num:%d clock:%ld ms oneclock:%f ms\n\n\n",numq,df,(double)(df)/(double)(numq));

	puts("\n\n\n (3):TIMEOUT: wait a moment now is testing timeout-remove"); 
	int num = 0;
	int ernum=0;
	begin = pre = clock();
	for(i=0;i<100;i++){
		num += (ernum = TET_onTimer(tet,1));
		int now = clock();
		df = now-pre;
		printf("TIMEOUT-RESULT: TimePass : %d s :DelNum:%d clock:%ld ms oneclock:%f ms\n",i,ernum,df,(double)(df)/(double)(ernum));
		pre = now;
	}
	df = clock()-begin;
	printf("\n\n\nTIMEOUT-RESULT: TimePass : %d s :DelNum:%d clock:%ld ms oneclock:%f ms\n\n\n",i,num,df,(double)(df)/(double)(num));

	puts("\n\n\n(4)REMOVE: wait a moment now is testing remove");
	pre = clock(); 
	int numd = 100;
	for(i=0;i<100;i++){
		int d = rand()%len;
		if( (ret = TET_removeEntry(tet,d,&data)) == 0 ){
			printf("[%8d->%8d] %s",d,(int)data.data,i%4 == 0?"\n":"");
		}else{
			printf("[%8d->%8s] %s",d,"nil",i%4 == 0?"\n":"");
		}
	}
	df = clock()-pre;
	printf("\n\n\nREMOVE-RESULT : num:%d clock:%ld ms oneclock:%f ms\n\n\n",numd,df,(double)(df)/(double)(numd));
	TET_del(tet);
	return 0;
}
