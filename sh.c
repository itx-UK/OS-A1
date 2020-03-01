#include<string.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define LINE_LEN 80
#define MAX_ARGS 64
#define MAX_ARG_LEN 16
#define MAX_PATHS 64
#define MAX_PATH_LEN 96
#define WHITESPACE ".,\t\n"

#ifndef NULL
#define NULL
#endif

struct command_t{
	char* name;
	int argc;
	char* argv[MAX_ARGS];
	command_t()
	{
		name="";
	}
};

void printPrompt()
{
	char * temp;
	string name;
	temp = getenv("USER");
    	if (temp != 0) {
		name=temp;
		temp = 0;
    	}
	char result[MAX_PATH_LEN];
	ssize_t count = readlink( "/proc/self/exe", result, MAX_PATH_LEN );
	string path=std::string( result, (count > 0) ? count : 0 );

	cout<<name<<">> "<<path<<">> ";
}
void readCommand(char* buffer)
{
	gets(buffer);
}
int parsePath(char* dirs[])
{
	char *pathEnvVar;
	char *thePath;
	string name="";
	for(int i=0; i<MAX_ARGS;i++)
	{
		dirs[i] = "";
	}
	pathEnvVar =(char*) getenv("PATH");
	thePath = (char*) malloc(strlen(pathEnvVar) + 1);
	strcpy(thePath,pathEnvVar);
	int i=0,j=0,k=0;
	while(thePath[i+k]!='\0')
	{
		if(thePath[i+k]==':')
		{
			k=k+i;
			i=0;
			int len=0;
			for(int p=0;name[p]!='\0';p++)
			{
				len++;
			}
			dirs[j] = new char[len + 1];
			int p=0;
			for(p=0;p<len;p++)
			{
				dirs[j][p]=name[p];
			}
			dirs[j][p]='\0';

			name="";
			j++;
		}
		else
		{
			name+=thePath[i+k];
		}
		i++;
	}
	int len=0;
	for(int p=0;name[p]!='\0';p++)
	{
		len++;
	}
	dirs[j] = new char[len + 1];
	int p=0;
	for(p=0;p<len;p++)
	{
		dirs[j][p]=name[p];
	}
	dirs[j][p]='\0';
	j++;

	return j;
}

int parseCommand(char* read,command_t* command_break)
{
	string name="";
	int i=0,j=0,k=0;
	while(read[i+k]!='\0')
	{
		if(read[i+k]==' ' || read[i+k]=='\0')
		{
			k=k+i;
			i=0;

			int len=0;
			for(int p=0;name[p]!='\0';p++)
			{
				len++;
			}
			command_break->argv[j] = new char[len + 1];
			int p=0;
			for(p=0;p<len;p++)
			{
				command_break->argv[j][p]=name[p];
			}
			command_break->argv[j][p]='\0';

			name="";
			j++;
		}
		else
		{
			name+=read[i+k];
		}
		i++;
	}
	int len=0;
	for(int p=0;name[p]!='\0';p++)
	{
		len++;
	}
	command_break->argv[j] = new char[len + 1];
	int p=0;
	for(p=0;p<len;p++)
	{
		command_break->argv[j][p]=name[p];
	}
	command_break->argv[j][p]='\0';
	j++;
	command_break->argv[j] = NULL;

	command_break->argc=j;
	return j;
}
char *lookupPath(char** argv ,char** dir, char *&c)
{
	char* result;
	char pName[MAX_PATH_LEN];
	if(*argv[0]== '/')
	{
		int len2=strlen(argv[0]);
		c = new char[len2];
		for(int j=0;j<len2;j++)
		{
			c[j]=argv[0][j];
		}
		return NULL;
	}
	for(int i=0;i<MAX_PATHS;i++)
	{
		
		int len=strlen(dir[i]);
		int len1= strlen(argv[0]);
		char* store = new char[len+len1+1];
		for(int j=0;j<len;j++)
		{
			store[j]=dir[i][j];
		}
		store[len]='/';
		int k=0;
		for(int j=len+1;j<len+len1+1;j++)
		{
			store[j]=argv[0][k];
			k++;
		}
	//	cout<<store<<"\n";
		if(access(store,F_OK)==0)
		{
			int len2=strlen(store);
			c = new char[len2];
			for(int j=0;j<len2;j++)
			{
				c[j]=store[j];
			}
			
		//	c[len-1]='\0';
			return NULL;
		}
	}
	cout<<"\n"<<stderr<<argv[0]<<"Command not found\n";
	return NULL;
}
int main()
{

	char* dirs[MAX_ARGS];
	parsePath(dirs);

	while(true)
	{
		printPrompt();

		char read[256];
		readCommand(read);
	
		command_t* command_break=new command_t;
		parseCommand(read,command_break);

		char * c;
		lookupPath(command_break->argv,dirs,c);
		pid_t pid;
		pid=fork();
		if(pid==0)
		{
			if(c!=NULL)
			{
				cout<<execv(c,command_break->argv);
			}
			else
			{
				cout<<"not found";
			}
		}
		else if(pid>0)
		{
			wait(NULL);
		}
		else
		{
			cout<<"\nerror\n";
		}
	}
	return 0;
}
