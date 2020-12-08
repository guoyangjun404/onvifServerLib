#include <sys/un.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <syslog.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mount.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <netdb.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <linux/if_ether.h>

#include <unistd.h>
#include <mntent.h>
#include <dirent.h>
#include <signal.h>
#include <stdarg.h>
#include <dirent.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

enum {
	MAX_READ = 512,
	DNS_WAIT_SEC = 2 * 60,
};

typedef struct logFile_t {
	char path[64];
	int fd;
	unsigned size;
	unsigned char isRegular;
} logFile_t;
typedef struct JLogFileInfo{
    int isInUse;
	int day;
    char aszFileName[64];
}LogFileInfo;

typedef struct 
{
	char 	TZ[128];								    // required, Posix timezone string
} onvif_TimeZone;

typedef struct
{
	int		Hour;									    // Range is 0 to 23
	int		Minute;									    // Range is 0 to 59
	int		Second;									    // Range is 0 to 61 (typically 59)
} onvif_Time;

typedef struct
{
	int		Year;									    // 
	int		Month;									    // Range is 1 to 12
	int		Day;									    // Range is 1 to 31
} onvif_Date;

typedef struct
{
	onvif_Time	Time;								    // required 
	onvif_Date  Date;								    // required 
} onvif_DateTime;

typedef struct 
{
	unsigned int  TimeZoneFlag	: 1;		    		    // Indicates whether the field TimeZone is valid
	unsigned int	Reserved		: 31;
	
	bool 	DaylightSavings;						    // required, Informative indicator whether daylight savings is currently on/off
	
	int 	DateTimeType;			    // required, Indicates if the time is set manully or through NTP	
	onvif_TimeZone			TimeZone;				    // optional, Timezone information in Posix format 
} onvif_SystemDateTime;

static time_t g_last_log_time = 0;
static char* g_parsebuf = NULL;
static char* g_printbuf = NULL;
static logFile_t g_log_file;
#define MAXLOG_QUEUE 30
static int g_logstatus = 0;
static int log_file_switch_flag = 0;
static int g_alive = 0;
pthread_mutex_t g_alivemutex;
#if !defined(max)
#define max(A,B) ((A) > (B) ? (A):(B))
#endif
#define LOGPATH   "/user/log"
#define GPTPATH   "/user/gpt"

//此参数主要用来判断syslodd进程是否是basegpt的还是gpt，gpt需要统计 重启值如果超过10次自动切换到
static  bool  isbase = false;

char* safe_strncpy(char *dst, const char *src, size_t size)
{
	if (!size) return dst;
	dst[--size] = '\0';
	return strncpy(dst, src, size);
}

static char* last_char_is(const char *s, int c)
{
	if (s && *s) {
		size_t sz = strlen(s) - 1;
		s += sz;
		if ( (unsigned char)*s == c)
			return (char*)s;
	}
	return NULL;
}

ssize_t log_safe_write(int fd, const void *buf, size_t count)
{
	ssize_t n;

	do {
		n = write(fd, buf, count);
	} while (n < 0 && errno == EINTR);

	return n;
}

/*
 * Write all of the supplied buffer out to a file.
 * This does multiple writes as necessary.
 * Returns the amount written, or -1 on an error.
 */
ssize_t  log_full_write(int fd, const void *buf, size_t len)
{
	ssize_t cc;
	ssize_t total;

	total = 0;

	while (len) {
		cc = log_safe_write(fd, buf, len);

		if (cc < 0) {
			if (total) {
				/* we already wrote some! */
				/* user can do another write to know the error code */
				return total;
			}
			return cc;  /* write() returns -1 on failure. */
		}

		total += cc;
		buf = ((const char *)buf) + cc;
		len -= cc;
	}

	return total;
}

