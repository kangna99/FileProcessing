#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int offset, file_size;
    char *data;
    FILE *fp;

    if(argc != 4) {
        fprintf(stderr, "argc count error\n");
        exit(1);
    }

    if((fp = fopen(argv[3], "r+")) == NULL) {
        fprintf(stderr, "fopen error\n");
        exit(1);
    }

    //파일 사이즈 계산
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    offset = atoi(argv[1]);
    data = argv[2];

    //offset 예외 처리
    if(offset > file_size - 1) {
        fprintf(stderr, "offset이 file size를 초과함\n");
        exit(1);
    }

    fseek(fp, offset, SEEK_SET);
    fwrite(data, sizeof(char) * strlen(data), 1, fp);

    fclose(fp);

    return 0;
}
