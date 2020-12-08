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
#include "util.h"

/***************************************************************************************/
int get_if_nums()
{
#if __WINDOWS_OS__

	char ipt_buf[512];
	MIB_IPADDRTABLE * ipt = (MIB_IPADDRTABLE *)ipt_buf;
	ULONG ipt_len = sizeof(ipt_buf);
	DWORD fr = GetIpAddrTable(ipt, &ipt_len, FALSE);
	if (fr != NO_ERROR)
	{
		return 0;
	}
	
	return ipt->dwNumEntries;
	
#elif __LINUX_OS__

	SOCKET socket_fd;
	struct ifconf conf;
	char buff[BUFSIZ];
	int num;
	
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_fd <= 0)
	{
		return 0;
	}
	
	conf.ifc_len = BUFSIZ;
	conf.ifc_buf = buff;
	
	ioctl(socket_fd, SIOCGIFCONF, &conf);
	
	num = conf.ifc_len / sizeof(struct ifreq);

	closesocket(socket_fd);
	
	return num;
	
#endif

	return 0;
}

uint32 get_if_ip(int index)
{
#if __WINDOWS_OS__

	char ipt_buf[512];
	DWORD i;
	MIB_IPADDRTABLE * ipt = (MIB_IPADDRTABLE *)ipt_buf;
	ULONG ipt_len = sizeof(ipt_buf);
	DWORD fr = GetIpAddrTable(ipt, &ipt_len, FALSE);
	if (fr != NO_ERROR)
	{
		return 0;
	}
	
	for (i=0; i<ipt->dwNumEntries; i++)
	{
		if (i == index)
		{
			return ipt->table[i].dwAddr;
		}	
	}
	
#elif __LINUX_OS__

	int i;
	SOCKET socket_fd;
	struct ifreq *ifr;
	struct ifconf conf;
	char buff[BUFSIZ];
	int num;
	
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	conf.ifc_len = BUFSIZ;
	conf.ifc_buf = buff;
	
	ioctl(socket_fd, SIOCGIFCONF, &conf);
	
	num = conf.ifc_len / sizeof(struct ifreq);
	ifr = conf.ifc_req;

	uint32 ip_addr = 0;
	
	for (i=0; i<num; i++)
	{
		if (i == index)
		{
			struct sockaddr_in *sin = (struct sockaddr_in *)(&ifr->ifr_addr);

			ioctl(socket_fd, SIOCGIFFLAGS, ifr);
			if ((ifr->ifr_flags & IFF_LOOPBACK) != 0)
			{
				ip_addr = 0;
			}
			else
			{
				ip_addr = sin->sin_addr.s_addr;
			}

			break;
		}
		
		ifr++;
	}

	closesocket(socket_fd);
	
	return ip_addr;
	
#endif

	return 0;
}

uint32 get_route_if_ip(uint32 dst_ip)
{
#if __WINDOWS_OS__

	DWORD i;
	DWORD dwIfIndex,fr;
	char ipt_buf[512];
	MIB_IPADDRTABLE *ipt;
	ULONG ipt_len;

	fr = GetBestInterface(dst_ip, &dwIfIndex);
	if (fr != NO_ERROR)
	{
		return 0;
	}
	
	ipt = (MIB_IPADDRTABLE *)ipt_buf;
	ipt_len = sizeof(ipt_buf);
	fr = GetIpAddrTable(ipt,&ipt_len,FALSE);
	if (fr != NO_ERROR)
	{
		return 0;
	}
	
	for (i=0; i<ipt->dwNumEntries; i++)
	{
		if (ipt->table[i].dwIndex == dwIfIndex)
		{
			return ipt->table[i].dwAddr;
		}	
	}
	
#elif __LINUX_OS__

	int i;
	SOCKET socket_fd;
	struct ifreq *ifr;
	struct ifconf conf;
	char buff[BUFSIZ];
	int num;
	
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	conf.ifc_len = BUFSIZ;
	conf.ifc_buf = buff;
	
	ioctl(socket_fd, SIOCGIFCONF, &conf);
	
	num = conf.ifc_len / sizeof(struct ifreq);
	ifr = conf.ifc_req;

	uint32 ip_addr = 0;
	
	for (i=0; i<num; i++)
	{
		struct sockaddr_in *sin = (struct sockaddr_in *)(&ifr->ifr_addr);
		
		if (ifr->ifr_addr.sa_family != AF_INET)
		{
			ifr++;
			continue;
		}

		ioctl(socket_fd, SIOCGIFFLAGS, ifr);
		
		if (((ifr->ifr_flags & IFF_LOOPBACK) == 0) && (ifr->ifr_flags & IFF_UP))
		{
			ip_addr = sin->sin_addr.s_addr;
			break;
		}
		
		ifr++;
	}

	closesocket(socket_fd);
	
	return ip_addr;
	
#endif

	return 0;
}

