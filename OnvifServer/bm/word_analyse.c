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

/***************************************************************************************/
#include "sys_inc.h"
#include "word_analyse.h"

/***************************************************************************************/
BOOL is_char(char ch)
{
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '#') || (ch == '@') || (ch == '/'))
	{	
		return TRUE;
	}
	
	return FALSE;
}

BOOL is_num(char ch)
{
	if (ch >= '0' && ch <= '9')
	{
		return TRUE;
	}
	
	return FALSE;
}

static char separator[] = {' ','\t','\r','\n',',',':','{','}','(',')','\0','\'','"','?','<','>','=',';'};
				  
BOOL is_separator(char ch)
{
	uint32 i;
	
	for (i=0; i<sizeof(separator); i++)
	{
		if (separator[i] == ch)
		{
			return TRUE;
		}	
	}

	return FALSE;
}

BOOL is_ip_address(const char * address)
{	
	int i;
	int index = 0;
	uint16 byte_value;

	int total_len = (int)strlen(address);
	if (total_len > 15)
	{
		return FALSE;	
	}
	
	for (i=0; i<4; i++)
	{
		if ((address[index] < '0') || (address[index] > '9'))
		{
			return FALSE;
		}
		
		if (((address[index +1] < '0') || (address[index +1] > '9')) && (address[index +1] != '.'))
		{
			if ((address[index +1] == '\0') && (i == 3))
			{
				return TRUE;
			}
			
			return FALSE;
		}
		
		if (address[index +1] == '.')
		{
			index+=2;
			continue;
		}

		if (((address[index +2] < '0') || (address[index +2] > '9')) && (address[index +2] != '.'))
		{
			if ((address[index +2] == '\0') && (i == 3))
			{
				return TRUE;
			}
			
			return FALSE;
		}
		
		if (address[index +2] == '.')
		{
			index+=3;
			continue;
		}

		if (i < 3)	
		{
			if (address[index +3] != '.')
			{
				return FALSE;
			}	
		}

		byte_value = (address[index] - '0') *100 + (address[index +1] -'0') *10 + (address[index +2] - '0');

		if (byte_value > 255)
		{
			return FALSE;
		}
		
		if (i < 3)
		{
			index += 4;
		}
		else
		{
			index += 3;
		}
	}

	if (index != total_len)
	{
		return FALSE;	
	}
	
	return TRUE;
}


BOOL is_integer(char * p_str)
{
	int i;
	int len = (int)strlen(p_str);

	for (i=0; i<len; i++)
	{
		if (!is_num(p_str[i]))
		{
			return FALSE;
		}
	}
	
	return TRUE;	
}

BOOL GetLineText(char * buf, int cur_line_offset, int max_len, int * len, int * next_line_offset)
{
	char * ptr_start = buf+cur_line_offset;
	char * ptr_end = ptr_start;
	int	   line_len;
	BOOL   bHaveNextLine = TRUE;

	while ((*ptr_end != '\r') && (*ptr_end != '\n') && (*ptr_end != '\0') && ((ptr_end - ptr_start) < max_len))
	{
		ptr_end++;
	}
	
	while (*(ptr_end-1) == ',')
	{
		while ((*ptr_end == '\r') || (*ptr_end == '\n'))
		{
			*ptr_end = ' ';	
			ptr_end++;
		}

		while ((*ptr_end != '\r') && (*ptr_end != '\n') && (*ptr_end != '\0') && ((ptr_end - ptr_start) < max_len))
		{
			ptr_end++;
		}	
	}

	line_len = (int)(ptr_end - ptr_start);

	if ((*ptr_end == '\r') && (*(ptr_end+1) == '\n'))
	{
		line_len += 2;
		
		if (line_len == max_len)
		{
			bHaveNextLine = FALSE;
		}	
	}
	else if(*ptr_end == '\n')
	{
		line_len += 1;
		
		if (line_len == max_len)
		{
			bHaveNextLine = FALSE;
		}	
	}
	else if ((*ptr_end == '\0') || ((ptr_end - ptr_start) < max_len))
	{
		bHaveNextLine = FALSE;
	}	
	else
	{
		bHaveNextLine = FALSE;
	}
	
	*len = (int)(ptr_end - ptr_start);
	*next_line_offset = cur_line_offset + line_len;

	return bHaveNextLine;
}

