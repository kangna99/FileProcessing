﻿#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "blkmap.h"
#include "ftl.c"

FILE *devicefp;

int main( )
{
	// 아래 변수는 테스트할 때 필요하면 사용하기 바람
	char sectorbuf[SECTOR_SIZE], *blockbuf;
	int i, lsn;

	// flash memory의 파일명은 'flashmemory'을 가정함
	devicefp = fopen("flashmemory", "w+b");	

	if(devicefp == NULL)
	{
		printf("file open error\n");
		exit(1);
	}

	//
	// initialize flash memory where each byte are set to '0xFF'
	// 
	blockbuf = (char *)malloc(BLOCK_SIZE);
	memset(blockbuf, 0xFF, BLOCK_SIZE);

	for(i = 0; i < BLOCKS_PER_DEVICE; i++)
	{
		fwrite(blockbuf, BLOCK_SIZE, 1, devicefp);
	}

	ftl_open();

	//
	// ftl_write() 및 ftl_read() 테스트를 위한 코드를 자유자재로 만드세요.
	// 채점할 때 이 부분에 테스트 코드를 심어서 합니다. Flash memory에 대한 데이터의 읽기 및 쓰기가
	// 올바르게 동작하는지를 테스트하고, 필요하면 다른 부분도 검사를 합니다.
	//
    /**
    memset(sectorbuf, 0xff, SECTOR_SIZE);
    strcpy(sectorbuf, "@@123abc");
    ftl_write(128, sectorbuf);
    ftl_write(63, sectorbuf);
    ftl_write(64, sectorbuf);
    ftl_write(0, sectorbuf);
    ftl_write(17, sectorbuf);
    ftl_write(99, sectorbuf);
    ftl_write(200, sectorbuf);

    print_addrmaptbl();
    print_block(1023);
    print_block(1022);
    print_block(1021);
    print_block(1020);
    print_block(1019);
    print_block(1018);
    print_block(1017);

    ftl_write(128, sectorbuf);
    ftl_write(64, sectorbuf);
    ftl_write(63, sectorbuf);
    memset(sectorbuf, 0xff, SECTOR_SIZE);
    printf("===== main func maptable =====\n");
    print_addrmaptbl();
    ftl_read(0, sectorbuf);
    printf("read: %s\n", sectorbuf);
    print_block(1023);
    print_block(1022);
    print_block(1021);
    print_block(1020);
    print_block(1019);
    print_block(1018);
    print_block(1017);
    **/
	free(blockbuf);
	fclose(devicefp);

	return 0;
}
