#define PRINT_FOR_DEBUG

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <time.h>
#include "blkmap.h"
#include "devicedriver.c"

AddrMapTbl addrmaptbl;
int reserved_empty_blk = DATABLKS_PER_DEVICE;
extern FILE *devicefp;

/****************  prototypes ****************/
void ftl_open();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);
void print_block(int pbn);
void print_addrmaptbl();

//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다
//
void ftl_open()
{
	int i;
    int lbn;
    char pagebuf[PAGE_SIZE], sparebuf[SPARE_SIZE];
    memset(pagebuf, 0xff, PAGE_SIZE);
    memset(sparebuf, 0xff, SPARE_SIZE);

	// initialize the address mapping table
	for(i = 0; i < DATABLKS_PER_DEVICE; i++) //lbn(=i)을 pbn으로 매핑
	{
		addrmaptbl.pbn[i] = -1;
	}

	//
	// 추가적으로 필요한 작업이 있으면 수행하면 되고 없으면 안해도 무방함
	//
    //매핑 테이블 업데이트
    for(i = 0; i < BLOCKS_PER_DEVICE; i++) {
        //각 블록의 첫페이지를 읽음
        if(dd_read(i * PAGES_PER_BLOCK, pagebuf) < 0) {
            fprintf(stderr, "dd_read() error\n");
            exit(1);
        }
        //spare영역에서 dummy에 있을 lbn값을 읽음
        memcpy(sparebuf, pagebuf+SECTOR_SIZE, SPARE_SIZE);
        memcpy(&lbn, sparebuf+sizeof(int), sizeof(int));

        if(lbn >= 0) //빈 블록이 아니라면 매핑테이블에 업데이트
            addrmaptbl.pbn[lbn] = i;
    }
    //freeblock 지정-가능한 값 중 가장 뒷번호가 선택됨
    for(i = DATABLKS_PER_DEVICE; i>=0; i--) {
        if(dd_read(i * PAGES_PER_BLOCK, pagebuf) < 0) {
            fprintf(stderr, "dd_read() error\n");
            exit(1);
        }
        memcpy(sparebuf, pagebuf+SECTOR_SIZE, SPARE_SIZE);
        memcpy(&lbn, sparebuf+sizeof(int), sizeof(int));

        if(lbn == -1) {
            reserved_empty_blk = i;
            break;
        }
    }

	return;
}

