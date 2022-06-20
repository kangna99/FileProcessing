#include <stdio.h>		// �ʿ��� header file �߰� ����
#include <stdlib.h>
#include <string.h>
#include "student.h"

//
// �Լ� readRecord()�� �л� ���ڵ� ���Ͽ��� �־��� rrn�� �ش��ϴ� ���ڵ带 �о 
// recordbuf�� �����ϰ�, ���� unpack() �Լ��� ȣ���Ͽ� �л� Ÿ���� ������ ���ڵ���
// �� �ʵ尪�� �����Ѵ�. �����ϸ� 1�� �׷��� ������ 0�� �����Ѵ�.
// unpack() �Լ��� recordbuf�� ����Ǿ� �ִ� record���� �� field�� �����ϴ� ���� �Ѵ�.
//
int readRecord(FILE *fp, STUDENT *s, int rrn);
void unpack(const char *recordbuf, STUDENT *s);

//
// �Լ� writeRecord()�� �л� ���ڵ� ���Ͽ� �־��� rrn�� �ش��ϴ� ��ġ�� recordbuf�� 
// ����Ǿ� �ִ� ���ڵ带 �����Ѵ�. ������ pack() �Լ��� ȣ���Ͽ� recordbuf�� �����͸� ä�� �ִ´�.
// ���������� �����ϸ� '1'��, �׷��� ������ '0'�� �����Ѵ�.
//
int writeRecord(FILE *fp, const STUDENT *s, int rrn);
void pack(char *recordbuf, const STUDENT *s);

//
// �Լ� appendRecord()�� �л� ���ڵ� ���Ͽ� ���ο� ���ڵ带 append�Ѵ�.
// ���ڵ� ���Ͽ� ���ڵ尡 �ϳ��� �������� �ʴ� ��� (ù ��° append)�� header ���ڵ带
// ���Ͽ� �����ϰ� ù ��° ���ڵ带 �����Ѵ�. 
// �翬�� ���ڵ带 append�� �� ������ header ���ڵ忡 ���� ������ �ڵ���� �Ѵ�.
// �Լ� appendRecord()�� ���������� writeRecord() �Լ��� ȣ���Ͽ� ���ڵ� ������ �ذ��Ѵ�.
// ���������� �����ϸ� '1'��, �׷��� ������ '0'�� �����Ѵ�.
//
int appendRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email);

//
// �л� ���ڵ� ���Ͽ��� �˻� Ű���� �����ϴ� ���ڵ尡 �����ϴ����� sequential search ����� 
// ���� ã�Ƴ���, �̸� �����ϴ� ��� ���ڵ��� ������ ����Ѵ�. �˻� Ű�� �л� ���ڵ带 �����ϴ�
// � �ʵ嵵 �����ϴ�. ���������� readRecord() �Լ��� ȣ���Ͽ� sequential search�� �����Ѵ�.
// �˻� ����� ����� �� �ݵ�� printRecord() �Լ��� ����Ѵ�. (�ݵ�� ���Ѿ� �ϸ�, �׷���
// �ʴ� ��� ä�� ���α׷����� �ڵ������� Ʋ�� ������ �ν���)
//
void searchRecord(FILE *fp, enum FIELD f, char *keyval);
void printRecord(const STUDENT *s);

//
// ���ڵ��� �ʵ���� enum FIELD Ÿ���� ������ ��ȯ���� �ش�.
// ���� ���, ����ڰ� ������ ��ɾ��� ���ڷ� "NAME"�̶�� �ʵ���� ����Ͽ��ٸ� 
// ���α׷� ���ο��� �̸� NAME(=1)���� ��ȯ�� �ʿ伺�� ������, �̶� �� �Լ��� �̿��Ѵ�.
//
enum FIELD getFieldID(char *fieldname);

