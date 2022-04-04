#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// �ʿ��ϸ� header ���� �߰� ����

//
// input parameters: �л� ���ڵ� ��, ���ڵ� ����
//
int main(int argc, char **argv)
{
	
	// ����ڷκ��� �Է� ���� ���ڵ� �� ��ŭ�� ���ڵ� ������ �����ϴ� �ڵ� ����
	
	// ���Ͽ� '�л� ���ڵ�' ������ �� ���� ����
	// 1. ���ڵ��� ũ��� ������ 200 ����Ʈ�� �ؼ�
	// 2. ���ڵ� ���Ͽ��� ���ڵ�� ���ڵ� ���̿� � �����͵� ���� �ȵ�
	// 3. ���ڵ忡�� ������ �����͸� �����ص� ����
	// 4. ���� n���� ���ڵ带 �����ϸ� ���� ũ��� ��Ȯ�� 200 x n ����Ʈ�� �Ǿ�� ��
    
    int record_num = 0;
    char *file_name, *buffer;
    FILE* fp;

    if(argc != 3) {
        fprintf(stderr, "argc count error\n");
        exit(1);
    }

    record_num = atoi(argv[1]);
    file_name = argv[2];

    if(record_num < 0) {
        fprintf(stderr, "record number shoule be positive number\n");
        exit(1);
    }

    if((fp = fopen(file_name, "w")) == NULL) {
        fprintf(stderr, "fopen error\n");
        exit(1);
    }

    buffer = (char*)malloc(sizeof(char) * 200);
    
    //���Ͽ� �л� ���ڵ带 �ϳ��� �ݺ������� record_num ����ŭ ����
    for(int i = 0; i < record_num; i++) {
        memset(buffer, 0, sizeof(char) * 200);
        strcpy(buffer, "20192937 Nayoon Kang Soongsil University CSE");
        if(fwrite(buffer, sizeof(char)*200, 1, fp) != 1) {
            fprintf(stderr, "fwrite error\n");
            exit(1);
        }
    }

    free(buffer);
    fclose(fp);

	return 0;
}
