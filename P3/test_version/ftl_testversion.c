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
void freeblock();

//
// flash memory�� ó�� ����� �� �ʿ��� �ʱ�ȭ �۾�, ���� ��� address mapping table�� ����
// �ʱ�ȭ ���� �۾��� �����Ѵ�
//
void ftl_open()
{
	int i;

	// initialize the address mapping table
	for(i = 0; i < DATABLKS_PER_DEVICE; i++) //lbn(=i)�� pbn���� ����
	{
		addrmaptbl.pbn[i] = -1;
	}

	//
	// �߰������� �ʿ��� �۾��� ������ �����ϸ� �ǰ� ������ ���ص� ������
	//

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
    int check_lsn, check_lbn;
    char pagebuf[PAGE_SIZE], buffer[PAGE_SIZE], sparebuf[SPARE_SIZE];
    memset(pagebuf, 0xff, PAGE_SIZE);
    memset(buffer, 0xff, PAGE_SIZE);
    memset(sparebuf, 0xff, SPARE_SIZE);

    //lsn ���� ppn���ϱ�
    int lbn = lsn / PAGES_PER_BLOCK;
    int offset = lsn % PAGES_PER_BLOCK;
    int pbn = addrmaptbl.pbn[lbn];
    int ppn = pbn * PAGES_PER_BLOCK + offset;
    
    //�ش� ��Ͽ� ���ʷ� �����͸� ���� ���
    if(pbn < 0) {
        pbn = reserved_empty_blk; //pbn ����� ���� �Ҵ�
        ppn = pbn * PAGES_PER_BLOCK + offset; //������ pbn�� ���� ppn�� ���� ����
        printf("lsn: %d, lbn: %d, offset: %d, pbn: %d, ppn: %d\n", lsn, lbn, offset, pbn, ppn);
        
        //�ش� pbn ù������ spare ������ lbn�� ����
        memcpy(pagebuf+SECTOR_SIZE+sizeof(int), &lbn, sizeof(int));
        dd_write(ppn - offset, pagebuf);
        memset(pagebuf, 0xff, PAGE_SIZE);

        addrmaptbl.pbn[lbn] = pbn; //�������̺� ������Ʈ
        freeblock(); //freeblock ����- ������ �� �� ���� �޹�ȣ�� ���õ�
        printf(">>> new freeblock: %d\n", reserved_empty_blk);
    }
    else {
        printf("lsn: %d, lbn: %d, offset: %d, pbn: %d, ppn: %d\n", lsn, lbn, offset, pbn, ppn);
        //�ش� ppn�� �о spare������ lsn�� Ȯ��
        dd_read(ppn, buffer);
        memcpy(&check_lsn, buffer+SECTOR_SIZE, sizeof(int));
        //offset�� 0�� �ƴ϶�� �ش� ����� ù�������� �о����
        if(offset != 0) {
            memset(buffer, 0xff, PAGE_SIZE);
            dd_read(ppn-offset, buffer);
        }
        memcpy(&check_lbn, buffer+SECTOR_SIZE+sizeof(int), sizeof(int)); //spare ���� lbn Ȯ��
        //overwrite�� ���
        if(check_lsn >= 0) {
            //�ش� pbn�� ��� ���������� freeblock���� copy
            for(int i = 0; i<PAGES_PER_BLOCK; i++) {
                if(i == offset)
                    continue;
                else {
                    memset(buffer, 0xff, PAGE_SIZE);
                    dd_read(pbn * PAGES_PER_BLOCK + i, buffer);
                    dd_write(reserved_empty_blk * PAGES_PER_BLOCK + i, buffer);
                }
            }
            //�ش� ppn�� ���ο� ��Ͽ� overwrite
            memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
            memcpy(sparebuf, &lsn, sizeof(int));
            //���� offset 0�� �ű�� ���, ���ο� lbn�� ����� ù��° ������ spare ������ ����
            if(offset == 0) {
                memcpy(sparebuf+sizeof(int), &lbn, sizeof(int));
            }
            memcpy(pagebuf+SECTOR_SIZE, sparebuf, SPARE_SIZE);
            dd_write(reserved_empty_blk * PAGES_PER_BLOCK + offset, pagebuf);

            //���� ��� �����
            if(dd_erase(pbn) == -1) {
                fprintf(stderr, "dd_erase() error\n");
                exit(1);
            }
            printf("===== BLOCK ERASE =====\n");
            addrmaptbl.pbn[lbn] = reserved_empty_blk; //�������̺� ������Ʈ
            printf("%d(removed) -> %d(new block)\n", pbn, reserved_empty_blk);
            reserved_empty_blk = pbn; //freeblock�� �� ���� ���� ������� ����
            
            return;
        }
    }
    
    //ppn�� write
    memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
    memcpy(sparebuf, &lsn, sizeof(int));
    if(offset == 0) {
        memcpy(sparebuf+sizeof(int), &lbn, sizeof(int));
    }
    memcpy(pagebuf+SECTOR_SIZE, sparebuf, SPARE_SIZE);
    
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

void freeblock()
{
    //�ǵ� pbn���� üũ�ؼ� ���� pbn�� ���� ù������ spare���� lbn���� -1�̸� freeblock
    int lbn;
    char pagebuf[PAGE_SIZE];

    for(int i = DATABLKS_PER_DEVICE; i >= 0; i--) {
        if(dd_read(i * PAGES_PER_BLOCK, pagebuf) < 0) {
            fprintf(stderr, "dd_read() error\n");
            exit(1);
        }
        memcpy(&lbn, pagebuf+SECTOR_SIZE+sizeof(int), sizeof(int));
        if(lbn == -1) {
            reserved_empty_blk = i;
            break;
        }
        else
            continue;
    }
    return;
}
