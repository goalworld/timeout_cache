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
unsigned 	hashFunc(void *env,const void *key)
{
	return (unsigned)key;
}
double
rainGetTime()
{
	struct timeval time;
	gettimeofday(&time,(void *)0);
	return time.tv_sec *1E6 + time.tv_usec ;
}
int main(int argc, char const *argv[])
{
	struct wcHashMapType whmt;
	memset(&whmt,0,sizeof(whmt));
	whmt.hashFunc = hashFunc;
	struct wcHashMap *hm = wcHashMapNew(whmt,NULL);
	int i=0;
	double pre = rainGetTime(),df;
	for(;i<10000000;i++){
		wcHashMapInsert(hm,(void *)i,(void *)i);
	}
	df = rainGetTime()-pre;
	printf("wcHashMapInsert >>> all:%f---one:%f\n",df,df*1E-7);
	pre = rainGetTime();
	for( i=0;i<10000000;i++){
		wcHashMapQuery(hm,(void *)i);
	}
	df = rainGetTime()-pre;
	printf("wcHashMapQuery >>> all:%f---one:%f\n",df,df*1E-7);
	wcHashMapDelete(hm);
	return 0;
}
