#include "compress.h"

void init_heap(HEAP* heap, HEAP_NODE** array, int size){
	heap->array = array;
	heap->size = size;
	heap->last = 0;
}

HEAP_NODE* create_heap_node(unsigned char data, int priority){
	HEAP_NODE* node = malloc(sizeof(HEAP_NODE));
	node->data = data;
	node->priority = priority;
	node->left = NULL;
	node->right = NULL;
	return node;
}

void insert_heap(HEAP* heap, HEAP_NODE* node){
	int i;
		
	if(heap->size <= heap->last+1){
		printf("FULL HEAP\n");
		return;
	}
	
	heap->last += 1;
	
	for(i=heap->last;i>1 && heap->array[i/2]->priority>node->priority;i/=2){
		heap->array[i] = heap->array[i/2];
	}
	
	heap->array[i] = node;
}

HEAP_NODE* delete_heap(HEAP* heap){
	int i,min;
	HEAP_NODE* res;
	HEAP_NODE* data;
	
	if(heap->last<1){
		printf("EMPTY HEAP\n");
		return NULL;
	}
	res = heap->array[1];
	data = heap->array[heap->last--];
	
	i=1;
	while(i*2<=heap->last){
		if(i*2+1 > heap->last) min = i*2;
		else min = heap->array[i*2]->priority < heap->array[i*2+1]->priority ? i*2 : i*2+1;
		if(heap->array[min]->priority < data->priority){
			heap->array[i] = heap->array[min];
		}else{
			break;
		}
		i = min;
	}
	
	heap->array[i] = data;
	return res;
}