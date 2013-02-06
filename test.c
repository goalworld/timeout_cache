/*
 * test.c
 *
 *  Created on: 2013-2-5
 *      Author: Administrator
 */

#include "./entrymaptimeout.h"
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
	struct ToEntryTable * tet = TET_new(TET_HASH);
	int len = atoi(argv[1]);
	if(len <= 0){
		puts("please input number like : ./test 10000");
		return 1;
	}
	int i;
	srand(time(NULL));
	clock_t pre = clock();
	clock_t begin = pre;
	printf("adding entry max number : %d please wait\n cutclock:%ld\n, ",len,pre);
	struct UserData data;
	for(i=0;i<len;i++){
		data.data = (void *)i;
		data.sz = 0;
		TET_insertEntry(tet,i,data,(rand()*7)%10240);
		if(i%10000 == 0 && i!=0){
			clock_t now = clock();
			printf("added %d ....clock:%ld dif:%ld\n",i,now,now-pre );
			pre = now;
		}
		
	}
	clock_t lst = clock();
	printf("added entry max number : %d cutclock:%ld all:%ld\n, ",len,lst,lst-begin);
	int ret =0;
	for(i=0;i<100;i++){
		int d = rand()%len;
		if( (ret = TET_queryEntry(tet,d,&data)) == 0){
			printf("[%8d->%8d] %s",d,(int)data.data,i%4 == 0?"\n":"");
		}else{
			printf("[%8d->%8s] %s","nil",i%4 == 0?"\n":"");
		}

	}
	puts("");
	clock_t pre1;
	int num = 0;
	for(i=0;i<1000;i++){
		//test_sleep(1.0);
		pre1 = clock();
		num = TET_onTimer(tet,1);
		clock_t now1 = clock();
		printf("ontimer : %d :deled:%d clock:%ld\n",i,num,now1-pre1);
		pre1 = now1;
	}
	for(i=0;i<10000;i++){
		int d = rand()%len;
		if( (ret = TET_removeEntry(tet,d,&data)) == 0){
			printf("[%8d->%8d] %s",d,(int)data.data,i%4 == 0?"\n":"");
		}else{
			printf("[%8d->%8s] %s","nil",i%4 == 0?"\n":"");
		}
	}

	return 0;
}