uint32 get_default_if_ip()
{
	struct in_addr addr;
    addr.s_addr = get_route_if_ip(0);

    if (addr.s_addr != 0)
    {
    	return addr.s_addr;
    }
    else
    {
        int i;
    	int nums = get_if_nums();
    	
    	for (i = 0; i < nums; i++)
    	{
    		addr.s_addr = get_if_ip(i);
    		if (addr.s_addr != 0)
    		{
    			return addr.s_addr;
    		}
    	}
    }

    return 0;
}

int get_default_if_mac(uint8 * mac)
{
#ifdef IOS

#elif __WINDOWS_OS__

	IP_ADAPTER_INFO AdapterInfo[16];            // Allocate information for up to 16 NICs  
    DWORD dwBufLen = sizeof(AdapterInfo);       // Save the memory size of buffer  
  
    DWORD dwStatus = GetAdaptersInfo(           // Call GetAdapterInfo  
        AdapterInfo,                            // [out] buffer to receive data  
        &dwBufLen);                             // [in] size of receive data buffer  
  
    PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info  
	if (pAdapterInfo)
	{
        memcpy(mac, pAdapterInfo->Address, 6);	
        return 0;
    }  
    
#elif __LINUX_OS__

	int i;
	SOCKET socket_fd;
	struct ifreq *ifr;
	struct ifreq ifreq;
	struct ifconf conf;
	char buff[BUFSIZ];
	int num;
	
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	conf.ifc_len = BUFSIZ;
	conf.ifc_buf = buff;
	
	ioctl(socket_fd, SIOCGIFCONF, &conf);
	
	num = conf.ifc_len / sizeof(struct ifreq);
	ifr = conf.ifc_req;
	
	for (i=0; i<num; i++)
	{
		if (ifr->ifr_addr.sa_family != AF_INET)
		{
			ifr++;
			continue;
		}
		
		ioctl(socket_fd, SIOCGIFFLAGS, ifr);
		
		if ((ifr->ifr_flags & IFF_LOOPBACK) != 0)
		{
			ifr++;
			continue;
		}

		strncpy(ifreq.ifr_name, ifr->ifr_name, sizeof(ifreq.ifr_name));

        if (ioctl(socket_fd, SIOCGIFHWADDR, &ifreq) < 0) 
        {
        	ifr++;
            continue;
        }

		memcpy(mac, &ifreq.ifr_hwaddr.sa_data, 6);		

		close(socket_fd);
		
		return 0;
	}

	close(socket_fd);
#endif	

	return -1;
}