/* Print a message to the log file. */
static void log_locally(time_t now, char *msg, logFile_t *log_file)
{
	int len = strlen(msg);
    struct stat statf;
	if (log_file->fd >= 0) {
		/* Reopen log file every second. This allows admin
		 * to delete the file and not worry about restarting us.
		 * This costs almost nothing since it happens
		 * _at most_ once a second.
		 */
		if (!now)
			now = time(NULL);
		if (g_last_log_time != now) {
			g_last_log_time = now;
			close(log_file->fd);
			goto reopen;
		}
	} else {
 reopen:
		log_file->fd = open(log_file->path, O_WRONLY | O_CREAT
					| O_NOCTTY | O_APPEND | O_NONBLOCK,
					0666);
		if (log_file->fd < 0) {
			printf("fail to create the syslog fd\n");
			return;
		}

		log_file->isRegular = (fstat(log_file->fd, &statf) == 0 && S_ISREG(statf.st_mode));
		/* bug (mostly harmless): can wrap around if file > 4gb */
		log_file->size = statf.st_size;
		
	}
	//printf("syslog msg===%s\n", msg);

	log_file->size += log_full_write(log_file->fd, msg, len);
}



/* len parameter is used only for "is there a timestamp?" check.
 * NB: some callers cheat and supply len==0 when they know
 * that there is no timestamp, short-circuiting the test. */
static void timestamp_and_log(int pri, char *msg, int len)
{
	char *timestamp;
	time_t now;

	/* Jan 18 00:11:22 msg... */
	/* 01234567890123456 */
	if (len < 16 || msg[3] != ' ' || msg[6] != ' '
	 || msg[9] != ':' || msg[12] != ':' || msg[15] != ' '
	) {
		time(&now);
		timestamp = (char*)(ctime(&now) + 4); /* skip day of week */
	} else {
		now = 0;
		timestamp = msg;
		msg += 16;
	}
	timestamp[15] = '\0';

	sprintf(g_printbuf, "%s %s\n", timestamp, msg);
	

	/* Log message locally (to file or shared mem) */
	log_locally(now, g_printbuf, &g_log_file);
	
}

static void timestamp_and_log_internal(const char *msg)
{
	timestamp_and_log(LOG_SYSLOG | LOG_INFO, (char*)msg, 0);
}

# define MAXSYMLINKS 20

char* xstrdup(const char *s)
{
	char *t;

	if (s == NULL)
		return NULL;

	t = strdup(s);

	return t;
}
#define isalnum(a) bb_ascii_isalnum(a)
static inline int bb_ascii_isalnum(unsigned char a)
{
	unsigned char b = a - '0';
	if (b <= 9)
		return (b <= 9);
	b = (a|0x20) - 'a';
	return b <= 'z' - 'a';
}

static unsigned long long handle_errors(unsigned long long v, char **endp)
{
	char next_ch = **endp;

	/* errno is already set to ERANGE by strtoXXX if value overflowed */
	if (next_ch) {
		/* "1234abcg" or out-of-range? */
		if (isalnum(next_ch) || errno)
			return 0;
		/* good number, just suspicious terminator */
		errno = EINVAL;
	}
	return v;
}

unsigned long  bb_strtoul(const char *arg, char **endp, int base)
{
	unsigned long v;
	char *endptr;

	if (!endp) endp = &endptr;
	*endp = (char*) arg;

	if (!isalnum(arg[0])) return 0;
	errno = 0;
	v = strtoul(arg, endp, base);
	return handle_errors(v, endp);
}

static inline unsigned bb_strtou(const char *arg, char **endp, int base)
{ 
    return bb_strtoul(arg, endp, base); 
}


/* tmpbuf[len] is a NUL byte (set by caller), but there can be other,
 * embedded NULs. Split messages on each of these NULs, parse prio,
 * escape control chars and log each locally. */