BOOL GetSipLine(char * p_buf, int max_len, int * len, BOOL * bHaveNextLine)
{
	char * ptr_start = p_buf;
	char * ptr_end = ptr_start;
	int	   line_len;

	*bHaveNextLine = TRUE;

	while ((*ptr_end != '\0') && (!((*ptr_end == '\r') || (*ptr_end == '\n'))) && ((ptr_end - ptr_start) < max_len))
	{
		ptr_end++;
	}
	
	while (*(ptr_end-1) == ',')
	{
		while ((*ptr_end == '\r') || (*ptr_end == '\n'))
		{
			*ptr_end = ' ';	
			ptr_end++;
		}

		while ((*ptr_end != '\r') && (*ptr_end != '\n') && (*ptr_end != '\0') && ((ptr_end - ptr_start) < max_len))
		{
			ptr_end++;
		}	
	}

	line_len = (int)(ptr_end - ptr_start);

	if (((*ptr_end == '\r') && (*(ptr_end+1) == '\n')) || ((*ptr_end == '\n') && (*(ptr_end+1) == '\n')))
	{
		*ptr_end = '\0';
		*(ptr_end+1) = '\0';
		line_len += 2;
		
		if (line_len == max_len)
		{
			*bHaveNextLine = FALSE;
		}
		
		*len = line_len;
		
		return TRUE;
	}
	else if ((*ptr_end == '\r') || (*ptr_end == '\n'))
	{
		*ptr_end = '\0';
		line_len += 1;
		
		if (line_len == max_len)
		{
			*bHaveNextLine = FALSE;
		}
		
		*len = line_len;
		
		return TRUE;
	}
	else if (*ptr_end == '\0')
	{
		if (line_len == max_len)
		{
			*bHaveNextLine = FALSE;
		}
		
		*len = line_len;
		
		return TRUE;
	}
	else
	{
		return FALSE;
	}	
}

BOOL GetLineWord(char * line, int cur_word_offset, int line_max_len, char * word_buf, int buf_len, int * next_word_offset, WORD_TYPE w_t)
{
	int     len;
	char *	ptr_start = line+cur_word_offset;
	char *	ptr_end = ptr_start;
	BOOL	bHaveNextWord = TRUE;

	word_buf[0] = '\0';	

	while (((*ptr_start == ' ') || (*ptr_start == '\t')) && (cur_word_offset < line_max_len))
	{ 
		cur_word_offset++; 
		ptr_start++;
	}

	if (*ptr_start == '\0')
	{
		return FALSE;	
	}
	
	ptr_end = ptr_start;

	while ((!is_separator(*ptr_end)) && ((ptr_end - ptr_start) < line_max_len))
	{
		ptr_end++;
	}
	
	len = (int)(ptr_end - ptr_start);
	
	if (len >= buf_len)
	{
		word_buf[0] = '\0';
		return bHaveNextWord;
	}

	*next_word_offset = cur_word_offset + len;
	
	if ((*next_word_offset >= line_max_len) || (line[*next_word_offset] == '\0'))
	{
		bHaveNextWord = FALSE;
	}
	
	switch (w_t)
	{
	case WORD_TYPE_NULL:
		break;

	case WORD_TYPE_STRING:
		if (len == 0 && is_separator(*ptr_end))
		{
			(*next_word_offset)++;
			word_buf[0] = *ptr_end;
			word_buf[1] = '\0';
			
			return bHaveNextWord;
		}
		break;

	case WORD_TYPE_NUM:
		{
			char * ptr;
			for (ptr=ptr_start; ptr<ptr_end; ptr++)
			{
				if (!is_num(*ptr))
				{
					word_buf[0] = '\0';
					return bHaveNextWord;
				}
			}
		}
		break;

	case WORD_TYPE_SEPARATOR:
		if (is_separator(*ptr_end))
		{
			(*next_word_offset)++;
			word_buf[0] = *ptr_end;
			word_buf[1] = '\0';
			
			return bHaveNextWord;
		}
		break;
	}

	memcpy(word_buf, ptr_start, len);
	word_buf[len] = '\0';

	return bHaveNextWord;
}

