#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	XML 0
#define	HTML 1
#define	FNA 2
#define	WAV 3
#define	BMP 4

#define QUEUE_SIZE 100000
#define QUEUE_POS(q,x) (((x)+(q).size)%(q).size)
#define QUEUE_EMPTY(q) ((q).front==(q).rear)
#define QUEUE_FULL(q) (((q).rear+1)%(q).size==(q).front)

typedef struct heap_node{
	unsigned char data;
	int priority;
	struct heap_node* left;
	struct heap_node* right;
} HEAP_NODE;

typedef struct heap{
	HEAP_NODE** array;
	int last;
	int size;
} HEAP;

typedef struct huffman_node{
	int code;
	int size;
} HUFFMAN_NODE;

typedef struct RANGE_NODE{
	unsigned int start;
	unsigned int size;
} RANGE_NODE; 

typedef union RANGE_BUF{
	unsigned long long data;
	unsigned char part[8];
} RANGE_BUF;

typedef struct LZCode{
	int pos;
	int len;
}LZCode;

typedef struct Queue{
	char* buf;
	int front;
	int rear;
	int size;
}Queue;

void InitQueue(Queue* queue, int size);
char PushQueue(Queue* queue, char _data);
char PopQueue(Queue* queue);

void FlushKey(char* key, fpos_t* key_pos, FILE* output);
void WriteKey(char* key,int* key_index,fpos_t* key_pos,int value,FILE* output);
int ReadKey(char* key,int* key_index,FILE* input);

LZCode LempelZivEncode(Queue* dict, Queue* buffer);
LZCode LempelZivDecode(Queue* dict, LZCode code);
int LempelZivCompress(FILE* input, FILE* output);
int LempelZivDeCompress(FILE* input, FILE* output);
char LempelZivMoveWindow(Queue* dict, Queue* buffer, FILE* input);

int runlength(FILE* input, FILE* output);
int huffman(FILE* input, FILE* output);
int range(FILE* input, FILE* output);

int de_runlength(FILE* input, FILE* output);
int de_huffman(FILE* input, FILE* output);
int de_range(FILE* input, FILE* output);

void print_huffman_list(HUFFMAN_NODE** list);
void make_huffman_list(HEAP_NODE* root, HUFFMAN_NODE** list, int depth, int buf);

int range_flush(RANGE_BUF* low, RANGE_BUF* range, unsigned int total, FILE* output);
void range_encode(RANGE_BUF* low, RANGE_BUF* range, RANGE_NODE* node, unsigned int total);
unsigned char range_decode(RANGE_BUF* plow, RANGE_BUF* prange, RANGE_BUF* buf, RANGE_NODE range_table[256], unsigned int total);

void init_heap(HEAP* heap, HEAP_NODE** array, int size);
void insert_heap(HEAP* heap, HEAP_NODE* node);
HEAP_NODE* delete_heap(HEAP* heap);
HEAP_NODE* create_heap_node(unsigned char data, int priority);

char* get_out_file_name(char* str, char* ext);
char* get_ext(char* str);
