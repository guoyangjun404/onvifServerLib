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
#include "http.h"
#include "http_cln.h"


BOOL http_tcp_tx(HTTPREQ * p_req, const char * p_data, int len)
{
    int slen;
    
	if (p_req->cfd <= 0)
	{
		return FALSE;
    }
    
#ifdef HTTPS
	if (p_req->https)
	{
		slen = SSL_write(p_req->ssl, p_data, len);
	}
	else
	{
		slen = send(p_req->cfd, p_data, len, 0);
	}
#else	
	slen = send(p_req->cfd, p_data, len, 0);
#endif
	if (slen != len)
	{
	    log_print(LOG_ERR, "%s, slen = %d, len = %d\r\n", __FUNCTION__, slen, len);
		return FALSE;
    }
    
	return TRUE;
}

BOOL http_onvif_req(HTTPREQ * p_req, const char * p_xml, int len)
{
    int offset = 0;
	char bufs[1024 * 4];
	
	if (len > 3072)
	{
		return FALSE;
	}
	
	offset += sprintf(bufs+offset, "POST %s HTTP/1.1\r\n", p_req->url);
	offset += sprintf(bufs+offset, "Host: %s:%d\r\n", p_req->host, p_req->port);
	offset += sprintf(bufs+offset, "User-Agent: ltxd/1.0\r\n");
	offset += sprintf(bufs+offset, "Content-Type: application/soap+xml; charset=utf-8; action=\"%s\"\r\n", p_req->action);
	offset += sprintf(bufs+offset, "Content-Length: %d\r\n", len);
	offset += sprintf(bufs+offset, "Connection: close\r\n\r\n");

	if (p_xml && len > 0)
	{
		memcpy(bufs+offset, p_xml, len);
		offset += len;
	}
	
	bufs[offset] = '\0';

    log_print(LOG_DBG, "TX >> %s\r\n", bufs);
    
	return http_tcp_tx(p_req, bufs, offset);
}


BOOL http_onvif_trans(HTTPREQ * p_req, int timeout, const char * bufs, int len)
{
	BOOL ret = FALSE;
	
#ifdef HTTPS
	SSL_CTX* ctx = NULL;
	const SSL_METHOD* meth = NULL;

	if (p_req->https)
	{
		SSLeay_add_ssl_algorithms();  
		meth = SSLv23_client_method();	
		SSL_load_error_strings();  
		ctx = SSL_CTX_new(meth);
		if (NULL == ctx)
		{
			log_print(LOG_ERR, "%s, SSL_CTX_new failed!\r\n", __FUNCTION__);
			return FALSE;
		}
	}
#else
	if (p_req->https)
	{
		log_print(LOG_ERR, "%s, the server require ssl connection, unsupport!\r\n", __FUNCTION__);
		return FALSE;
	}
#endif

	p_req->cfd = tcp_connect_timeout(inet_addr(p_req->host), p_req->port, timeout);
	if (p_req->cfd <= 0)
	{
	    log_print(LOG_ERR, "%s, tcp_connect_timeout\r\n", __FUNCTION__);
		goto FAILED;
    }

#ifdef HTTPS
	if (p_req->https)
	{
		p_req->ssl = SSL_new(ctx); 
		if (NULL == p_req->ssl)
		{
			log_print(LOG_ERR, "%s, SSL_new failed!\r\n", __FUNCTION__);
			goto FAILED;
		}
		
		SSL_set_fd(p_req->ssl, (int)p_req->cfd);
		
		if (SSL_connect(p_req->ssl) == -1)
		{
			log_print(LOG_ERR, "%s, SSL_connect failed!\r\n", __FUNCTION__);
			goto FAILED;
		}
	}
#endif

	ret = http_onvif_req(p_req, bufs, len);

FAILED:

	if (p_req->cfd > 0)
	{
		closesocket(p_req->cfd);
		p_req->cfd = 0;
	}

#ifdef HTTPS
	if (p_req->ssl)
	{
		SSL_free(p_req->ssl);
		p_req->ssl = NULL;
	}

	if (ctx)
	{
		SSL_CTX_free(ctx);
	}
#endif
    
	return ret;
}