const char * get_default_gateway()
{   
	static char gateway[32];	
	
#if __WINDOWS_OS__

	IP_ADAPTER_INFO AdapterInfo[16];            // Allocate information for up to 16 NICs  
    DWORD dwBufLen = sizeof(AdapterInfo);       // Save the memory size of buffer  
  
    DWORD dwStatus = GetAdaptersInfo(           // Call GetAdapterInfo  
        AdapterInfo,                            // [out] buffer to receive data  
        &dwBufLen);                             // [in] size of receive data buffer  
  
    PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info  
	if (NULL == pAdapterInfo)
	{		
        return NULL;
    } 

	memset(gateway, 0, sizeof(gateway));
	
    strncpy(gateway, pAdapterInfo->GatewayList.IpAddress.String, sizeof(gateway)-1);

#elif __LINUX_OS__
	
    char line[100], *p, *c, *g, *saveptr;
	int ret = 0;
	
    FILE *fp = fopen("/proc/net/route" , "r");
	if (NULL == fp)
	{
		return NULL;
	}

	memset(gateway, 0, sizeof(gateway));
	
    while (fgets(line, 100, fp))
    {
        p = strtok_r(line, " \t", &saveptr);
        c = strtok_r(NULL, " \t", &saveptr);
        g = strtok_r(NULL, " \t", &saveptr);

        if (p != NULL && c != NULL)
        {
            if (strcmp(c, "00000000") == 0)
            {
                if (g)
                {
                    char *p_end;
                    int gw = strtol(g, &p_end, 16);
                    
                    struct in_addr addr;
                    addr.s_addr = gw;
                    
                    strcpy(gateway, inet_ntoa(addr));
                    ret = 1;
                }
                
                break;
            }
        }
    }

	fclose(fp);
	
    if (ret == 0)
    {
    	return NULL;
    }

#endif

	return gateway;
}

const char * get_dns_server()
{
	static char dns[32];

#if __WINDOWS_OS__

	IP_ADAPTER_ADDRESSES addr[16], *paddr;
	DWORD len = sizeof(addr);

	memset(dns, 0, sizeof(dns));
	
	if (NO_ERROR == GetAdaptersAddresses(AF_INET, 0, 0, addr, &len) && len >= sizeof(IP_ADAPTER_ADDRESSES))
	{
		paddr = addr;
		
		while (paddr)
		{
			PIP_ADAPTER_DNS_SERVER_ADDRESS p_ipaddr;
				
			if (paddr->IfType & IF_TYPE_SOFTWARE_LOOPBACK)
			{
				paddr = paddr->Next;
				continue;
			}

			p_ipaddr = paddr->FirstDnsServerAddress;
			if (p_ipaddr)
			{
				struct sockaddr_in * p_inaddr = (struct sockaddr_in *)p_ipaddr->Address.lpSockaddr;
				strcpy(dns, inet_ntoa(p_inaddr->sin_addr));

				break;
			}	

			paddr = paddr->Next;
		}
	}	

#elif __LINUX_OS__

	memset(dns, 0, sizeof(dns));

	// todo : parese /etc/resolv.conf file to get dns server
	if (NULL != get_default_gateway()) {
		strcpy(dns, get_default_gateway());
	}
	else {
		return NULL;
	}
#endif

	return dns;
}

const char * get_mask_by_prefix_len(int len)
{
	int i;
    static char mask_str[32] = {'\0'};    
    uint32 mask = 0;
	
    for (i = 0; i < len; i++)
    {
        mask |= (1 << (31 - i));
    }

    memset(mask_str, 0, sizeof(mask_str));
    sprintf(mask_str, "%u.%u.%u.%u", (mask & 0xFF000000) >> 24, (mask & 0x00FF0000) >> 16, 
        (mask & 0x0000FF00) >> 8, (mask & 0x000000FF));

    return mask_str;    
}

int get_prefix_len_by_mask(const char * mask)
{
	int i;
	int len = 0;
    uint32 n = inet_addr(mask);
	
    n = ntohl(n);    

    for (i = 0; i < 32; i++)
    {
        if (n & (1 << (31 - i)))
        {
            len++;
        }
        else
        {
            break;
        }
    }

    return len;
}

const char * get_ip_str(uint32 ipaddr /* network byte order */)
{
	struct in_addr addr;

	addr.s_addr = ipaddr;

	return inet_ntoa(addr);
}

uint32 get_address_by_name(const char * host_name)
{
	uint32 addr = 0;

	if (is_ip_address(host_name))
	{
		addr = inet_addr(host_name);
	}	
	else
	{
		struct hostent * remoteHost = gethostbyname(host_name);
		if (remoteHost)
		{
			addr = *(unsigned long *)(remoteHost->h_addr);
		}	
	}

	return addr;
}

char * lowercase(char * str) 
{
	uint32 i;
	
	for (i = 0; i < strlen(str); ++i)
	{
		str[i] = tolower(str[i]);
	}
	
	return str;
}

