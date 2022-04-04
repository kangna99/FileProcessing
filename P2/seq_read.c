#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

//
// input parameters: 레코드 파일
//
int main(int argc, char **argv)
{

    // 레코드 파일로부터 전체 레코드를 순차적으로 읽어들이고, 이때
    // 걸리는 시간을 측정하는 코드 구현

    struct timeval start_t, end_t;
    int record_num;
    long time;
    char *buffer;
    FILE *fp;

    if(argc != 2) {
        fprintf(stderr, "argc count error\n");
        exit(1);
    }

    if((fp = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "fopen error\n");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    record_num = ftell(fp) / 200;
    rewind(fp);

    buffer = (char*)malloc(sizeof(char) * 200);

    //시작 시간 측정
    gettimeofday(&start_t, NULL);
    //printf("start: %ld:%ld\n", start_t.tv_sec, start_t.tv_usec);

    //레코드 수만큼 순차적으로 읽음
    for(int i = 0; i < record_num; i++) {
        if(fread(buffer, sizeof(char) * 200, 1, fp) != 1) {
            fprintf(stderr, "fread error\n");
            exit(1);
        }
    }

    //종료 시간 측정
    gettimeofday(&end_t, NULL);
    //printf("end: %ld:%ld\n", end_t.tv_sec, end_t.tv_usec);

    time = (end_t.tv_sec - start_t.tv_sec) * 1000000 + (end_t.tv_usec - start_t.tv_usec);
    fprintf(stdout, "%ld usec\n", time);

    free(buffer);
    fclose(fp);

    return 0;
}
