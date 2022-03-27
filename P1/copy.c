#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    FILE *original_fp;
    FILE *new_fp;
    int file_size = 0, buf_size = 0;
    char buffer[10];
    memset(buffer, 0, sizeof(buffer));
    char *buf;

    //에러 처리(인자오류)
    if(argc != 3) {
        fprintf(stderr, "argc count error\n");
        exit(1);
    }

    //정상 작동
    else {
        if((original_fp = fopen(argv[1], "r")) == NULL) {
            fprintf(stderr, "original file open error\n");
            exit(1);
        }

        if((new_fp = fopen(argv[2], "w")) == NULL) {
            fprintf(stderr, "new file open error\n");
            exit(1);
        }

        //파일 사이즈 계산
        fseek(original_fp, 0, SEEK_END);
        file_size = ftell(original_fp);
        buf_size = file_size % 10;
        rewind(original_fp);

        //10바이트 단위로 copy
        for(int i = 0; i < (file_size / 10); i++) {
            if(fread(buffer, sizeof(buffer), 1, original_fp) != 1) {
                fprintf(stderr, "fread error\n");
                exit(1);
            }
            
            if(fwrite(buffer, sizeof(buffer), 1, new_fp) != 1) {
                fprintf(stderr, "fwrite error\n");
                exit(1);
            }
        }

        //나머지(10바이트 미만) copy
        buf = (char*)malloc(sizeof(char) * buf_size);

        fread(buf, sizeof(char) * buf_size, 1, original_fp);
        fwrite(buf, sizeof(char) * buf_size, 1, new_fp);

        fclose(original_fp);
        fclose(new_fp);
        free(buf);
    }

    return 0;
}