//
// file system을 위한 FTL이 제공하는 write interface
// 'sectorbuf'가 가리키는 메모리의 크기는 'SECTOR_SIZE'이며, 호출하는 쪽에서 미리 메모리를 할당받아야 함
//
void ftl_write(int lsn, char *sectorbuf)
{
#ifdef PRINT_FOR_DEBUG			// 필요 시 현재의 block mapping table을 출력해 볼 수 있음
	//print_addrmaptbl();
#endif

	//
	// block mapping 기법에서 overwrite가 발생하면 이를 해결하기 위해 반드시 하나의 empty block이
	// 필요하며, 초기값은 flash memory에서 맨 마지막 block number를 사용함
	// overwrite를 해결하고 난 후 당연히 reserved_empty_blk는 overwrite를 유발시킨 (invalid) block이 되어야 함
	// 따라서 reserved_empty_blk는 고정되어 있는 것이 아니라 상황에 따라 계속 바뀔 수 있음
	//
    int check_lsn;
    char pagebuf[PAGE_SIZE], buffer[PAGE_SIZE], sparebuf[SPARE_SIZE];
    memset(pagebuf, 0xff, PAGE_SIZE);
    memset(buffer, 0xff, PAGE_SIZE);
    memset(sparebuf, 0xff, SPARE_SIZE);

    printf("start write, current freeblock: %d\n", reserved_empty_blk);
    //lsn 통해 ppn구하기
    int lbn = lsn / PAGES_PER_BLOCK;
    printf("what i got lsn data from func call: %d\n", lsn);
    int offset = lsn % PAGES_PER_BLOCK;
    int pbn = addrmaptbl.pbn[lbn];
    int ppn = pbn * PAGES_PER_BLOCK + offset;

    //해당 블록에 최초로 데이터를 쓰는 경우
    if(pbn < 0) {
        pbn = reserved_empty_blk; //pbn 블록을 지정
        addrmaptbl.pbn[lbn] = pbn; //매핑테이블 업데이트
        ppn = pbn * PAGES_PER_BLOCK + offset; //지정된 pbn에 따라 ppn을 새로 구함.
        //해당 pbn 첫 페이지 spare 영역에 lbn 저장
        memcpy(sparebuf+sizeof(int), &lbn, sizeof(int));
        printf("sparebuf: %s\n", sparebuf);
        memcpy(pagebuf+SECTOR_SIZE, sparebuf, SPARE_SIZE);
        printf("pagebuf: %s\n", pagebuf);
        dd_write(pbn * PAGES_PER_BLOCK, pagebuf);
        memset(sparebuf, 0xff, SPARE_SIZE);

        ftl_open();    
        printf("after first write in the block, current freeblock: %d\n", reserved_empty_blk);
    }
    else {
        dd_read(ppn, buffer); //접근하려는 페이지를 읽어 buffer에 저장
        memcpy(&check_lsn, buffer+SECTOR_SIZE, sizeof(int)); //해당 페이지의 lsn을 읽음

        //overwrite인 경우
        if(check_lsn >= 0) {
            //해당 블록에서 새로 쓸 page를 제외한 나머지를 freeblock으로 copy
            for(int i = 0; i < PAGES_PER_BLOCK; i++) {
                if(i == offset)
                    continue;
                else {
                    memset(buffer, 0xff, PAGE_SIZE);
                    dd_read(pbn * PAGES_PER_BLOCK + i, buffer);
                    dd_write(reserved_empty_blk * PAGES_PER_BLOCK + i, buffer);
                }
            }

            //해당 ppn overwrite
            memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
            memcpy(sparebuf, &lsn, sizeof(int));
            memcpy(pagebuf+SECTOR_SIZE, sparebuf, SPARE_SIZE);
            dd_write(reserved_empty_blk * PAGES_PER_BLOCK + offset, pagebuf);
            
            //원래 블록 지우기
            if(dd_erase(pbn) == -1) {
                fprintf(stderr, "dd_erase() error\n");
                exit(1);
            }
            addrmaptbl.pbn[lbn] = reserved_empty_blk; //매핑테이블 업데이트
            reserved_empty_blk = pbn; //freeblock을 좀전에 없앤 블록으로 지정
            printf("after overwrite in the block, current freeblock: %d\n", reserved_empty_blk);
            return;

        }
    }
    memset(pagebuf, 0xff, PAGE_SIZE);
    memset(sparebuf, 0xff, SPARE_SIZE);
    memcpy(pagebuf, sectorbuf, SECTOR_SIZE); //page에 sector 영역 저장
    memcpy(sparebuf, &lsn, sizeof(int)); //spare에 lsn 저장
    memcpy(pagebuf+SECTOR_SIZE, sparebuf, SPARE_SIZE); //page에 spare 영역 저장
    //memcpy(pagebuf+SECTOR_SIZE, &lsn, sizeof(int));
    printf("pagebuf: %s\n", pagebuf);
    printf("spare lsn: %s, func lsn: %d\n", pagebuf+SECTOR_SIZE, lsn);

    if(dd_write(ppn, pagebuf) == -1) {
        fprintf(stderr, "dd_write() error\n");
        exit(1);
    }

	return;
}

//
// file system을 위한 FTL이 제공하는 read interface
// 'sectorbuf'가 가리키는 메모리의 크기는 'SECTOR_SIZE'이며, 호출하는 쪽에서 미리 메모리를 할당받아야 함
// 
void ftl_read(int lsn, char *sectorbuf)
{
    char pagebuf[PAGE_SIZE];
    memset(pagebuf, 0xff, PAGE_SIZE);

    //lsn 통해 ppn구하기
    int lbn = lsn / PAGES_PER_BLOCK;
    int offset = lsn % PAGES_PER_BLOCK;
    int pbn = addrmaptbl.pbn[lbn];
    int ppn = pbn * PAGES_PER_BLOCK + offset;

    //flashmemory에서 ppn에 해당하는 페이지를 읽어와서 sectorbuf에 저장
    if(dd_read(ppn, pagebuf) == -1) {
        fprintf(stderr, "dd_read() error\n");
        exit(1);
    }

    memcpy(sectorbuf, pagebuf, SECTOR_SIZE);

#ifdef PRINT_FOR_DEBUG			// 필요 시 현재의 block mapping table을 출력해 볼 수 있음
	//print_addrmaptbl();
#endif

	return;
}

//
// for debugging
//
void print_block(int pbn)
{
	char *pagebuf;
	SpareData *sdata;
	int i;
	
	pagebuf = (char *)malloc(PAGE_SIZE);
	sdata = (SpareData *)malloc(SPARE_SIZE);

	printf("Physical Block Number: %d\n", pbn);

	for(i = pbn*PAGES_PER_BLOCK; i < (pbn+1)*PAGES_PER_BLOCK; i++)
	{
		dd_read(i, pagebuf);
		memcpy(sdata, pagebuf+SECTOR_SIZE, SPARE_SIZE);
		printf("\t   %5d-[%7d]\n", i, sdata->lsn);
	}

	free(pagebuf);
	free(sdata);

	return;
}

//
// for debugging
//
void print_addrmaptbl()
{
	int i;

	printf("Address Mapping Table: \n");
	for(i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		if(addrmaptbl.pbn[i] >= 0)
		{
			printf("[%d %d]\n", i, addrmaptbl.pbn[i]);
		}
	}
}
