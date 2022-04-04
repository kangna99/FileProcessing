#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// 필요하면 header 파일 추가 가능

//
// input parameters: 학생 레코드 수, 레코드 파일
//
int main(int argc, char **argv)
{
	
	// 사용자로부터 입력 받은 레코드 수 만큼의 레코드 파일을 생성하는 코드 구현
	
	// 파일에 '학생 레코드' 저장할 때 주의 사항
	// 1. 레코드의 크기는 무조건 200 바이트를 준수
	// 2. 레코드 파일에서 레코드와 레코드 사이에 어떤 데이터도 들어가면 안됨
	// 3. 레코드에는 임의의 데이터를 저장해도 무방
	// 4. 만약 n개의 레코드를 저장하면 파일 크기는 정확히 200 x n 바이트가 되어야 함
    
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
    
    //파일에 학생 레코드를 하나씩 반복적으로 record_num 수만큼 저장
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