BOOL GetNameValuePair(char * text_buf, int text_len, const char * name, char * value, int value_len)
{
	char word_buf[256];
	int	 cur_offset = 0,next_offset = 0;
	char * value_start, * value_end;

	while (next_offset < text_len)
	{
		word_buf[0] = '\0';

		cur_offset = next_offset;

		GetLineWord(text_buf, cur_offset, text_len, word_buf, sizeof(word_buf), &next_offset, WORD_TYPE_STRING);

		if (strcmp(word_buf, name) == 0)
		{
			int len;
			char * ptr = text_buf + next_offset;
			
			while ((*ptr == ' ' || *ptr == '\t') && (next_offset <text_len))
			{ 
				ptr++;
				next_offset++;
			}
			
			if ((*ptr == ';') || (*ptr == ',') || (*ptr == '\0'))
			{
				value[0] = '\0';
				return TRUE;
			}

			if (*ptr != '=')
			{
				return FALSE;
			}
			
			ptr++;
			next_offset++;

			while ((*ptr == ' ' || *ptr == '\t') && (next_offset <text_len))
			{
				ptr++;
				next_offset++;
			}
			
			if (*ptr != '"')
			{
				value_start = ptr;
				
				while ((*ptr != ';') && (*ptr != ',') && (*ptr != '&') && (next_offset <text_len))
				{
					ptr++;
					next_offset++;
				}
				
				if ((*ptr != ';') && (*ptr != ',') && (*ptr != '&') && (*ptr != '\0'))
				{
					return FALSE;
				}
				
				value_end = ptr;
			}
			else
			{
				ptr++;
				next_offset++;
				value_start = text_buf + next_offset;
				
				while ((*ptr != '"') && (next_offset <text_len))
				{
					ptr++;
					next_offset++;
				}
				
				if (*ptr != '"')
				{
					return FALSE;
				}
				
				value_end = ptr;
			}

			len = (int)(value_end - value_start);
			if (len > value_len)
			{
				len = value_len - 1;
			}
			
			memcpy(value, value_start, len);
			value[len] = '\0';

			return TRUE;
		}
		else	
		{
			char * ptr = text_buf + next_offset;
			
			while((*ptr == ' ' || *ptr == '\t') && (next_offset <text_len))
			{ 
				ptr++;
				next_offset++;
			}

			if ((*ptr == ';') || (*ptr == ',') || (*ptr == '&'))
			{
				next_offset++;
				continue;
			}

			if (*ptr != '=')
			{
				return FALSE;
			}
			
			ptr++;
			next_offset++;

			while ((*ptr == ' ' || *ptr == '\t') && (next_offset <text_len))
			{ 
				ptr++;
				next_offset++;
			}
			
			if (*ptr != '"')
			{
				while ((*ptr != ';') && (*ptr != ',') && (*ptr != '&') && (next_offset <text_len))
				{
					ptr++;
					next_offset++;
				}
				
				if ((*ptr != ';') && (*ptr != ',') && (*ptr != '&') && (*ptr != '\0'))
				{
					return FALSE;
				}
				
				next_offset++;	
			}
			else
			{
				ptr++;
				next_offset++;
				
				while ((*ptr != '"') && (next_offset <text_len))
				{ 
					ptr++;
					next_offset++;
				}
				
				if (*ptr != '"')
				{
					return FALSE;
				}
				
				ptr++;
				next_offset++;
				
				while ((*ptr == ' ' || *ptr == '\t') && (next_offset <text_len))
				{ 
					ptr++;
					next_offset++;
				}
				
				if (*ptr != ',')
				{
					return FALSE;
				}
				
				next_offset++;	
			}
		}
	}

	return FALSE;
}





