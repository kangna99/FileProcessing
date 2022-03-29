#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int offset, file_size;
    char *data, *buffer;
    FILE *fp;

    if(argc != 4) {
        fprintf(stderr, "argc count error\n");
        exit(1);
    }

    if((fp = fopen(argv[3], "r+")) == NULL) {
        fprintf(stderr, "fopen error\n");
        exit(1);
    }

    offset = atoi(argv[1]);
    data = argv[2];

    //파일 사이즈 계산
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    if(offset > file_size-1) {    
        fprintf(stderr, "offset이 파일 크기를 초과함.\n");
        exit(1);
    }

    buffer = (char*)malloc(sizeof(char) * (file_size-offset-1));

    fseek(fp, offset+1, SEEK_SET);
    fread(buffer, sizeof(char) * (file_size-offset), 1, fp);
    rewind(fp);

    fseek(fp, offset+1, SEEK_SET);
    fwrite(data, sizeof(char) * strlen(data), 1, fp);
    fwrite(buffer, sizeof(char) * strlen(buffer), 1, fp);

    fclose(fp);
    free(buffer);

    return 0;
}
