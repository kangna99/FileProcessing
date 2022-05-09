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
// flash memory�� ó�� ����� �� �ʿ��� �ʱ�ȭ �۾�, ���� ��� address mapping table�� ����
// �ʱ�ȭ ���� �۾��� �����Ѵ�
//
void ftl_open()
{
	int i;
    int lbn;
    char pagebuf[PAGE_SIZE], sparebuf[SPARE_SIZE];
    memset(pagebuf, 0xff, PAGE_SIZE);
    memset(sparebuf, 0xff, SPARE_SIZE);

	// initialize the address mapping table
	for(i = 0; i < DATABLKS_PER_DEVICE; i++) //lbn(=i)�� pbn���� ����
	{
		addrmaptbl.pbn[i] = -1;
	}

	//
	// �߰������� �ʿ��� �۾��� ������ �����ϸ� �ǰ� ������ ���ص� ������
	//
    //���� ���̺� ������Ʈ
    for(i = 0; i < BLOCKS_PER_DEVICE; i++) {
        //�� ����� ù�������� ����
        if(dd_read(i * PAGES_PER_BLOCK, pagebuf) < 0) {
            fprintf(stderr, "dd_read() error\n");
            exit(1);
        }
        //spare�������� dummy�� ���� lbn���� ����
        memcpy(sparebuf, pagebuf+SECTOR_SIZE, SPARE_SIZE);
        memcpy(&lbn, sparebuf+sizeof(int), sizeof(int));

        if(lbn >= 0) //�� ����� �ƴ϶�� �������̺� ������Ʈ
            addrmaptbl.pbn[lbn] = i;
    }
    //freeblock ����-������ �� �� ���� �޹�ȣ�� ���õ�
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
// file system�� ���� FTL�� �����ϴ� write interface
// 'sectorbuf'�� ����Ű�� �޸��� ũ��� 'SECTOR_SIZE'�̸�, ȣ���ϴ� �ʿ��� �̸� �޸𸮸� �Ҵ�޾ƾ� ��
//
void ftl_write(int lsn, char *sectorbuf)
{
#ifdef PRINT_FOR_DEBUG			// �ʿ� �� ������ block mapping table�� ����� �� �� ����
	//print_addrmaptbl();
#endif

	//
	// block mapping ������� overwrite�� �߻��ϸ� �̸� �ذ��ϱ� ���� �ݵ�� �ϳ��� empty block��
	// �ʿ��ϸ�, �ʱⰪ�� flash memory���� �� ������ block number�� �����
	// overwrite�� �ذ��ϰ� �� �� �翬�� reserved_empty_blk�� overwrite�� ���߽�Ų (invalid) block�� �Ǿ�� ��
	// ���� reserved_empty_blk�� �����Ǿ� �ִ� ���� �ƴ϶� ��Ȳ�� ���� ��� �ٲ� �� ����
	//
    int check_lsn;
    char pagebuf[PAGE_SIZE], buffer[PAGE_SIZE], sparebuf[SPARE_SIZE];
    memset(pagebuf, 0xff, PAGE_SIZE);
    memset(buffer, 0xff, PAGE_SIZE);
    memset(sparebuf, 0xff, SPARE_SIZE);

    printf("start write, current freeblock: %d\n", reserved_empty_blk);
    //lsn ���� ppn���ϱ�
    int lbn = lsn / PAGES_PER_BLOCK;
    printf("what i got lsn data from func call: %d\n", lsn);
    int offset = lsn % PAGES_PER_BLOCK;
    int pbn = addrmaptbl.pbn[lbn];
    int ppn = pbn * PAGES_PER_BLOCK + offset;

    //�ش� ��Ͽ� ���ʷ� �����͸� ���� ���
    if(pbn < 0) {
        pbn = reserved_empty_blk; //pbn ����� ����
        addrmaptbl.pbn[lbn] = pbn; //�������̺� ������Ʈ
        ppn = pbn * PAGES_PER_BLOCK + offset; //������ pbn�� ���� ppn�� ���� ����.
        //�ش� pbn ù ������ spare ������ lbn ����
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
        dd_read(ppn, buffer); //�����Ϸ��� �������� �о� buffer�� ����
        memcpy(&check_lsn, buffer+SECTOR_SIZE, sizeof(int)); //�ش� �������� lsn�� ����

        //overwrite�� ���
        if(check_lsn >= 0) {
            //�ش� ��Ͽ��� ���� �� page�� ������ �������� freeblock���� copy
            for(int i = 0; i < PAGES_PER_BLOCK; i++) {
                if(i == offset)
                    continue;
                else {
                    memset(buffer, 0xff, PAGE_SIZE);
                    dd_read(pbn * PAGES_PER_BLOCK + i, buffer);
                    dd_write(reserved_empty_blk * PAGES_PER_BLOCK + i, buffer);
                }
            }

            //�ش� ppn overwrite
            memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
            memcpy(sparebuf, &lsn, sizeof(int));
            memcpy(pagebuf+SECTOR_SIZE, sparebuf, SPARE_SIZE);
            dd_write(reserved_empty_blk * PAGES_PER_BLOCK + offset, pagebuf);
            
            //���� ��� �����
            if(dd_erase(pbn) == -1) {
                fprintf(stderr, "dd_erase() error\n");
                exit(1);
            }
            addrmaptbl.pbn[lbn] = reserved_empty_blk; //�������̺� ������Ʈ
            reserved_empty_blk = pbn; //freeblock�� ������ ���� ������� ����
            printf("after overwrite in the block, current freeblock: %d\n", reserved_empty_blk);
            return;

        }
    }
    memset(pagebuf, 0xff, PAGE_SIZE);
    memset(sparebuf, 0xff, SPARE_SIZE);
    memcpy(pagebuf, sectorbuf, SECTOR_SIZE); //page�� sector ���� ����
    memcpy(sparebuf, &lsn, sizeof(int)); //spare�� lsn ����
    memcpy(pagebuf+SECTOR_SIZE, sparebuf, SPARE_SIZE); //page�� spare ���� ����
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
// file system�� ���� FTL�� �����ϴ� read interface
// 'sectorbuf'�� ����Ű�� �޸��� ũ��� 'SECTOR_SIZE'�̸�, ȣ���ϴ� �ʿ��� �̸� �޸𸮸� �Ҵ�޾ƾ� ��
// 
void ftl_read(int lsn, char *sectorbuf)
{
    char pagebuf[PAGE_SIZE];
    memset(pagebuf, 0xff, PAGE_SIZE);

    //lsn ���� ppn���ϱ�
    int lbn = lsn / PAGES_PER_BLOCK;
    int offset = lsn % PAGES_PER_BLOCK;
    int pbn = addrmaptbl.pbn[lbn];
    int ppn = pbn * PAGES_PER_BLOCK + offset;

    //flashmemory���� ppn�� �ش��ϴ� �������� �о�ͼ� sectorbuf�� ����
    if(dd_read(ppn, pagebuf) == -1) {
        fprintf(stderr, "dd_read() error\n");
        exit(1);
    }

    memcpy(sectorbuf, pagebuf, SECTOR_SIZE);

#ifdef PRINT_FOR_DEBUG			// �ʿ� �� ������ block mapping table�� ����� �� �� ����
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