static void split_escape_and_log(char *tmpbuf, int len)
{
	char *p = tmpbuf;

	tmpbuf += len;
	while (p < tmpbuf) {
		char c;
		char *q = g_parsebuf;
		int pri = (LOG_USER | LOG_NOTICE);

		if (*p == '<') {
			/* Parse the magic priority number */
			pri = bb_strtou(p + 1, &p, 10);
			if (*p == '>')
				p++;
			if (pri & ~(LOG_FACMASK | LOG_PRIMASK))
				pri = (LOG_USER | LOG_NOTICE);
		}

		while ((c = *p++)) {
			if (c == '\n')
				c = ' ';
			if (!(c & ~0x1f) && c != '\t') {
				*q++ = '^';
				c += '@'; /* ^@, ^A, ^B... */
			}
			*q++ = c;
		}
		*q = '\0';

		/* Now log it */
		timestamp_and_log(pri, g_parsebuf, q - g_parsebuf);
	}
}

char* xmalloc_readlink(const char *path)
{
	enum { GROWBY = 80 }; /* how large we will grow strings by */

	char *buf = NULL;
	int bufsize = 0, readsize = 0;

	do {
		bufsize += GROWBY;
		buf = realloc(buf, bufsize);
		readsize = readlink(path, buf, bufsize);
		if (readsize == -1) {
			free(buf);
			return NULL;
		}
	} while (bufsize < readsize + 1);

	buf[readsize] = '\0';

	return buf;
}


char* bb_get_last_path_component_nostrip(const char *path)
{
	char *slash = strrchr(path, '/');

	if (!slash || (slash == path && !slash[1]))
		return (char*)path;

	return slash + 1;
}


char*  bb_get_last_path_component_strip(char *path)
{
	char *slash = last_char_is((const char*)path, (int)'/');

	if (slash)
		while (*slash == '/' && slash != path)
			*slash-- = '\0';

	return bb_get_last_path_component_nostrip(path);
}

char* xmalloc_follow_symlinks(const char *path)
{
	char *buf;
	char *lpc;
	char *linkpath;
	int bufsize;
	int looping = MAXSYMLINKS + 1;

	buf = xstrdup(path);
    if (NULL == buf){
        printf("fail to xstrdup\n");
        return NULL;
    }
	goto jump_in;

	while (1) {
		linkpath = xmalloc_readlink(buf);
		if (!linkpath) {
			/* not a symlink, or doesn't exist */
			if (errno == EINVAL || errno == ENOENT)
				return buf;
			goto free_buf_ret_null;
		}

		if (!--looping) {
			free(linkpath);
 free_buf_ret_null:
			free(buf);
			return NULL;
		}

		if (*linkpath != '/') {
			bufsize += strlen(linkpath);
			buf = realloc(buf, bufsize);
			lpc = bb_get_last_path_component_strip(buf);
			strcpy(lpc, linkpath);
			free(linkpath);
		} else {
			free(buf);
			buf = linkpath;
 jump_in:
			bufsize = strlen(buf) + 1;
		}
	}
}

int xsocket(int domain, int type, int protocol)
{
	int r = socket(domain, type, protocol);

	if (r < 0) {
		printf("fail to create socket in syslogd\n");
        return -1;
	}

	return r;
}

void xbind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen)
{
	if (bind(sockfd, my_addr, addrlen))printf("bind error\n");
}


/* Don't inline: prevent struct sockaddr_un to take up space on stack
 * permanently */
static int create_socket(void)
{
	struct sockaddr_un sunx;
	int sock_fd;
	char *dev_log_name;

	memset(&sunx, 0, sizeof(sunx));
	sunx.sun_family = AF_UNIX;

	/* Unlink old /dev/log or object it points to. */
	/* (if it exists, bind will fail) */
	strcpy(sunx.sun_path, _PATH_LOG);
	dev_log_name = xmalloc_follow_symlinks(_PATH_LOG);
	if (dev_log_name) {
		safe_strncpy(sunx.sun_path, dev_log_name, sizeof(sunx.sun_path));
		free(dev_log_name);
	}
	unlink(sunx.sun_path);

	sock_fd = xsocket(AF_UNIX, SOCK_DGRAM, 0);

    //flags = fcntl(sock_fd, F_GETFL, 0);
    //fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK);

    xbind(sock_fd, (struct sockaddr *) &sunx, sizeof(sunx));

	chmod(_PATH_LOG, 0666);

	return sock_fd;
}

