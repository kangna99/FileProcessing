#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *buf1, *buf2;
    int f1_size, f2_size;
    FILE *original_fp1, *original_fp2, *new_fp;

    if(argc != 4) {
        fprintf(stderr, "argc count error\n");
        exit(1);
    }

    if((original_fp1 = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "original file1 open error\n");
        exit(1);
    }

    if((original_fp2 = fopen(argv[2], "r")) == NULL) {
        fprintf(stderr, "original file2 open error\n");
        exit(1);
    }

    if((new_fp = fopen(argv[3], "w")) == NULL) {
        fprintf(stderr, "new file open error\n");
        exit(1);
    }

    //파일 사이즈 계산
    fseek(original_fp1, 0, SEEK_END);
    f1_size = ftell(original_fp1);
    rewind(original_fp1);

    fseek(original_fp2, 0, SEEK_END);
    f2_size = ftell(original_fp2);
    rewind(original_fp2);

    buf1 = (char*)malloc(sizeof(char) * f1_size);
    buf2 = (char*)malloc(sizeof(char) * f2_size);

    //파일 읽기
    fread(buf1, sizeof(char) * f1_size, 1, original_fp1);
    fread(buf2, sizeof(char) * f2_size, 1, original_fp2);

    //파일 병합
    fwrite(buf1, sizeof(char) * f1_size, 1, new_fp);
    fwrite(buf2, sizeof(char) * f2_size, 1, new_fp);

    fclose(original_fp1);
    fclose(original_fp2);
    fclose(new_fp);
    free(buf1);
    free(buf2);

    return 0;
}
