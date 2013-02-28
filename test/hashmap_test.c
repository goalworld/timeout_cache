/*
 * hashmap_test.c
 *
 *  Created on: 2013-2-8
 *      Author: Administrator
 */


#include <stdio.h>
#include <stdlib.h>
#include "../include/hashmap.h"
#include <time.h>
#include <memory.h>
#include <assert.h>
#include <sys/time.h>
static unsigned 	
hashFunc(void *env,const void *key)
{
	return (unsigned)key;
}
static double
rainGetTime()
{
	struct timeval time;
	gettimeofday(&time,(void *)0);
	return time.tv_sec *1E6 + time.tv_usec ;
}
static void test(unsigned  num );
int main(int argc, char const *argv[])
{
	test(1000);
	test(10000);
	test(100000);
	test(1000000);
	test(10000000);
	//test(100000000);
	return 0;
}
static void 
test(unsigned  num )
{
	struct wcHashMapType whmt;
	memset(&whmt,0,sizeof(whmt));
	whmt.hashFunc = hashFunc;
	struct wcHashMap *hm = wcHashMapNew(whmt,NULL);
	int i=0;
	double pre = rainGetTime(),df;
	for(;i<num;i++){
		wcHashMapInsert(hm,(void *)i,(void *)i);
	}
	printf("[BEGIN]number : %d\n",num);
	df = rainGetTime()-pre;
	printf("[Insert]%d >>> all:%f---one:%f\n",num,df,df/num);
	pre = rainGetTime();
	for( i=0;i<num;i++){
		wcHashMapQuery(hm,(void *)i);
	}
	df = rainGetTime()-pre;
	printf("[Query] %d>>> all:%f---one:%f\n",num,df,df/num);
	puts("[END]");
	wcHashMapDelete(hm);
}
