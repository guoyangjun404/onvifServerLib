/***************************************************************************************
 *
 *  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
 *
 *  By downloading, copying, installing or using the software you agree to this license.
 *  If you do not agree to this license, do not download, install, 
 *  copy or use the software.
 *
 *  Copyright (C) 2014-2019, Happytimesoft Corporation, all rights reserved.
 *
 *  Redistribution and use in binary forms, with or without modification, are permitted.
 *
 *  Unless required by applicable law or agreed to in writing, software distributed 
 *  under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 *  CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
 *  language governing permissions and limitations under the License.
 *
****************************************************************************************/

#include "sys_inc.h"
#include "sys_log.h"

/***************************************************************************************/
static FILE * g_pLogFile  = NULL;
static void * g_pLogMutex = NULL;
static int    g_log_level = LOG_ERR;

static const char * g_log_level_str[] = 
{
	"TRACE",
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"FATAL"
};

/***************************************************************************************/
int log_init(const char * log_fname)
{
	log_close();
	
	g_pLogFile = fopen(log_fname, "w+");
	if (g_pLogFile == NULL)
	{
		printf("log init fopen[%s] failed[%s]\r\n", log_fname, strerror(errno));
		return -1;
	}

	g_pLogMutex = sys_os_create_mutex();
	if (g_pLogMutex == NULL)
	{
		printf("log init mutex failed[%s]\r\n", strerror(errno));
		return -1;
	}
	
	return 0;
}

int log_time_init(const char * fname_prev)
{
	char fpath[256];
	time_t time_now = time(NULL);
	struct tm * st = localtime(&(time_now));
	
	sprintf(fpath, "%s-%04d%02d%02d_%02d%02d%02d.txt", fname_prev, 
		st->tm_year+1900, st->tm_mon+1, st->tm_mday, st->tm_hour, st->tm_min, st->tm_sec);
	
	return log_init(fpath);
}

int log_reinit(const char * log_fname)
{
    sys_os_mutex_enter(g_pLogMutex);
    
	if (g_pLogFile)
	{
		fclose(g_pLogFile);
		g_pLogFile = NULL;
	}

    g_pLogFile = fopen(log_fname, "w+");
	if (g_pLogFile == NULL)
	{
		printf("log init fopen[%s] failed[%s]\r\n", log_fname, strerror(errno));
		return -1;
	}
	
    sys_os_mutex_leave(g_pLogMutex);

    return 0;
}

int log_time_reinit(const char * fname_prev)
{
    char fpath[256];
	time_t time_now = time(NULL);
	struct tm * st = localtime(&(time_now));
	
	sprintf(fpath, "%s-%04d%02d%02d_%02d%02d%02d.log", fname_prev, 
		st->tm_year+1900, st->tm_mon+1, st->tm_mday, st->tm_hour, st->tm_min, st->tm_sec);
	
	return log_reinit(fpath);
}

void log_close()
{
    sys_os_mutex_enter(g_pLogMutex);
    
	if (g_pLogFile)
	{
		fclose(g_pLogFile);
		g_pLogFile = NULL;
	}

    sys_os_mutex_leave(g_pLogMutex);
    
	if (g_pLogMutex)
	{
		sys_os_destroy_sig_mutx(g_pLogMutex);
		g_pLogMutex = NULL;
	}
}

int _log_print(int level, const char *fmt, va_list argptr)
{
	int slen = 0;
	time_t time_now;
	struct tm * st;

	if (g_pLogFile == NULL || g_pLogMutex == NULL)
	{
		return 0;
	}
	
	time_now = time(NULL);
	st = localtime(&(time_now));
		
	sys_os_mutex_enter(g_pLogMutex);

	if (g_pLogFile)
    {
    	fprintf(g_pLogFile, "[%04d-%02d-%02d %02d:%02d:%02d] : [%s] ", 
    		st->tm_year+1900, st->tm_mon+1, st->tm_mday, st->tm_hour, st->tm_min, st->tm_sec, 
    		g_log_level_str[level]);
    	
    	slen = vfprintf(g_pLogFile,fmt,argptr);
    	fflush(g_pLogFile);
    }
    
	sys_os_mutex_leave(g_pLogMutex);

	return slen;
}

#ifndef IOS

int log_print(int level, const char * fmt,...)
{
    if (level < g_log_level || level > LOG_FATAL)
    {
        return 0;
    }
    else
    {
        int slen;
        va_list argptr;

        va_start(argptr,fmt);

        slen = _log_print(level, fmt, argptr);

        va_end(argptr);

        return slen;
    }
}

#else

int log_printfff(int level, const char * fmt,...)
{
    if (level < g_log_level || level > LOG_FATAL)
    {
        return 0;
    }
    else
    {
        int slen;
        va_list argptr;

        va_start(argptr,fmt);

        slen = _log_print(level,fmt, argptr);

        va_end(argptr);

        return slen;
    }
    
    return 0;
}

#endif

static int _log_lock_print(const char *fmt, va_list argptr)
{
	int slen;

	if (g_pLogFile == NULL || g_pLogMutex == NULL)
	{
		return 0;
	}
	
	slen = vfprintf(g_pLogFile, fmt, argptr);
	fflush(g_pLogFile);
	return slen;
}

int log_lock_start(const char * fmt,...)
{
	int slen = 0;
	va_list argptr;		

	if (g_pLogFile == NULL || g_pLogMutex == NULL)
		return 0;

	va_start(argptr,fmt);
	
	sys_os_mutex_enter(g_pLogMutex);
	
	slen = _log_lock_print(fmt,argptr);

	va_end(argptr);

	return slen;
}

int log_lock_print(const char * fmt,...)
{
	int slen;

	va_list argptr;
	va_start(argptr,fmt);

	slen = _log_lock_print(fmt, argptr);

	va_end(argptr);
	
	return slen;

}

int log_lock_end(const char * fmt,...)
{
	int slen;

	va_list argptr;
	va_start(argptr,fmt);

	slen = _log_lock_print(fmt, argptr);

	va_end(argptr);

	sys_os_mutex_leave(g_pLogMutex);

	return slen;
}

void log_set_level(int level)
{
	g_log_level = level;
}

int log_get_level()
{
	return g_log_level;
}



