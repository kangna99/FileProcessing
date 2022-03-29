#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    FILE *fp;
    int offset, byte_size;
    char *buffer;

    //에러 처리(인자오류)
    if(argc != 4) {
        fprintf(stderr, "argc count error\n");
        exit(1);
    }

    if((fp = fopen(argv[3], "r")) == NULL) {
        fprintf(stderr, "fopen error\n");
        exit(1);
    }

    offset = atoi(argv[1]);
    byte_size = abs(atoi(argv[2]));

    buffer = (char*)malloc(sizeof(char) * byte_size);
    
    fseek(fp, offset, SEEK_SET); //offset만큼 이동

    if(atoi(argv[2]) < 0) { //바이트 수가 음수이면
        if(byte_size > offset) { //존재하는 만큼만 읽음
            fseek(fp, -offset, SEEK_CUR);
            fread(buffer, sizeof(char) * offset, 1, fp);
            fwrite(buffer, sizeof(char) * offset, 1, stdout);
        }
        else {
            fseek(fp, atoi(argv[2]), SEEK_CUR);
            fread(buffer, sizeof(char) * byte_size, 1, fp);
            fwrite(buffer, sizeof(char) * byte_size, 1, stdout);
        }
    }
    else { //바이트 수가 양수이면
        fseek(fp, 1, SEEK_CUR);
        fread(buffer, sizeof(char) * byte_size, 1, fp);
        fwrite(buffer, sizeof(char) * byte_size, 1, stdout);
    }

    printf("\n");

    fclose(fp);
    free(buffer);

    return 0;
}