void syslogd_signal_handler(int sig)
{
	switch(sig)
	{
        case SIGTERM:
            printf(" get sigterm\n");
            exit(0);
            break;
        case SIGKILL:
            printf(" get sigkill\n");
            break;
	}
}

static int checkusermount()
{
    FILE *f;
	struct mntent *mnt;
    int bmounted = 0;

	if ((f = setmntent ("/proc/mounts", "r")) == NULL)
		return 0;

	while ((mnt = getmntent (f)) != NULL){
		if (strcmp ((const char *)"/user", (const char *)mnt->mnt_dir) == 0) {
            bmounted = 1;
			break;
		}
	}
	endmntent (f);
    
    return bmounted;
}

void getCurrentTime(struct tm *pNowTime)
{
	struct timeval structTimeSpec;
    gettimeofday(&structTimeSpec, NULL);
    localtime_r(&structTimeSpec.tv_sec, pNowTime);
}

static int search_logfile_list(const struct dirent *entry)
{
	if ((strcmp(entry->d_name, ".")== 0) ||
			(strcmp(entry->d_name, "..") == 0)){
			 return (false);
	}

	if (strstr(entry->d_name,"log") != NULL){
		return (true);
	}
	else {
		return (false);
	}
}

/*get and check substring from 0-10 is digital number*/
int log_substring(char *dest, char *src, int start, int end)  
{  
    int i=start;  
    if (start > strlen(src) || src[0] < '0' || src[0] > '9')
        return -1;  
    if (end>strlen(src))  
        end=strlen(src);  
	
    while (i < end) {  
        dest[i-start] = src[i];  
        i++;  
    }  
    dest[i-start]='\0';  
    return 0;  
}

int get_logfile_number()
{
	int totalCount = 0;
	int i = 0;
	char path[64] = {0};

	if (access(LOGPATH, F_OK) != 0) {		 
		printf("LOGPATH path %s is not exist!\n", LOGPATH);
		return -1;
	}
	struct dirent **namelist = NULL;
	
	//默认按文件名称排序
	totalCount = scandir(LOGPATH, &namelist, search_logfile_list, alphasort);
    if (totalCount <= 0) {
		printf("get file number is 0!\n");
        return 0;   /* no record */
    }
	
	printf("file number %d!\n", totalCount);
	if (totalCount > MAXLOG_QUEUE) {
		for (i = 0; i < MAXLOG_QUEUE-totalCount; i++) {
			snprintf(path, sizeof(path), 
					"%s/%s", LOGPATH, namelist[i]->d_name);
			unlink(path);
			printf("unlink path= %s!\n", path);
		}
	}

	if(namelist){
		for (i = 0; i < totalCount; i++) {
			if(namelist[i])
				free(namelist[i]);
		}
		free(namelist);
	}
	
	return totalCount;
}

static int search_corefile_list(const struct dirent *entry)
{
	if ((strcmp(entry->d_name, ".")== 0) ||
			(strcmp(entry->d_name, "..") == 0)){
			 return (false);
	}

	if (strstr(entry->d_name,"core") != NULL){
		return (true);
	}
	else {
		return (false);
	}
}

static int get_corefile_number()
{
	int totalCount = 0;
	int i = 0;
	char path[64] = {0};

	if (access(GPTPATH, F_OK) != 0) {		 
		printf("GPTPATH path %s is not exist!\n", GPTPATH);
		return -1;
	}
	struct dirent **namelist = NULL;
	
	//默认按文件名称排序
	totalCount = scandir(GPTPATH, &namelist, search_corefile_list, alphasort);
    if (totalCount <= 0) {
		printf("get file number is 0!\n");
        return 0;   /* no record */
    }
	
	printf("core file number %d!\n", totalCount);

	if(namelist){
		for (i = 0; i < totalCount; i++) {
			if(namelist[i])
				free(namelist[i]);
		}
		free(namelist);
	}
	
	return totalCount;
}

