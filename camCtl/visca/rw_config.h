#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int GetIniKeyInt(char *title,char *key,char *filename);
char *GetIniKeyString(char *title,char *key,char *filename);
int PutIniKeyString(char *title,char *key,char *val,char *filename);
int PutIniKeyInt(char *title,char *key,int val,char *filename);
