#ifndef _CFG_FILE_H
#define _CFG_FILE_H

#ifdef __cplusplus 
extern "C" { 
#endif

//成功返回0，失败返回-1
int save_cfg_to_file(char* file,char* cfg, int len);


//从文件读取配置,成功返回0，失败返回-1
int read_cfg_from_file(char* file,char *cfg,int len);
#ifdef __cplusplus 
} 
#endif 

#endif


