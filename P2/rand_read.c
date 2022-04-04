#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
//�ʿ��ϸ� header file �߰� ����


#define SUFFLE_NUM	10000	// �� ���� ������� ���� ����

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);
// �ʿ��� �Լ��� ������ �� �߰��� �� ����

//
// input parameters: ���ڵ� ����
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

	// �� �Լ��� �����ϸ� 'read_order_list' �迭�� �о�� �� ���ڵ� ��ȣ���� �������
	// �����Ǿ� �����. 'num_of_records'�� ���ڵ� ���Ͽ� ����Ǿ� �ִ� ��ü ���ڵ��� ���� �ǹ���.
	GenRecordSequence(read_order_list, num_of_records);

	// 'read_order_list'�� �̿��Ͽ� ���ڵ� ���Ϸκ��� ��ü ���ڵ带 random �ϰ� �о���̰�,
	// �̶� �ɸ��� �ð��� �����ϴ� �ڵ� ����
    buffer = (char*)malloc(sizeof(char) * 200);
    
    //���� �ð� ����
    gettimeofday(&start_t, NULL);
    //printf("start: %ld:%ld\n", start_t.tv_sec, start_t.tv_usec);

    for(int i = 0; i < num_of_records; i++) {
        if(fread(buffer, sizeof(char) * 200, 1, fp) != 1) {
            fprintf(stderr, "fread error\n");
            exit(1);
        }
    }
    
    //���� �ð� ����
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