static void create_corefile()
{
	int corenumber = 0;
	
	char cmdpath[64] = {0};
	struct tm NowTime;
	getCurrentTime(&NowTime);
	
	corenumber = get_corefile_number();

	snprintf(cmdpath, sizeof(cmdpath), 
			"touch %s/%04d%02d%02d-%d.core", GPTPATH, NowTime.tm_year + 1900, 
			NowTime.tm_mon+1, NowTime.tm_mday, corenumber);
	system(cmdpath);
}

void* Thread_CheckTime(void* pArg)
{
	struct tm NowTime;
	log_file_switch_flag = 0;
    int deadnum = 0;
    while (1){
		//此处主要用于切换到下一天的日志并删除大于MAXLOG_QUEUE的日志文件
		getCurrentTime(&NowTime);
		if  ((NowTime.tm_hour == 0 ) && (NowTime.tm_min == 0) && 
			(NowTime.tm_sec >= 0) && (NowTime.tm_sec <= 2)) {
			log_file_switch_flag = 1;
		}
		usleep(30*1000*1000LL);
		
        pthread_mutex_lock(&g_alivemutex);
        if (1 == g_alive){
            deadnum = 0;
            g_alive = 0;            
        }
        else{
            deadnum++;
        }
        pthread_mutex_unlock(&g_alivemutex);
		//心跳异常四分钟重启设备
        if (deadnum > 8){
            printf("main process is dead so reboot device\n");
			if (isbase && 0 == access(GPTPATH, F_OK)) {
				create_corefile();
			}
            system("reboot; sleep 3; reboot -f &");
			exit(0);
        }
    }
}

void init_log_file()
{
	int log_file_num = 0;
    int ret = -1;
	char daystr[16] = {0};
    g_log_file.fd = -1;
    g_log_file.isRegular = 1;
	struct tm NowTime;
	getCurrentTime(&NowTime);
	
	log_file_num = get_logfile_number();
	snprintf(daystr, sizeof(daystr), "%04d%02d%02d", 
				NowTime.tm_year + 1900, 
				NowTime.tm_mon+1, NowTime.tm_mday);
	printf("log_file_num=%d,daystr=%s!!!!\n", log_file_num, daystr);

	snprintf(g_log_file.path, sizeof(g_log_file.path), 
			"%s/%04d%02d%02d.log", LOGPATH, NowTime.tm_year + 1900, 
			NowTime.tm_mon+1, NowTime.tm_mday);
	
    g_log_file.size = 0;

}


//从文件读取配置,成功返回0，失败返回-1
int read_cfg_file(char* file, char *cfg, int len)
{
	int fd;
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

	close(fd);
	return 0;
}

#define  TIMEFILE  ("/user/cfg_files/datatime.dat")

int SetTimeZone()
{
	char cscurTZ[32] = {0};
	char linuxTz[32] = {0};
	onvif_SystemDateTime pDataTimeInfo;
    int ret = -1;

	//默认东八区
    if (read_cfg_file(TIMEFILE, (char *)&pDataTimeInfo, 
		sizeof(onvif_SystemDateTime)) != 0) {
		strcpy(cscurTZ, "CST-8:00:00");
	}
	else {
		strncpy(cscurTZ, pDataTimeInfo.TimeZone.TZ, sizeof(cscurTZ));
	}
	
	sprintf(linuxTz, "GMT%s", &cscurTZ[3]);
	printf("syslogd cscurTZ Tz is %s linuxTZP:%s\n", cscurTZ, linuxTz);	

    setenv("TZ", linuxTz, 1);
	return 0;
}

