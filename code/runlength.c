#include "compress.h"

int runlength(FILE* input, FILE* output){
	int total_len = 0;
	unsigned char len = 1;
	unsigned char max = ~0x0;
	char bfr;
	char now;
	fpos_t key_pos;
	fpos_t out_pos;
	unsigned char key=0x00;
	unsigned char key_in=0;
	
	fgetpos(output,&key_pos);
	fputc(key,output);
	bfr = fgetc(input);
	while(1){
		now = fgetc(input);
		
		if(bfr == now && len!=max) len += 1;
		else{
			fputc(bfr,output);
			if(len > 1){
				fputc(len,output);
				key |= 0x1<<(7-key_in);
				total_len+=1;
			}
			
			key_in+=1;
			if(key_in>7){
				fgetpos(output,&out_pos);
				fsetpos(output,&key_pos);
				fputc(key,output);
				key_in = 0;
				key=0x00;
				fsetpos(output,&out_pos);
				fputc(key,output);
				key_pos = out_pos;
			}
			
			total_len+=1;
			len = 1;
		}
		if(feof(input)){
			fsetpos(output,&key_pos);
			fputc(key,output);
			break;	
		}
		bfr = now;
	}
	
	return total_len;
}

int de_runlength(FILE* input, FILE* output){
	int total_len = 0;
	unsigned char len;
	unsigned char i;
	unsigned char key;
	unsigned char key_in;
	char now;
	
	key_in = 0;
	key = fgetc(input);
	
	while(1){
		len = 1;
		now = fgetc(input);
		if(0x1 & key>>(7-key_in) ){
			len = fgetc(input);
		}		
		
		if(feof(input)) break;
		
		for(i=0;i<len;i++){
			fputc(now,output);
			total_len += 1;
		}
		
		key_in+=1;
		if(key_in > 7){
			key = fgetc(input);
			key_in = 0;
		}
	}
	
	return total_len;
}
