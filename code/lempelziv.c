#include "compress.h"

int LempelZivCompress(FILE* input, FILE* output){
	LZCode code;
	char data;
	int i,key_index=0;
	fpos_t key_pos;
	char key=0;
	
	Queue dict;
	Queue buffer;
	
	InitQueue(&dict,QUEUE_SIZE);
	InitQueue(&buffer,QUEUE_SIZE);
	
	while(!QUEUE_FULL(buffer)){
		fread(&data,sizeof(char),1,input);
		if(feof(input))break;
		PushQueue(&buffer,data);
	}
	
	fgetpos(output,&key_pos);
	fwrite(&key,sizeof(char),1,output);
	
	while(1){
		code = LempelZivEncode(&dict,&buffer);
		if(code.len<7){
			if(code.len==0) code.len = 1;
			for(i=0;i<code.len;i++){
				data = LempelZivMoveWindow(&dict, &buffer, input);
				WriteKey(&key,&key_index,&key_pos,0x1,output);
				fwrite(&data,sizeof(char),1,output);
			}
		}
		else{
			WriteKey(&key,&key_index,&key_pos,0x0,output);
			fwrite(&code,sizeof(LZCode),1,output);
			for(i=0;i<code.len;i++){
				LempelZivMoveWindow(&dict, &buffer, input);
			}
		}
		if(QUEUE_EMPTY(buffer)) break;
	}
	
	if(key_index > 0){
		FlushKey(&key, &key_pos, output);
	}
	
	return 1;
}

int LempelZivDeCompress(FILE* input, FILE* output){
	LZCode code;
	char data;
	int i,value,key_index=0;
	char key=0;
	
	Queue dict;
	Queue buffer;
	
	InitQueue(&dict,QUEUE_SIZE);
	InitQueue(&buffer,QUEUE_SIZE);
	
	fread(&key,sizeof(char),1,input);
	
	while(1){
		value=ReadKey(&key,&key_index,input);
		if(value){
			fread(&data,sizeof(char),1,input);
			if(feof(input)) break;
			if(QUEUE_FULL(dict))PopQueue(&dict);
			PushQueue(&dict,data);
			fwrite(&data,sizeof(char),1,output);
		}else{
			fread(&code,sizeof(LZCode),1,input);
			if(feof(input)) break;
			code = LempelZivDecode(&dict,code);

			for(i=0;i<code.len;i++){
				data = dict.buf[(code.pos+i)%dict.size];
				if(QUEUE_FULL(dict))PopQueue(&dict);
				PushQueue(&dict,data);
				fwrite(&data,sizeof(char),1,output);
			}
		}
	}
	
	return 1;
}

void FlushKey(char* key, fpos_t* key_pos, FILE* output){
	fpos_t tmp;
	fgetpos(output,&tmp);
	fsetpos(output,key_pos);
	fwrite(key,sizeof(char),1,output);
	fsetpos(output,&tmp);
}

void WriteKey(char* key,int* key_index,fpos_t* key_pos,int value,FILE* output){
	if(*key_index > 7){
		FlushKey(key,key_pos,output);
		fgetpos(output,key_pos);
		*key = 0;
		*key_index = 0;	
		fwrite(key,sizeof(char),1,output);
	}
	
	if(value){
		*key |= 0x1<<(7-*key_index);
	}
	
	*key_index+=1;
}

int ReadKey(char* key,int* key_index,FILE* input){
	int value;
	
	if(*key_index > 7){
		fread(key,sizeof(char),1,input);
		*key_index = 0;
	}
	
	value = 0x1 & (*key>>(7-*key_index));

	*key_index+=1;
	
	return value;
}

char LempelZivMoveWindow(Queue* dict, Queue* buffer, FILE* input){
	char data,res;
	
	res=PopQueue(buffer);
	if(QUEUE_FULL(*dict))PopQueue(dict);
	PushQueue(dict,res);
	while(!QUEUE_FULL(*buffer)){
		fread(&data,sizeof(char),1,input);
		if(feof(input)) break;
		PushQueue(buffer,data);
	}
	return res;
}

LZCode LempelZivDecode(Queue* dict, LZCode code){
	LZCode res;

	res.pos = code.pos;
	res.len = code.len;
	
	return res;
}

LZCode LempelZivEncode(Queue* dict, Queue* buffer){
	int current = buffer->front;
	int start = dict->front;
	int i,j;
	LZCode code;
	code.len=0;
	code.pos=0;
	
	for(i=start ; i%dict->size!=dict->rear ; i++){
		if(buffer->buf[current] == dict->buf[i%dict->size]){
			j=1;
			while(	buffer->buf[ (current+j)%buffer->size] == dict->buf[(i+j)%dict->size] && 
					(current+j)%buffer->size != buffer->rear &&
					(i+j)%dict->size != dict->rear	){
				j+=1;
			}
			if(code.len < j){
				code.len = j;
				code.pos = i%dict->size;
			}
		}
	}
	
	return code;
}

void InitQueue(Queue* queue,int size){
	queue->buf = (char*)malloc(sizeof(char)*size);
	queue->size = size;
	queue->front=queue->rear=0;
}

char PushQueue(Queue* q, char _data){
	q->buf[q->rear++]=_data;
	q->rear=q->rear%q->size;
	return _data;
}

char PopQueue(Queue* q){
	char pop=q->buf[q->front++];
	q->front=q->front%q->size;
	return pop;
}