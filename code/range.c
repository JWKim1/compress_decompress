#include "compress.h"

int range(FILE* input, FILE* output){
	int total_len = 0;
	unsigned int sum,prime;
	unsigned int bytes_total=0,range_total;
	RANGE_BUF low, range;
	RANGE_NODE range_table[256];
	unsigned char data;
	int i,list_len=0;
	fpos_t start;
	
	fgetpos(input,&start);
	for(i=0;i<256;i++) range_table[i].size = 0;
	
	while(1){
		data = fgetc(input);
		if(feof(input)) break;
		if(range_table[data].size == 0) list_len += 1;
		bytes_total+=1;
		range_table[data].size += 1;
	}
	sum=0;	
	prime = ~0x0;
	for(i=0;i<256;i++){
		sum+=range_table[i].size;
		if(range_table[i].size > 0 && range_table[i].size < prime) 
			prime = range_table[i].size;
	}
	
	range_total = 0xFF;
	while(range_total < bytes_total/prime && range_total<0xFFFFFFFF){
		range_total = (range_total<<8) | 0xFF;
	}

	sum = 0;
	for(i=0;i<256;i++){
		if(range_table[i].size != 0){
			range_table[i].start = sum;
			range_table[i].size = (double)range_total*(double)range_table[i].size/(double)bytes_total;
			if(range_table[i].size == 0) range_table[i].size=1;
			sum+=range_table[i].size;
		}
	}


	fwrite(&bytes_total,sizeof(int),1,output);
	fwrite(&range_total,sizeof(int),1,output);
	fwrite(&list_len,sizeof(int),1,output);

	for(i=0;i<256;i++){
		if(range_table[i].size != 0){
			data = i;
			fwrite(&data,sizeof(char),1,output);
			fwrite(&range_table[i],sizeof(RANGE_NODE),1,output);
			total_len += sizeof(RANGE_NODE);
		}
	}
	
	//fseek(input,0,SEEK_SET);
	fsetpos(input,&start);
	low.data  = 0;
	range.data = ~0x0;
	while(1){
		data = fgetc(input);
		if(feof(input)) break;
		range_encode(&low,&range,&range_table[data],range_total);
		total_len += range_flush(&low,&range,range_total,output);
	}
	
	for(i=7;i>=0 && low.part[i] != 0;i--){
		fwrite(&low.part[i],sizeof(char),1,output);
		total_len += 1;
	}
	return total_len;
}

int de_range(FILE* input, FILE* output){
	int i,cnt,total_len=0,list_len;
	unsigned int bytes_total,range_total;
	unsigned char data,out;
	RANGE_NODE range_table[256];
	RANGE_BUF low, range, buf;
	
	for(i=0;i<256;i++){
		range_table[i].size=0;
	}
	fread(&bytes_total,sizeof(int),1,input);
	fread(&range_total,sizeof(int),1,input);
	fread(&list_len,sizeof(int),1,input);
	for(i=0;i<list_len;i++){
		fread(&data,sizeof(char),1,input);
		fread(&range_table[data],sizeof(RANGE_NODE),1,input);
	}
	low.data=0;
	range.data=~0x0;
	
	for(i=7;i>=0;i--)
		fread(&buf.part[i],sizeof(char),1,input);
	
	while(1){
		out = range_decode(&low,&range,&buf,range_table,range_total);
		cnt = range_flush(&low,&range,range_total,NULL);
		fwrite(&out,sizeof(unsigned char),1,output);
		total_len += 1;
		for(i=0;i<cnt && !feof(input);i++){
			buf.data=buf.data<<8;
			fread(&buf.part[0],sizeof(char),1,input);
		}
		
		if(total_len >= bytes_total) break;
	}
	
	return total_len;
}

void range_encode(RANGE_BUF* low, RANGE_BUF* range, RANGE_NODE* node, unsigned int total){
	range->data /= total;
	low->data += node->start * range->data;
	range->data *= node->size;
}

unsigned char range_decode(RANGE_BUF* plow, RANGE_BUF* prange, RANGE_BUF* buf, RANGE_NODE range_table[256], unsigned int total){
	unsigned char i,res=0;
	RANGE_NODE node;
	RANGE_BUF low;
	RANGE_BUF range;
	RANGE_BUF max;
	
	range.data = prange->data/total;
	
	i=0;
	while(1){
		node = range_table[i];
		if(node.size > 0){
			low.data = plow->data + (node.start * range.data);
			max.data = low.data+(range.data*node.size);
	
			if(buf->data >= low.data && buf->data < max.data){
				plow->data = low.data;
				prange->data = range.data*node.size;
				res = i;
				break;
			}
		}
		if(i==0xFF) break;
		i++;
	}
	return res;
}

int range_flush(RANGE_BUF* low, RANGE_BUF* range, unsigned int total, FILE* output){
	int done_cnt,i;
	int total_bytes = total/0xFF;
	RANGE_BUF buf;
	buf.data = low->data ^ (low->data+range->data);
	
	done_cnt = 0;
	for(i=7;i>=0 && buf.part[i]==0;i--){
		done_cnt+=1;
	}

	for(i=0;i<done_cnt;i++){
		if(output != NULL)
			fwrite(&low->part[7],sizeof(char),1,output);
		low->data = low->data << 8;
		range->data = range->data << 8;
	}
	
	if(range->data < total){
		done_cnt+=total_bytes;
		if(output != NULL){
			for(i=0;i<total_bytes;i++)
				fwrite(&low->part[7-i],sizeof(char),1,output);
		}
		low->data = low->data << (8*total_bytes);
		range->data = (unsigned int)0xFFFFFFFF - low->data; 
	}
	
	return done_cnt;
}
