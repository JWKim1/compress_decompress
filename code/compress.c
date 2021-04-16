#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "compress.h"

int main(int argc, char **argv){
	FILE* input;
	FILE* output;
	FILE* tmp,tmp2;
	char* input_file;
	char* output_file;
	char* ext;
	int res=0;
	char type=-1;
	fpos_t fp,fp2;
	
	if(argc != 2){
		printf("error!\n");
		return 0;
	}
	
	input_file = argv[1];
	ext = get_ext(input_file);
	output_file = get_out_file_name(input_file,"cpd");
	
	if(strcmp(ext,"xml") == 0){
		type = XML;
	}else if(strcmp(ext,"html") == 0){
		type = HTML;
	}else if(strcmp(ext,"fna") == 0){
		type = FNA;
	}else if(strcmp(ext,"wav") == 0){
		type = WAV;
	}else if(strcmp(ext,"bmp") == 0){
		type = BMP;
	}
	
	input = fopen(input_file,"rb");
	output = fopen(output_file,"wb");
	
	if(type==-1 || !input || !output){
		printf("file open error!\n");
		fclose(input);
		fclose(output);
		return 0;
	}
	
	fputs("CMP",output);
	fwrite(&type,sizeof(char),1,output);
	
	switch(type){
		case XML:
		case HTML:
			tmp = fopen("___tmp___","wb+");
			fgetpos(tmp,&fp);
			res = LempelZivCompress(input,tmp);
			fsetpos(tmp,&fp);
			res = huffman(tmp,output);
			fclose(tmp);
			break;
		case FNA:
			res = range(input,output);
			break;
		case WAV:
			res = range(input,output);
			break;
		case BMP:
			tmp = fopen("___tmp___","wb+");
			fgetpos(tmp,&fp);
			res = runlength(input,tmp);
			fsetpos(tmp,&fp);
			res = range(tmp,output);
			fclose(tmp);
			break;
	}
	
	
	if(res < 1){
		printf("compress error!\n");
		fclose(input);
		fclose(output);
		return 0;
	}
	
	printf("compress complete!\n");
	printf("compressed length : %d\n",res);
	
	fclose(input);
	fclose(output);
	
	return 0;
}