char * uppercase(char * str)
{
	uint32 i;
	
	for (i = 0; i < strlen(str); ++i)
	{
		str[i] = toupper(str[i]);
	}
	
	return str;
}


int unicode(char ** dst, char * src) 
{
	char *ret;
	int l, i;
	
	if (!src) 
	{
		*dst = NULL;
		return 0;
	}
	
	l = MIN(64, (int)strlen(src));
	ret = (char *)malloc(2*l);

	for (i = 0; i < l; ++i)
	{
		ret[2*i] = src[i];
		ret[2*i+1] = '\0';
	}

	*dst = ret;
	
	return 2*l;
}

static char hextab[17] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 0};
static int hexindex[128] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,0,1,2,3,4,5,6,7,8,9,-1,-1,-1,-1,-1,-1,
        -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,10,11,12,13,14,15,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

char * printmem(char * src, size_t len, int bitwidth) 
{
	char *tmp;
	uint32 i;
	
	tmp = (char *)malloc(2*len + 1);
	
	for (i = 0; i < len; ++i) 
	{
		tmp[i*2] = hextab[((uint8)src[i] ^ (uint8)(7-bitwidth)) >> 4];
		tmp[i*2+1] = hextab[(src[i] ^ (uint8)(7-bitwidth)) & 0x0F];
	}
	
	return tmp;
}

char * scanmem(char * src, int bitwidth) 
{
	int h, l, i, bytes;
	char *tmp;
	
	if (strlen(src) % 2)
	{
		return NULL;
	}
	
	bytes = (int)strlen(src)/2;
	tmp = (char *)malloc(bytes+1);

	for (i = 0; i < bytes; ++i) 
	{
		h = hexindex[(int)src[i*2]];
		l = hexindex[(int)src[i*2+1]];

		if (h < 0 || l < 0) 
		{
		        free(tmp);
		        return NULL;
		}
		
		tmp[i] = ((h << 4) + l) ^ (uint8)(7-bitwidth);
	}
	tmp[i] = 0;

	return tmp;
}

int url_encode(const char * src, const int srcsize, char * dst, const int dstsize)  
{  
    int i;  
    int j;  
    char ch;  
  
    if ((NULL == src) || (NULL == dst) || (srcsize <= 0) || (dstsize <= 0)) 
    {  
        return 0;  
    }  
  
    for (i = 0, j = 0; i < srcsize && j < dstsize; ++i) 
    {  
        ch = src[i];  
        
        if (((ch >= 'A') && (ch <= 'Z')) ||  
            ((ch >= 'a') && (ch <= 'z')) ||  
            ((ch >= '0') && (ch <= '9')) || 
            ch == '.' || ch == '-' || ch == '_' || ch == '*') 
        {  
            dst[j++] = ch;  
        } 
        else if (ch == ' ') 
        {  
            dst[j++] = '+';  
        }
        else 
        {  
            if (j+3 < dstsize)
            {  
                dst[j] = '%';
                dst[j+1] = hextab[ch >> 4];  
                dst[j+2] = hextab[ch & 15];  

                j+=3;
            }
            else 
            {  
                return 0;  
            }  
        }  
    }  
  
    dst[j] = '\0';  
    
    return j;  
} 

int url_decode(char * dst, char const * src, uint32 len) 
{	
    char * p_dst = dst;
    const char * p_src = src;
    
	while (len > 0) 
	{
		int before = 0;
		int after = 0;

        if (*p_src == '+')   
        {  
            ++p_src;
            --len;
            *p_dst++ = ' ';
        }
		else if (*p_src == '%' && len >= 3 && sscanf(p_src+1, "%n%2hhx%n", &before, p_dst, &after) == 1) 
		{
			uint32 size = after - before; // should be 1 or 2

			++p_dst;
			p_src += (1 + size);
			len -= (1 + size);
		} 
		else 
		{
			*p_dst++ = *p_src++;
			--len;
		}
	}
	
	*p_dst = '\0';

	return (int)(p_dst - dst);
}

