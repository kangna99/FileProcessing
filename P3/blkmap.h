#ifndef	_BLOCK_MAPPING_H_
#define	_BLOCK_MAPPING_H_

#define TRUE			1
#define	FALSE			0

#define	SECTOR_SIZE		512			
#define	SPARE_SIZE		16			
#define	PAGE_SIZE		(SECTOR_SIZE+SPARE_SIZE)
#define 	SECTORS_PER_PAGE	1
#define	PAGES_PER_BLOCK		32					// ���� ����
#define	SECTORS_PER_BLOCK	(SECTORS_PER_PAGE*PAGES_PER_BLOCK)
#define	BLOCK_SIZE		(PAGE_SIZE*PAGES_PER_BLOCK)
#define	BLOCKS_PER_DEVICE	1024					// ���� ����
#define	DEVICE_SIZE		(BLOCK_SIZE*BLOCKS_PER_DEVICE)
#define	DATABLKS_PER_DEVICE	(BLOCKS_PER_DEVICE - 1)			// spare block�μ� ��� �ϳ��� ���
//
// �ʿ��ϸ� ����� �߰��ؼ� ��� ������
// 

//
// flash memory�� spare area�� �ٷ�� ���� ����ü
//
typedef struct
{
	int lsn;			// page�� �����͸� ������ �� spare area�� lsn�� ���� ������
	char dummy[SPARE_SIZE - 4];
} SpareData;

//
// FTL�� �����ϴ� address mapping table�� ���� ����ü
//
typedef struct
{
	int pbn[DATABLKS_PER_DEVICE];
} AddrMapTbl;

#endif
