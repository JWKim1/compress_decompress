#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "compress.h"

int main(int argc, char **argv){
	FILE* input;
	FILE* output;
	FILE* tmp;
	char* input_file;
	char type, magic[3]; 
	char *output_file;
	char* ext;
	int res=0;
	fpos_t fp;

	ext = get_ext(argv[1]);
	output_file = get_out_file_name(argv[1],"dcpd");
	
	if(argc != 2){
		printf("error!\n");
		return 0;
	}
	
	input_file = argv[1];
	input = fopen(input_file,"rb");
	output = fopen(output_file,"wb");
	
	if(strcmp(ext,"cpd") != 0 || !input || !output){
		printf("file open error!\n");
		fclose(input);
		fclose(output);
		return 0;
	}
	
	fread(&magic,sizeof(char),3,input);
	fread(&type,sizeof(char),1,input);
	switch(type){
		case XML:
		case HTML:
			tmp = fopen("___tmp___","wb+");
			fgetpos(tmp,&fp);
			res = de_huffman(input,tmp);
			fsetpos(tmp,&fp);
			res = LempelZivDeCompress(tmp,output);
			fclose(tmp);
			break;
		case FNA:
			res = de_range(input,output);
			break;
		case WAV:
			res = de_range(input,output);
			break;
		case BMP:		
			tmp = fopen("___tmp___","wb+");
			fgetpos(tmp,&fp);
			res = de_range(input,tmp);
			fsetpos(tmp,&fp);
			res = de_runlength(tmp,output);
			fclose(tmp);
			break;
	}

	
	if(res < 1){
		printf("decompress error!\n");
		
		fclose(input);
		fclose(output);
		return 0;
	}
	
	printf("decompress complete!\n");
	printf("decompressed length : %d\n",res);
	
	fclose(input);
	fclose(output);
	return 0;
}

/*
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
*/





