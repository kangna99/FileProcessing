#define PRINT_FOR_DEBUG

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <time.h>
#include "blkmap.h"

AddrMapTable addrmaptbl;
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

	// initialize the address mapping table
	for(i = 0; i < DATABLKS_PER_DEVICE; i++)
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
void ftl_write(int lsn, char *sectorbuf);
{
#ifdef PRINT_FOR_DEBUG			// �ʿ� �� ������ block mapping table�� ����� �� �� ����
	print_addrmaptbl_info();
#endif

	//
	// block mapping ������� overwrite�� �߻��ϸ� �̸� �ذ��ϱ� ���� �ݵ�� �ϳ��� empty block��
	// �ʿ��ϸ�, �ʱⰪ�� flash memory���� �� ������ block number�� �����
	// overwrite�� �ذ��ϰ� �� �� �翬�� reserved_empty_blk�� overwrite�� ���߽�Ų (invalid) block�� �Ǿ�� ��
	// ���� reserved_empty_blk�� �����Ǿ� �ִ� ���� �ƴ϶� ��Ȳ�� ���� ��� �ٲ� �� ����
	//
	int reserved_empty_blk = DATABLKS_PER_DEVICE;


	return;
}

//
// file system�� ���� FTL�� �����ϴ� read interface
// 'sectorbuf'�� ����Ű�� �޸��� ũ��� 'SECTOR_SIZE'�̸�, ȣ���ϴ� �ʿ��� �̸� �޸𸮸� �Ҵ�޾ƾ� ��
// 
void ftl_read(int lsn, char *sectorbuf);
{
#ifdef PRINT_FOR_DEBUG			// �ʿ� �� ������ block mapping table�� ����� �� �� ����
	print_addrmaptbl_info();
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
		read(i, pagebuf);
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