time_t get_time_by_string(char * p_time_str)
{
	char * ptr_s;
	struct tm st;
	
	memset(&st, 0, sizeof(struct tm));

	ptr_s = p_time_str;

	while (*ptr_s == ' ' || *ptr_s == '\t')
	{
		ptr_s++;
	}
	
	sscanf(ptr_s, "%04d-%02d-%02d %02d:%02d:%02d", &st.tm_year, &st.tm_mon, &st.tm_mday, &st.tm_hour, &st.tm_min, &st.tm_sec);

	st.tm_year -= 1900;
	st.tm_mon -= 1;

	return mktime(&st);
}

void get_time_str(char * buff, int len)
{
	time_t nowtime;
	struct tm *t1;	

	time(&nowtime);
	t1 = localtime(&nowtime);

	snprintf(buff, len, "%04d-%02d-%02d %02d:%02d:%02d",
		t1->tm_year+1900, t1->tm_mon+1, t1->tm_mday,
		t1->tm_hour, t1->tm_min, t1->tm_sec);
}

void get_time_str_day_off(time_t nt, char * buff, int len, int dayoff)
{
    struct tm *t1;
    time_t nt1 = nt + dayoff * 24 * 3600;

    t1 = localtime(&nt1);

	snprintf(buff, len, "%04d-%02d-%02d %02d:%02d:%02d",
		t1->tm_year+1900, t1->tm_mon+1, t1->tm_mday,
		t1->tm_hour, t1->tm_min, t1->tm_sec);
}

void get_time_str_mon_off(time_t nt, char * buff, int len, int moffset)
{
	struct tm *t1;
	int year;

	int day_of_month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

	t1 = localtime(&nt);

	t1->tm_mon += moffset;

	t1->tm_year += t1->tm_mon / 12;
	t1->tm_mon = t1->tm_mon % 12;

    year = t1->tm_year + 1900;
    
    if ((year % 400) == 0 || ((year % 100) != 0 && (year % 4) == 0))
    {
        day_of_month[1] = 29;
    }
    else
    {
        day_of_month[1] = 28;
    }

	if (t1->tm_mday > day_of_month[t1->tm_mon])
	{
		t1->tm_mday = day_of_month[t1->tm_mon];
    }
    
	snprintf(buff, len, "%04d-%02d-%02d %02d:%02d:%02d",
		t1->tm_year+1900, t1->tm_mon+1, t1->tm_mday,
		t1->tm_hour, t1->tm_min, t1->tm_sec);
}

SOCKET tcp_connect(const char * hostname, int port, int timeout)
{
    int ret;
	SOCKET fd;
    char portstr[10];
    struct timeval tv;
    struct addrinfo hints = { 0 }, *ai, *cur_ai;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    snprintf(portstr, sizeof(portstr), "%d", port);
    
    ret = getaddrinfo(hostname, portstr, &hints, &ai);
    if (ret) 
    {
        log_print(LOG_ERR, "Failed to resolve hostname %s\r\n", hostname);
        return -1;
    }

    fd = -1;
    
    for (cur_ai = ai; cur_ai; cur_ai = cur_ai->ai_next) 
    {
        fd = socket(cur_ai->ai_family, cur_ai->ai_socktype, cur_ai->ai_protocol);
        if (fd < 0)
        {
            continue;
        }

        tv.tv_sec = timeout/1000;
	    tv.tv_usec = (timeout%1000) * 1000;
	    
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(tv));
        
        if (connect(fd, cur_ai->ai_addr, (int)(cur_ai->ai_addrlen)) < 0) 
        {
            closesocket(fd);
            fd = -1;

            log_print(LOG_ERR, "Connect hostname %s failed\r\n", hostname);
            continue;
        } 

        break;  /* okay we got one */
    }

    freeaddrinfo(ai);
    
    return fd;   
}

