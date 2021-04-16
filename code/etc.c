#include "compress.h"

char* get_out_file_name(char* str, char* ext){
	int i,len,dot_pos,ext_size;	
	char* out_file_name;
	
	ext_size = strlen(ext);
	len = strlen(str);
	dot_pos=len;
	for(i=len-1;i>=0;i--){
		if(str[i] == '.'){
			dot_pos = i;
			break; 
		}
	}
	
	out_file_name = malloc(sizeof(char) * (dot_pos+1+ext_size+1));
	strcpy(out_file_name,str);
	strcpy(out_file_name+dot_pos,".");
	strcpy(out_file_name+dot_pos+1,ext);
	
	return out_file_name;
}

char* get_ext(char* str){
	int i,len,dot_pos;
	char* ext;
	
	len = strlen(str);
	dot_pos = len;
	for(i=len-1;i>=0;i--){
		if(str[i] == '.'){
			dot_pos = i;
			break;
		}
	}
	
	ext = (char*)malloc(sizeof(char)* (len-dot_pos));
	strcpy(ext,str+dot_pos+1);
	
	return ext;
}