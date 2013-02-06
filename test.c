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
	clock_t pre  =  clock();
	clock_t begin= pre;

	printf("adding entry max number : %d please wait\n",len);
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
	clock_t ddf = clock()-begin;
	printf("added entry max number : %d all:%ld ms  one:%f ms\n, ",len,ddf,(double)(ddf)/len);

	puts("wait a moment now is testing query"); 
	int ret =0;
	for(i=0;i<100;i++){
		int d = rand()%len;
		if( (ret = TET_queryEntry(tet,d,&data)) == 0){
			printf("[%8d->%8d] %s",d,(int)data.data,i%4 == 0?"\n":"");
		}else{
			printf("[%8d->%8s] %s",d,"nil",i%4 == 0?"\n":"");
		}

	}

	puts("\n wait a moment now is testing timeout-remove"); 
	clock_t pre1 = clock(),df;
	int num = 0;
	for(i=0;i<300;i++){
		num += TET_onTimer(tet,1);
	}
	df = clock()-pre1;
	printf("TimePass : %d s :DelNum:%d clock:%ld ms oneclock:%f ms\n",i,num,df,(double)(df)/(double)(num));

	puts("wait a moment now is testing remove"); 
	for(i=0;i<100;i++){
		int d = rand()%len;
		if( (ret = TET_removeEntry(tet,d,&data)) == 0 ){
			printf("[%8d->%8d] %s",d,(int)data.data,i%4 == 0?"\n":"");
		}else{
			printf("[%8d->%8s] %s",d,"nil",i%4 == 0?"\n":"");
		}
	}
	TET_del(tet);
	return 0;
}