SOCKET tcp_connect_timeout(uint32 rip, int port, int timeout)
{
	SOCKET cfd;	
	struct sockaddr_in addr;
	
#if __LINUX_OS__	
	uint32 starttime = sys_os_get_ms();
	struct timeval tv;
#elif __WINDOWS_OS__
    int flag = 0;
	unsigned long ul = 1;
#endif

	cfd = socket(AF_INET, SOCK_STREAM, 0);
	if (cfd <= 0)
	{
	    log_print(LOG_ERR, "%s, socket failed\n", __FUNCTION__);
		return 0;
    }    
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = rip;
	addr.sin_port = htons((uint16)port);

#if __LINUX_OS__
	
	tv.tv_sec = timeout/1000;
	tv.tv_usec = (timeout%1000) * 1000;
	
	setsockopt(cfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(tv));

	while (connect(cfd, (struct sockaddr *)&addr, sizeof(addr)) == -1 && errno != EISCONN)
	{
        if (sys_os_get_ms() > starttime + timeout)
        {
        	closesocket(cfd);
            return -1;
        }
        
        if (errno != EINTR) 
        {
            closesocket(cfd);
            return -1;
        }
    }
    
	return cfd;
	
#elif __WINDOWS_OS__    
    
	ioctlsocket(cfd, FIONBIO, &ul);

	if (connect(cfd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
	{
		fd_set set;
		struct timeval tv;
		
		tv.tv_sec = timeout/1000;
		tv.tv_usec = (timeout%1000) * 1000;
		
		FD_ZERO(&set);
		FD_SET(cfd, &set);

		if (select((int)(cfd+1), NULL, &set, NULL, &tv) > 0)
		{
			int err = 0, len = sizeof(int);
			
			getsockopt(cfd, SOL_SOCKET, SO_ERROR, (char *)&err, (socklen_t*) &len);

			if (err == 0)
			{
				flag = 1;
			}	
		}
	}
	else
	{
	    flag = 1;
	}

    ul = 0;
	ioctlsocket(cfd, FIONBIO, &ul);
		
	if (flag == 1)
	{
		return cfd;
	}
	else
	{
		closesocket(cfd);
		return 0;
	}

#endif	
}

#if __LINUX_OS__

int daemon_init()
{
	pid_t pid;

	pid = fork();
	
	if (pid == -1)
	{
		return -1;
	}	
	else if (pid > 0)
	{
		exit(0);
	}
	
	setsid();

	return 0;
}

#endif

#if __WINDOWS_OS__

#include <sys/timeb.h>

// used to make sure that static variables in gettimeofday() aren't initialized simultaneously by multiple threads
static long initializeLock_gettimeofday = 0;  

int gettimeofday(struct timeval * tp, int * tz)
{
	static LARGE_INTEGER tickFrequency, epochOffset;

	static BOOL isInitialized = FALSE;

	LARGE_INTEGER tickNow;

	QueryPerformanceCounter(&tickNow);
 
	if (!isInitialized) 
	{
		if (1 == InterlockedIncrement(&initializeLock_gettimeofday)) 
		{
			// For our first call, use "ftime()", so that we get a time with a proper epoch.
			// For subsequent calls, use "QueryPerformanceCount()", because it's more fine-grain.
			struct timeb tb;
			ftime(&tb);
			
			tp->tv_sec = (long)tb.time;
			tp->tv_usec = 1000*tb.millitm;

			// Also get our counter frequency:
			QueryPerformanceFrequency(&tickFrequency);

			// compute an offset to add to subsequent counter times, so we get a proper epoch:
			epochOffset.QuadPart = tp->tv_sec * tickFrequency.QuadPart + (tp->tv_usec * tickFrequency.QuadPart) / 1000000L - tickNow.QuadPart;

			// next caller can use ticks for time calculation
			isInitialized = TRUE; 
			
			return 0;
		} 
		else 
		{
			InterlockedDecrement(&initializeLock_gettimeofday);
			
			// wait until first caller has initialized static values
			while (!isInitialized)
			{
				Sleep(1);
			}
		}
	}

    // adjust our tick count so that we get a proper epoch:
    tickNow.QuadPart += epochOffset.QuadPart;

    tp->tv_sec =  (long)(tickNow.QuadPart / tickFrequency.QuadPart);
    tp->tv_usec = (long)(((tickNow.QuadPart % tickFrequency.QuadPart) * 1000000L) / tickFrequency.QuadPart);

    return 0;
}

#endif


