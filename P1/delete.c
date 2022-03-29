#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int offset, byte_size, file_size;
    char *buffer;
    int fd;

    if(argc != 4) {
        fprintf(stderr, "argc count error\n");
        exit(1);
    }

    if((fd = open(argv[3], O_RDWR)) < 0) {
        fprintf(stderr, "open error\n");
        exit(1);
    }

    offset = atoi(argv[1]);
    byte_size = abs(atoi(argv[2]));

    //파일 사이즈 계산
    file_size = lseek(fd, (off_t)0, SEEK_END);

    if((offset < 0) || (offset > file_size -1)) {
        fprintf(stderr, "offset error\n");
        exit(1);
    }

    lseek(fd, offset, SEEK_SET);

    //바이트 수가 음수이면
    if(atoi(argv[2]) < 0) {
        buffer = (char*)malloc(sizeof(char) * (file_size-offset)+1);
        read(fd, buffer, sizeof(char) * (file_size-offset));

        //바이트 수가 파일 범위를 초과하면
        if(byte_size > offset)
            lseek(fd, 0, SEEK_SET);
        else
            lseek(fd, offset-byte_size, SEEK_SET);

        write(fd, buffer, sizeof(char) * strlen(buffer));
    }

    //바이트 수가 양수이면
    else {
        //바이트 수가 파일 범위를 초과하면
        if(byte_size > file_size - offset-1) {
            lseek(fd, 1, SEEK_CUR);
        }
        else {
            lseek(fd, byte_size+1, SEEK_CUR);
            buffer = (char*)malloc(sizeof(char) * (file_size-offset-byte_size-1));
            read(fd, buffer, sizeof(char) * (file_size-offset-byte_size-1));
            lseek(fd, offset+1, SEEK_SET);
            write(fd, buffer, sizeof(char) * strlen(buffer));
        }
    }

    file_size = lseek(fd, 0, SEEK_CUR);
    ftruncate(fd, file_size);

    close(fd);
    free(buffer);

    return 0;
}