//
// �л� ���ڵ� ���Ͽ��� "�й�" Ű���� �����ϴ� ���ڵ带 ã�Ƽ� �̰��� �����Ѵ�.
// �����, �˻� ������ �ݵ�� �й�(ID)���� ����Ѵ�. ���ڵ��� �˻��� searchRecord() �Լ���
// ������ ������� �����Ѵ�. ���������� �����ϸ� '1'��, �׷��� ������ '0'�� �����Ѵ�.
//
int deleteRecord(FILE *fp, enum FIELD f, char *keyval);

//
// �л� ���ڵ� ���Ͽ� ���ο� ���ڵ带 �߰��Ѵ�. ���� �������� ����� ���, ���� ���ڵ尡
// �����ϸ� �ݵ�� ���� ���ڵ�� �� �ϳ��� ���ο� ���ڵ带 �����Ѵ�. ���� ���ڵ� ����Ʈ ������
// ���� ���ڵ��� ������ ���� ����� ������. ���ο� ���ڵ��� �߰��� appendRecord() �Լ��� �����
// ������� �����Ѵ�. ���������� �����ϸ� '1'��, �׷��� ������ '0'�� �����Ѵ�.
//
int insertRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email);

void main(int argc, char *argv[])
{
	FILE *fp;			// ��� file processing operation�� C library�� ����� ��
    char *token;
    char field[10];
    char value[MAX_VALUE + 1];
    memset(field, 0, sizeof(field));
    memset(value, 0, sizeof(value));

    //���ڵ� ������ ������ ���� ������ ���ο� ���� ����
    if((fp = fopen(argv[2], "r+")) == NULL) {
        fp = fopen(argv[2], "w+");
    }

    //���1) ���ڵ� �߰� -a �ɼ�
    if(!strcmp(argv[1], "-a")) {

        //����ó��
        if(argc != 8) {
            fprintf(stderr, "input error\nusage: %s -a <record_file_name> \"id\" \"name\" \"dept\" \"addr\" \"email\"\n", argv[0]);
            exit(1);
        }
        if(strlen(argv[3]) > 8) {
            fprintf(stderr, "id max length is 8\n");
            exit(1);
        }
        if(strlen(argv[4]) > 10) {
            fprintf(stderr, "name max length is 10\n");
            exit(1);
        }
        if(strlen(argv[5]) > 12) {
            fprintf(stderr, "dept max length is 12\n");
            exit(1);
        }
        if(strlen(argv[6]) > 30) {
            fprintf(stderr, "addr max length is 30\n");
            exit(1);
        }
        if(strlen(argv[7]) > 20) {
            fprintf(stderr, "email max length is 20\n");
            exit(1);
        }

        //�Է¹��� ������ ���ڵ� �߰�
        if((appendRecord(fp, argv[3], argv[4], argv[5], argv[6], argv[7])) == 0) {
            fprintf(stderr, "appendRecord fail\n");
            exit(1);
        }
    }

    //���2) ���ڵ� �˻� -s �ɼ�
    if(!strcmp(argv[1], "-s")) {

        //����ó��
        if(argc != 4) {
            fprintf(stderr, "input error\nusage: %s -s <record_file_name> \"field_name = field_value\"\n", argv[0]);
            exit(1);
        }
        //�˻��� �Է°��� �ʵ�� ������ split
        token = strtok(argv[3], "=");
        strcpy(field, token);
        token = strtok(NULL, "=");
        strcpy(value, token);

        searchRecord(fp, getFieldID(field), value);
    }

    //���3) ���ڵ� ���� -d �ɼ�
    if(!strcmp(argv[1], "-d")) {

        //����ó��
        if(argc != 4) {
            fprintf(stderr, "input error\nusage: %s -d <record_file_name> \"ID = field_value\"\n", argv[0]);
            exit(1);
        }
        //������ �Է°��� �ʵ�� ������ split
        token = strtok(argv[3], "=");
        strcpy(field, token);
        token = strtok(NULL, "=");
        strcpy(value, token);
        
        if(strcmp(field, "ID") != 0) {
            fprintf(stderr, "input error: Field should be \"ID\"\n");
            exit(1);
        }

        if(deleteRecord(fp, getFieldID(field), value) == 0) {
            fprintf(stderr, "deleteRecord fail\n");
            exit(1);
        }
    }

    return;
}

