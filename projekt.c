#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <dirent.h>

typedef struct 
{
	char *permissions;
	nlink_t hardLinks;
	char *ownerName;
	char *groupName;
	off_t fileSize;
	char *date;
	char *fileName;
} lsLine;

int arrSize = 0;

char *getPermissions(char *dir)
{
	char *returnVal;
	//FREE!
	returnVal = (char*) malloc(11 * sizeof(char));
	strcpy(returnVal, "----------");

	short currBit;

	struct stat statbuf;
	stat(dir, &statbuf);

	if((S_IFMT & statbuf.st_mode) == S_IFDIR) returnVal[0] = 'd';
	else if((S_IFMT & statbuf.st_mode) == S_IFLNK) returnVal[0] = 'l';

	mode_t st_m = statbuf.st_mode;
	for(int i = 3; i>0; i--)
	{
		currBit = st_m % 8;
		st_m = st_m / 8;
		if((currBit & 1) == 1) (returnVal[3*i] = 'x');
		if((currBit & 2) == 2) (returnVal[(3*i) - 1] = 'w');
		if((currBit & 4) == 4) (returnVal[(3*i) - 2] = 'r');
	}

	return returnVal;
}

nlink_t getHardLinks(char *dir)
{
	struct stat statbuf;
	stat(dir, &statbuf);

	return statbuf.st_nlink;
}

char *getOwnerName(char *dir)
{
	struct stat statbuf;
	stat(dir, &statbuf);

	uid_t uid = statbuf.st_uid;
	struct passwd *owner = getpwuid(uid);
	return owner->pw_name;
}

char *getGroupName(char *dir)
{
	struct stat statbuf;
	stat(dir, &statbuf);

	gid_t gid = statbuf.st_gid;
	struct group *grp = getgrgid(gid);
	return  grp->gr_name;
}

off_t getSize(char *dir)
{
	struct stat statbuf;
	stat(dir, &statbuf);
	return statbuf.st_size;
}

char *getDate(char *dir)
{
	char *returnVal;
	//FREE
	returnVal = (char*) malloc(15*sizeof(char));
	struct stat statbuf;
	stat(dir, &statbuf);

	struct tm *modtime = localtime(&statbuf.st_mtim.tv_sec);
	sprintf(returnVal, "%d-%d %d:%d", modtime->tm_mon+1, modtime->tm_mday, modtime->tm_hour, modtime->tm_min);
	return returnVal;
}

/*void printLsLine(char *dir)
{
	char *perms = getPermissions(dir);
	char *date = getDate(dir);
	printf("%s %lu %s %s %ld %s\n", perms, getHardLinks(dir), getOwnerName(dir), getGroupName(dir), getSize(dir), date);
	free(perms);
	free(date);
}*/

lsLine newlsLine(char *dir)
{
	lsLine returnVal;
	//Free
	returnVal.fileName = (char *) malloc((strlen(dir) + 1) * sizeof(char));
	strcpy(returnVal.fileName, dir);
	returnVal.permissions = getPermissions(dir);
	returnVal.hardLinks = getHardLinks(dir);
	returnVal.ownerName = getOwnerName(dir);
	returnVal.groupName = getGroupName(dir);
	returnVal.fileSize = getSize(dir);
	returnVal.date = getDate(dir);
	return returnVal;
}

lsLine *getLsLines(char *dir)
{
	struct stat statbuf;
	stat(dir, &statbuf);
	//Directory
	if((S_IFMT & statbuf.st_mode) == S_IFDIR)
	{
		lsLine *returnVal;
		DIR *folder;
		struct dirent *file;
		char fileDir[256];
		int numFiles = 0;
		
		if((folder = opendir(dir)) == NULL)
		{
			printf("Problem");
			return NULL;
		}
		for(;;)
		{
			if(readdir(folder) == NULL)
			{
				break;
			}
			numFiles++;
		}
		arrSize = numFiles;
		closedir(folder);
		returnVal = (lsLine*) malloc(numFiles * sizeof(lsLine));
		numFiles = 0;
		if((folder = opendir(dir)) == NULL)
		{
			printf("Problem");
			return NULL;
		}
		for(;;)
		{
			fileDir[0] = '\0';
			strcpy(fileDir, dir);
			strcat(fileDir, "/");
			if((file = readdir(folder)) == NULL)
			{
				break;
			}
			strcat(fileDir, file->d_name);
			returnVal[numFiles] = newlsLine(fileDir);
			numFiles++;
		}
		closedir(folder);
		return returnVal;
	}
	else
	{
		//Error :)
	}
	
}

void printLsAl(lsLine *lines)
{
	
}

int main(int argc, char **argv) {
	newlsLine("./a.out");


	lsLine *files = getLsLines(".");

	for(int i = 0; i < arrSize; i++)
	{
		printf("%s\n", files[i].date);
	}
	free(files);

    /*DIR* dirp;
	struct passwd *pwd;
    struct group *grp;
	struct dirent* direntp;*/



	/*
	dirp = opendir(argv[1]);
	for(;;) 
    {
		direntp = readdir(dirp);
		if(direntp == NULL) break;
		printf( "%s\n", direntp->d_name);
	}
														        
	closedir(dirp);*/																			    
	return 0;
}
