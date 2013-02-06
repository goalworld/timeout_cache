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
//
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
}
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
	//srand(time(NULL));
	printf("adding entry max number : %d please wait\n, ",len);
	struct UserData data;
	for(i=0;i<len;i++){
		data.data = (void *)i;
		data.sz = 0;
		TET_insertEntry(tet,i,data,i%1000+2);
	}
	printf("added entry max number : %d \n, ",len);
	int ret =0;
	for(i=0;i<len;i++){
		if( (ret = TET_queryEntry(tet,i,&data)) == 0){
			printf("%d, ",(int)data.data);
		}else{
			printf("%s, ","nil");
		}

	}
	puts("");
	for(i=0;i<25;i++){
		test_sleep(1.0);
		printf("ontimer : %d \n, ",i);
		TET_onTimer(tet,1);
	}
	for(i=0;i<len;i++){
		if( (ret = TET_removeEntry(tet,i,&data)) == 0){
			printf("%d, ",(int)data.data);
		}else{
			printf("%s, ","nil");
		}
	}

	return 0;
}
