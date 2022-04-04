#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
//필요하면 header file 추가 가능


#define SUFFLE_NUM	10000	// 이 값은 마음대로 수정 가능

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);
// 필요한 함수가 있으면 더 추가할 수 있음

//
// input parameters: 레코드 파일
//
int main(int argc, char **argv)
{
	int *read_order_list;
	int num_of_records;
    struct timeval start_t, end_t;
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
    num_of_records = ftell(fp) / 200;
    rewind(fp);

    read_order_list = (int*)malloc(sizeof(int)*num_of_records);

	// 이 함수를 실행하면 'read_order_list' 배열에 읽어야 할 레코드 번호들이 순서대로
	// 나열되어 저장됨. 'num_of_records'는 레코드 파일에 저장되어 있는 전체 레코드의 수를 의미함.
	GenRecordSequence(read_order_list, num_of_records);

	// 'read_order_list'를 이용하여 레코드 파일로부터 전체 레코드를 random 하게 읽어들이고,
	// 이때 걸리는 시간을 측정하는 코드 구현
    buffer = (char*)malloc(sizeof(char) * 200);
    
    //시작 시간 측정
    gettimeofday(&start_t, NULL);
    //printf("start: %ld:%ld\n", start_t.tv_sec, start_t.tv_usec);

    for(int i = 0; i < num_of_records; i++) {
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

void GenRecordSequence(int *list, int n)
{
	int i, j, k;

	srand((unsigned int)time(0));

	for(i=0; i<n; i++)
	{
		list[i] = i;
	}
	
	for(i=0; i<SUFFLE_NUM; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}

	return;
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;

	return;
}