void printRecord(const STUDENT *s)
{
	printf("%s | %s | %s | %s | %s\n", s->id, s->name, s->dept, s->addr, s->email);
}

void unpack(const char *recordbuf, STUDENT *s)
{
    sscanf(recordbuf, "%[^'#']#%[^'#']#%[^'#']#%[^'#']#%[^'#']#", s->id, s->name, s->dept, s->addr, s->email);
}

void pack(char *recordbuf, const STUDENT *s)
{
    sprintf(recordbuf, "%s#%s#%s#%s#%s#", s->id, s->name, s->dept, s->addr, s->email);
}

enum FIELD getFieldID(char *fieldname)
{
    if(!strcmp(fieldname, "ID"))
        return ID;
    else if(!strcmp(fieldname, "NAME"))
        return NAME;
    else if(!strcmp(fieldname, "DEPT"))
        return DEPT;
    else if(!strcmp(fieldname, "ADDR"))
        return ADDR;
    else if(!strcmp(fieldname, "EMAIL"))
        return EMAIL;
    else {
        fprintf(stderr, "getFieldID error\n");
        exit(1);
    }
}

int appendRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email)
{
    int record_cnt = 0; //���Ͽ� ����� ���ڵ� ��
    int new_record_cnt;
    int rrn = 0;
    int result; //writeRecord ���� ��ȯ��
    char header[HEADER_SIZE]; //���
    memset(header, 0, sizeof(int)); //��ü���ڵ�� �ʱ�ȭ
    memset(header+sizeof(int), -1, sizeof(int)); //�������ڵ� ��� �ʱ�ȭ

    //�л� ����ü ���� �� �ʱ�ȭ
    STUDENT *s;
    s = (STUDENT *)malloc(sizeof(STUDENT));
    memset(s, 0, sizeof(STUDENT));
    strcpy(s->id, id);
    strcpy(s->name, name);
    strcpy(s->dept, dept);
    strcpy(s->addr, addr);
    strcpy(s->email, email);

    //ó�� �� �� header ���ڵ带 ���Ͽ� �����ϰ� rrn=0, ���Ŀ� rrn�� ��� ������� ã�� 
    if(fread(&record_cnt, sizeof(int), 1, fp) == 0) {
        fwrite(header, sizeof(header), 1, fp);
    }
    else {
        rrn = record_cnt;
    }
    //rrn�� record �����ϰ� header ����
    if((result = writeRecord(fp, s, rrn)) == 1) { //���������� ����
        rewind(fp);
        new_record_cnt = record_cnt + 1;
        fwrite(&new_record_cnt, sizeof(int), 1, fp);
    }
    return result;
}

int writeRecord(FILE *fp, const STUDENT *s, int rrn)
{
    //�־��� rrn ��ġ�� recordbuf�� ����� ���ڵ带 ����
    char recordbuf[RECORD_SIZE];
    memset(recordbuf, 0, sizeof(recordbuf));
    pack(recordbuf, s);
    fseek(fp, HEADER_SIZE + RECORD_SIZE * rrn, SEEK_SET);

    //���������� �����ϸ� 1�� ����, ���� �� 0�� ����
    if((fwrite(recordbuf, sizeof(recordbuf), 1, fp)) == 1)
        return 1;
    else return 0;
}

