/*
 * test.c
 *
 *  Created on: 2013-2-5
 *      Author: Administrator
 */

#include "entrymaptimeout.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main()
{
	struct ToEntryTable * tet = ToEnterTableNew();
	int len = 1000000;
	int i;
	srand(time(NULL));
	for(i=0;i<len;i++){
		insertEntry(tet,i,(void *)i,rand()%100);
	}
	int value;
	for(i=0;i<len;i++){
		value  = (int)removeEntry(tet,i);
		printf("%d\n",value);
	}
	return 0;
}
