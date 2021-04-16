#include "compress.h"

int huffman(FILE* input, FILE* output){
	int total_len = 0;
	int list_len = 1;
	int l;
	unsigned char i;
	unsigned char buf,buf_pos,pad=0;
	unsigned int list[256] = {0};
	HUFFMAN_NODE* huff_list[256] = {NULL};
	HEAP_NODE** array;
	HEAP_NODE* node;
	unsigned char data;
	HEAP heap;
	fpos_t pad_pos,start;
	
	fgetpos(input,&start);
	while(1){
		data = fgetc(input);
		if(feof(input)) break;
		if(list[data] == 0) list_len += 1;
		list[data] += 1;
	}
	array = malloc(sizeof(HEAP_NODE*)*list_len);	
	init_heap(&heap,array,list_len);

	i=0;
	while(1){
		if(list[i] != 0){
			node = create_heap_node(i,list[i]);
			insert_heap(&heap,node);
		}
		if(i==0xFF) break;
		i+=1;
	}

	fwrite(&heap.size,sizeof(int),1,output);
	fwrite(&heap.last,sizeof(int),1,output);
	fgetpos(output,&pad_pos);
	fwrite(&pad,sizeof(unsigned char),1,output);
	total_len += 8;

	for(l=0;l<heap.size;l++){
		fwrite(&heap.array[l]->priority,sizeof(int),1,output);
		fwrite(&heap.array[l]->data,sizeof(unsigned char),1,output);
		total_len += 5;
	}

	while(heap.last>1){
		node = create_heap_node(0x00,0x00);
		node->left = delete_heap(&heap);
		node->right = delete_heap(&heap);
		node->priority = node->left->priority + node->right->priority;
		insert_heap(&heap,node);
	}

	make_huffman_list(heap.array[1],huff_list,0,0x00000000);
	
	buf = 0;
	buf_pos = 0;
	//fseek(input,0,SEEK_SET);
	fsetpos(input,&start);
	while(1){
		data = fgetc(input);
		if(feof(input)) break;
		if(huff_list[data]!=NULL){
			for(i=0;i<huff_list[data]->size;i++){
				if(buf_pos > 7){
					fputc(buf,output);
					buf=0;
					buf_pos=0;
					total_len += 1;
				}
				buf |= (0x1 & (huff_list[data]->code >> i)) << (7-buf_pos);
				buf_pos+=1;
			}
		}
	}
	pad = 8-buf_pos;
	fputc(buf,output);
	fsetpos(output,&pad_pos);
	fputc(pad,output);
	return total_len;
}


void print_huffman_list(HUFFMAN_NODE** list){
	int j;
	unsigned char i;
	
	i=0;
	while(1){
		if(list[i] != NULL){
			printf("%.2x, size:%d, code:",i,list[i]->size);
			for(j=0;j<list[i]->size;j++){
				printf("%d",0x1&(list[i]->code >> j));
			}
			printf("\n");
		}
		if(i == 0xFF) break;
		i+=1;
	}
}

void make_huffman_list(HEAP_NODE* root, HUFFMAN_NODE** list, int depth, int buf){
	if(root->left == NULL && root->right == NULL){
		list[root->data] = malloc(sizeof(HUFFMAN_NODE));
		list[root->data]->size = depth;
		list[root->data]->code = buf;
		return;
	}
	
	if(root->left != NULL){
		make_huffman_list(root->left,list,depth+1,buf & ~(0x1<<depth));
	}
	
	if(root->right != NULL){
		make_huffman_list(root->right,list,depth+1,buf | 0x1<<depth);
	}
}

int de_huffman(FILE* input, FILE* output){
	int total_len = 0,priority,l;
	unsigned char i,data,buf,buf_pos,pad;
	HEAP_NODE** array;
	HEAP_NODE* node;
	HEAP heap;
	
	fread(&heap.size,sizeof(int),1,input);
	fread(&heap.last,sizeof(int),1,input);
	fread(&pad,sizeof(unsigned char),1,input);
	array = malloc(sizeof(HEAP_NODE*)*heap.size);
	heap.array = array;
	
	for(l=0;l<heap.size;l++){
		fread(&priority,sizeof(int),1,input);
		fread(&data,sizeof(unsigned char),1,input);
		heap.array[l] = create_heap_node(data,priority);
	}
		
	while(heap.last>1){
		node = create_heap_node(0x00,0x00);
		node->left = delete_heap(&heap);
		node->right = delete_heap(&heap);
		node->priority = node->left->priority + node->right->priority;
		insert_heap(&heap,node);
	}
	
	node = heap.array[1];
	while(1){
		buf = fgetc(input);
		if(feof(input)) break;
		for(buf_pos=0;buf_pos<8;buf_pos++){
			if(node->left == NULL && node->right == NULL){
				fputc(node->data,output);
				node = heap.array[1];
				total_len+=1;
			}
			if( 0x1 & (buf >> (7-buf_pos)) ){
				node = node->right;
			}else{
				node = node->left;
			}
		}
	}
	
	return total_len;
}