void searchRecord(FILE *fp, enum FIELD f, char *keyval)
{
    int record_cnt = 0; //���Ͽ� ����� ���ڵ� ��

    //�л� ����ü ���� �� �ʱ�ȭ
    STUDENT *s;
    s = (STUDENT *)malloc(sizeof(STUDENT));
    memset(s, 0, sizeof(STUDENT));

    fread(&record_cnt, sizeof(int), 1, fp); //���Ͽ� ����� �� ���ڵ� ���� ����
 
    
    //��� ���ڵ忡 ���� sequential search�� ����
    for(int rrn = 0; rrn < record_cnt; rrn++) {
        rewind(fp);
        int result = readRecord(fp, s, rrn);
        if(result == 1) {
            //������ ���ڵ��� �˻����� ����
            if(strchr(s->id, '*') != NULL)
                continue;
            //�˻����� ��ġ�ϴ� ���ڵ带 ã�� �����ϴ� ��� ���ڵ带 ���
            switch(f) {
                case ID:
                    if(!strcmp(keyval, s->id))
                        printRecord(s);
                    break;
                case NAME:
                    if(!strcmp(keyval, s->name))
                        printRecord(s);
                    break;
                case DEPT:
                    if(!strcmp(keyval, s->dept))
                        printRecord(s);
                    break;
                case ADDR:
                    if(!strcmp(keyval, s->addr))
                        printRecord(s);
                    break;
                case EMAIL:
                    if(!strcmp(keyval, s->email))
                        printRecord(s);
                    break;
            }
        }
        else {
            fprintf(stderr, "searchRecord error\n");
            exit(1);
        }
    }
}

int readRecord(FILE *fp, STUDENT *s, int rrn)
{
    //���Ͽ��� �־��� rrn ��ġ�� �ش��ϴ� ���ڵ带 �о� recordbuf�� ���� �� field�� ����
    char recordbuf[RECORD_SIZE];
    memset(recordbuf, 0, sizeof(recordbuf));

    fseek(fp, HEADER_SIZE + RECORD_SIZE * rrn, SEEK_SET);
    int result = fread(recordbuf, sizeof(recordbuf), 1, fp);
    unpack(recordbuf, s);
    
    return result;
}

int deleteRecord(FILE *fp, enum FIELD f, char *keyval)
{
    int record_cnt = 0; //���Ͽ� ����� �� ���ڵ� ��(header)
    int deleted_rrn = 0; //���� �ֱٿ� ������ ���ڵ� ��ȣ(header)
    int rrn;
    char deleteMark = '*';
    int d_list = -1;
    char header[HEADER_SIZE]; //���

    
    //�л� ����ü ���� �� �ʱ�ȭ
    STUDENT *s;
    s = (STUDENT *)malloc(sizeof(STUDENT));
    memset(s, 0, sizeof(STUDENT));

    
    //������ڵ� �б�
    fread(&record_cnt, sizeof(int), 1, fp);
    fread(&deleted_rrn, sizeof(int), 1, fp);
   
    //��� ���ڵ忡 ���� sequential search�� ����
    for(rrn = 0; rrn < record_cnt; rrn++) {
        rewind(fp);
        int result = readRecord(fp, s, rrn);
        if(result == 1) {
            //�˻����� ��ġ�ϴ� ���ڵ带 ã��
            if(strcmp(keyval, s->id) == 0)
                break;
        }
        else {
            fprintf(stderr, "searchRecord error\n");
            exit(1);
        }
    }
    

    if(rrn == record_cnt) //���� ��� ã�� ����
        return 0;
    
    else { //���� ��� ã�� ����

        //ã�� rrn ù��° ����Ʈ�� deleteMark�� ����
        fseek(fp, HEADER_SIZE + RECORD_SIZE * rrn, SEEK_SET);
        fwrite(&deleteMark, sizeof(char), 1, fp);
        //deleteMark�ڿ��� ���� �ֱٿ� ������ ���ڵ����� �˸��� -1�� ����
        fwrite(&d_list, sizeof(char), 1, fp);
        
        //���� ������ ���ڵ尡 �־��ٸ� �ش� ���ڵ��� '*-1' ���� '*rrn' �������� ������Ʈ�Ͽ� ���� ���ڵ� ����Ʈ ����
        if(deleted_rrn != -1) {
            fseek(fp, HEADER_SIZE + RECORD_SIZE * deleted_rrn + 1, SEEK_SET);
            fwrite(&rrn, sizeof(char), 1, fp);
        }
        
        //������ڵ� ��������� �������� ���·� ������ rrn ����
        rewind(fp);
        fseek(fp, sizeof(int), SEEK_SET);
        fwrite(&rrn, sizeof(int), 1, fp);
        
        return 1;
    }
}

int insertRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email)
{

}
