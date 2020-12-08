#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <time.h>
#include <fcntl.h>

#include <sys/stat.h> 

#ifdef __cplusplus
extern "C" {
#endif


//结构体版本
#define CFG_FOLDER					("/user/cfg_files/")
#define MAGIC_NUM   				(0x5a5a5a5a)



//创建多级目录，成功返回0，失败返回-1
static int create_multi_dir(const char *path)
{
	int i, len;

	len = strlen(path);
	char dir_path[len+1];
	dir_path[len] = '\0';

	strncpy(dir_path, path, len);
	for (i=1; i<len; i++)
	{
		if (dir_path[i] == '/' )
		{
			dir_path[i]='\0';
			if (access(dir_path, F_OK) < 0)
			{
				if (mkdir(dir_path, 0755) < 0)
					return -1;
			}
			dir_path[i]='/';
		}
	}
	return 0;
}


//成功返回0，失败返回-1
int save_cfg_to_file(char* file,char* cfg, int len)
{
	int fd;
	int MagicNum = MAGIC_NUM;


	//判断文件夹是否存在
	if (access(CFG_FOLDER, F_OK) < 0)
		if(create_multi_dir(CFG_FOLDER)==0)//创建多级目录，成功返回1，失败返回0
		{
			return -1;
		}


	//创建文件
	fd=open(file,O_WRONLY|O_CREAT,777);
	if(fd<=0)
		return -1;

	//写入结构体信息
	if(write(fd,cfg, len) != len)
	{
		close(fd);
		return -1;
	}

	//写入魔数
	if(write(fd,&MagicNum, sizeof(MagicNum)) != sizeof(MagicNum) )
	{
		close(fd);
		return -1;
	}

	fsync(fd);
	close(fd);
	return 0;
}


//从文件读取配置,成功返回0，失败返回-1
int read_cfg_from_file(char* file,char *cfg,int len)
{
	int fd;
	unsigned int MagicNum;

    if (!file || (file && 0 != access(file, F_OK))) {
		return -1;
    }
	
	fd = open(file,O_RDONLY);
	if (fd<=0)
		return -1;

	//读入配置
	if (read(fd,cfg,len) != len)
	{
		close(fd);
		return -1;
	}

	//读入魔数
	if(read(fd,&MagicNum,sizeof(MagicNum)) != sizeof(MagicNum))
	{
		close(fd);
		return -1;
	}

	//对比魔数
	if(MagicNum!=MAGIC_NUM)
	{
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}

#ifdef __cplusplus 
} 
#endif 


