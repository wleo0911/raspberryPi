#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <libgen.h>
#include <sys/vfs.h>
#include <dirent.h>

#define BUFSIZE 200
#define DIRPATH "/home/pi/rsapiEx/blackbox/"
#define NAMESTART "raspivid -p 1300,50,640,480 -w 1920 -h 1080 -t 600000 -o "
#define NAMEEND ".h264 -vf -hf"
#define REMOVE "rm -rf "

void DefineName(char *dir_name, char *sys_name)
{
    time_t      UTCtime;
    struct tm   *tm;
    char        temp[BUFSIZE];

    time(&UTCtime);
    tm = localtime(&UTCtime); // 사용자 정의 출력을 위한 tm 구조체 사용

    strftime(temp, sizeof(temp), "%Y%m%e%H", tm); // 사용자 정의 출력, 연월일시
    sprintf(dir_name, DIRPATH); // /home/pi/rsapiEx/blackbox/
    strcat(dir_name, temp);     // /home/pi/raspiEx/blackbox/20180723??

    sprintf(sys_name, NAMESTART);       // "raspivid -w 1920 -h 1080 -t 600000 -o "
    strcat(sys_name, dir_name);        // "/home/pi/rsapiEx/blackbox/20180723시"
    strcat(sys_name, "/");                             // "/"
    strftime(temp, sizeof(temp), "%Y%m%e_%H%M%S", tm); // 사용자 정의 출력, 연월일_시분초
    strcat(sys_name, temp);  // "20180723_시분초"
    strcat(sys_name, NAMEEND); // ".h264 -vf -hf"
  }

int CheckDisk(void)
{
    struct  statfs s;
    long    free_percent;

    if(statfs("/", &s) == -1)
    {
        perror("error : statfs()\n");
        return -1;
    }

    free_percent = (long)(s.f_bavail * 100 / (s.f_blocks) + 0.5);
    printf("free space = %ld%%\n", free_percent);

    if(free_percent < 20)
        return 1;
    return 0;
}

int RemoveDirectory(void)
{
    struct dirent   **namelist;
    char            rmv_name[BUFSIZE];
    int idx, count;

    if((count = scandir(DIRPATH, &namelist, NULL, alphasort)) == -1)
    {
        perror("error : scandir()\n");
        return -1;
    }

    sprintf(rmv_name, REMOVE); // 제일 오래된 거 하나 지우기
    strcat(rmv_name, namelist[2]->d_name);
    system(rmv_name);
    printf("%s directory delete\n", namelist[2]->d_name);
    

    for(idx=0; idx<count; idx++) // 건별 데이터 메모리 해제
        free(namelist[idx]);
    free(namelist);
}

int MakeDirectory(char *dir_name)
{
    if((mkdir(dir_name, 0777)) == -1)
    {
        if(errno != EEXIST) // 중복 체크
        {
            perror("error : mkdir()\n");
            return -1;
        }
    }
	return 1;
}

int main()
{
    char   dir_name[BUFSIZE];
    char   sys_name[BUFSIZE];

    while(1)
    {
    // 1. DefineName() time()를 통해서 폴더명, 파일명에 사용할 시간 뽑아내기
    DefineName(dir_name, sys_name);

    // 2. CheckDisk() 
    if(CheckDisk()) // 용량이 20%보다 적으면 참 
        RemoveDirectory();
    MakeDirectory(dir_name);

    // 3. start recording
	system("ls");
	system(sys_name);
    }
    return 0;
}