static void do_syslogd(void)
{
	int sock_fd = -1;
    char* recvbuf = NULL;
    int ret = -1;
	int bmounted = -1;
	pthread_t upintimeth;
    recvbuf = malloc(MAX_READ);
    g_parsebuf = malloc(2*MAX_READ);
    g_printbuf = malloc(MAX_READ*2 + 128);

    g_logstatus = 1;
    pthread_mutex_init(&g_alivemutex, NULL);
	
	///dev/mmcblk0p5 /user ext4 rw,relatime,data=ordered 0 0
    while(1) {
        bmounted = checkusermount();
		//2020-10-10 13:37:50 == 1602308270
		//暂时屏蔽掉时间检查
        if (1 == bmounted /*&& (time(NULL) > 1602308270)*/) {
			printf("time()====%ld\n",  time(NULL));
			break;
        }
		usleep(10*1000*1000LL);
    }

	//判断文件夹是否存在
	if (access(LOGPATH, F_OK) < 0) {
		if (mkdir(LOGPATH, 0755) < 0) {
			return ;
		}
	}
		            
	/* Set up signal handlers (so that they interrupt read()) */
	//signal_no_SA_RESTART_empty_mask(SIGTERM, record_signo);
	//signal_no_SA_RESTART_empty_mask(SIGINT, record_signo);
	//signal_no_SA_RESTART_empty_mask(SIGQUIT, record_signo);
	//signal(SIGHUP, SIG_IGN);
	signal(SIGUSR2, syslogd_signal_handler);
    signal(SIGTERM, syslogd_signal_handler);
    signal(SIGKILL, syslogd_signal_handler);
	
	SetTimeZone();
	
	init_log_file();
	
    ret = pthread_create(&upintimeth, NULL, Thread_CheckTime, NULL);
	sock_fd = create_socket();

	timestamp_and_log_internal("syslogd started: BusyBox v");

	while (1) {
		ssize_t sz;
        fd_set sets;
        struct timeval timeout;

        FD_ZERO(&sets);
        FD_SET(sock_fd, &sets);

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        ret = select(sock_fd + 1, NULL, &sets, NULL, &timeout);
        if (ret > 0) {
            if (0 == FD_ISSET(sock_fd, &sets)) {
               goto read_again;
            }
        } 
        else if (0 == ret) {
            printf("select time out\n");
        }
        else{
            printf("select error so break;\n");
            break;
        }
        
 read_again:
		sz = read(sock_fd, recvbuf, MAX_READ - 1);
		if (sz < 0) {
            int errnum = errno;
            printf("fail to read data in unix socket the error is %s\n", 
                    strerror(errnum));
			break;
		}

		/* Drop trailing '\n' and NULs (typically there is one NUL) */
		while (1) {
			if (sz == 0)
				goto read_again;
			/* man 3 syslog says: "A trailing newline is added when needed".
			 * However, neither glibc nor uclibc do this:
			 * syslog(prio, "test")   sends "test\0" to /dev/log,
			 * syslog(prio, "test\n") sends "test\n\0".
			 * IOW: newline is passed verbatim!
			 * I take it to mean that it's syslogd's job
			 * to make those look identical in the log files. */
			if (recvbuf[sz-1] != '\0' && recvbuf[sz-1] != '\n')
				break;
			sz--;
		}		
		recvbuf[sz] = '\0'; /* ensure it *is* NUL terminated */
		if (NULL != strstr(recvbuf, "SETZONE")){
            log_file_switch_flag = 1;
			SetTimeZone();
        }
		else if (NULL != strstr(recvbuf, "ALIVE")){
			pthread_mutex_lock(&g_alivemutex);
			g_alive = 1;
			pthread_mutex_unlock(&g_alivemutex);
		}
		
        if (1 == log_file_switch_flag) {
			close(g_log_file.fd);
			log_file_switch_flag = 0;
			init_log_file();
        }
        split_escape_and_log(recvbuf, sz);		
	} /* while (!bb_got_signal) */

	timestamp_and_log_internal("syslogd exiting");
}


int main(int argc, char **argv)
{
	printf("argv[1]=%s\n", argv[1]);

    if (NULL != argv[1] && 1 == atoi(argv[1])) {
		isbase = true;
    }
	do_syslogd();	
	
	return 0;
}


