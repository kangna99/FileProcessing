#include <stdio.h>		// 필요한 header file 추가 가능
#include <stdlib.h>
#include <string.h>
#include "student.h"

//
// 함수 readRecord()는 학생 레코드 파일에서 주어진 rrn에 해당하는 레코드를 읽어서 
// recordbuf에 저장하고, 이후 unpack() 함수를 호출하여 학생 타입의 변수에 레코드의
// 각 필드값을 저장한다. 성공하면 1을 그렇지 않으면 0을 리턴한다.
// unpack() 함수는 recordbuf에 저장되어 있는 record에서 각 field를 추출하는 일을 한다.
//
int readRecord(FILE *fp, STUDENT *s, int rrn);
void unpack(const char *recordbuf, STUDENT *s);

//
// 함수 writeRecord()는 학생 레코드 파일에 주어진 rrn에 해당하는 위치에 recordbuf에 
// 저장되어 있는 레코드를 저장한다. 이전에 pack() 함수를 호출하여 recordbuf에 데이터를 채워 넣는다.
// 성공적으로 수행하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
//
int writeRecord(FILE *fp, const STUDENT *s, int rrn);
void pack(char *recordbuf, const STUDENT *s);

//
// 함수 appendRecord()는 학생 레코드 파일에 새로운 레코드를 append한다.
// 레코드 파일에 레코드가 하나도 존재하지 않는 경우 (첫 번째 append)는 header 레코드를
// 파일에 생성하고 첫 번째 레코드를 저장한다. 
// 당연히 레코드를 append를 할 때마다 header 레코드에 대한 수정이 뒤따라야 한다.
// 함수 appendRecord()는 내부적으로 writeRecord() 함수를 호출하여 레코드 저장을 해결한다.
// 성공적으로 수행하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
//
int appendRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email);

//
// 학생 레코드 파일에서 검색 키값을 만족하는 레코드가 존재하는지를 sequential search 기법을 
// 통해 찾아내고, 이를 만족하는 모든 레코드의 내용을 출력한다. 검색 키는 학생 레코드를 구성하는
// 어떤 필드도 가능하다. 내부적으로 readRecord() 함수를 호출하여 sequential search를 수행한다.
// 검색 결과를 출력할 때 반드시 printRecord() 함수를 사용한다. (반드시 지켜야 하며, 그렇지
// 않는 경우 채점 프로그램에서 자동적으로 틀린 것으로 인식함)
//
void searchRecord(FILE *fp, enum FIELD f, char *keyval);
void printRecord(const STUDENT *s);

//
// 레코드의 필드명을 enum FIELD 타입의 값으로 변환시켜 준다.
// 예를 들면, 사용자가 수행한 명령어의 인자로 "NAME"이라는 필드명을 사용하였다면 
// 프로그램 내부에서 이를 NAME(=1)으로 변환할 필요성이 있으며, 이때 이 함수를 이용한다.
//
enum FIELD getFieldID(char *fieldname);

//
// 학생 레코드 파일에서 "학번" 키값을 만족하는 레코드를 찾아서 이것을 삭제한다.
// 참고로, 검색 조건은 반드시 학번(ID)만을 사용한다. 레코드의 검색은 searchRecord() 함수와
// 유사한 방식으로 구현한다. 성공적으로 수행하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
//
int deleteRecord(FILE *fp, enum FIELD f, char *keyval);

//
// 학생 레코드 파일에 새로운 레코드를 추가한다. 과제 설명서에서 언급한 대로, 삭제 레코드가
// 존재하면 반드시 삭제 레코드들 중 하나에 새로운 레코드를 저장한다. 삭제 레코드 리스트 관리와
// 삭제 레코드의 선택은 교재 방식을 따른다. 새로운 레코드의 추가는 appendRecord() 함수와 비슷한
// 방식으로 구현한다. 성공적으로 수행하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
//
int insertRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email);

void main(int argc, char *argv[])
{
	FILE *fp;			// 모든 file processing operation은 C library를 사용할 것
    char *token;
    char field[10];
    char value[MAX_VALUE + 1];
    memset(field, 0, sizeof(field));
    memset(value, 0, sizeof(value));

    //레코드 파일이 있으면 열고 없으면 새로운 파일 생성
    if((fp = fopen(argv[2], "r+")) == NULL) {
        fp = fopen(argv[2], "w+");
    }

    //기능1) 레코드 추가 -a 옵션
    if(!strcmp(argv[1], "-a")) {

        //예외처리
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

        //입력받은 값으로 레코드 추가
        if((appendRecord(fp, argv[3], argv[4], argv[5], argv[6], argv[7])) == 0) {
            fprintf(stderr, "appendRecord fail\n");
            exit(1);
        }
    }

    //기능2) 레코드 검색 -s 옵션
    if(!strcmp(argv[1], "-s")) {

        //예외처리
        if(argc != 4) {
            fprintf(stderr, "input error\nusage: %s -s <record_file_name> \"field_name = field_value\"\n", argv[0]);
            exit(1);
        }
        //검색할 입력값을 필드와 값으로 split
        token = strtok(argv[3], "=");
        strcpy(field, token);
        token = strtok(NULL, "=");
        strcpy(value, token);

        searchRecord(fp, getFieldID(field), value);
    }

    //기능3) 레코드 삭제 -d 옵션
    if(!strcmp(argv[1], "-d")) {

        //예외처리
        if(argc != 4) {
            fprintf(stderr, "input error\nusage: %s -d <record_file_name> \"ID = field_value\"\n", argv[0]);
            exit(1);
        }
        //삭제할 입력값을 필드와 값으로 split
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
    int record_cnt = 0; //파일에 저장된 레코드 수
    int new_record_cnt;
    int rrn = 0;
    int result; //writeRecord 수행 반환값
    char header[HEADER_SIZE]; //헤더
    memset(header, 0, sizeof(int)); //전체레코드수 초기화
    memset(header+sizeof(int), -1, sizeof(int)); //삭제레코드 헤더 초기화

    //학생 구조체 생성 및 초기화
    STUDENT *s;
    s = (STUDENT *)malloc(sizeof(STUDENT));
    memset(s, 0, sizeof(STUDENT));
    strcpy(s->id, id);
    strcpy(s->name, name);
    strcpy(s->dept, dept);
    strcpy(s->addr, addr);
    strcpy(s->email, email);

    //처음 쓸 땐 header 레코드를 파일에 생성하고 rrn=0, 이후엔 rrn을 헤더 기반으로 찾음 
    if(fread(&record_cnt, sizeof(int), 1, fp) == 0) {
        fwrite(header, sizeof(header), 1, fp);
    }
    else {
        rrn = record_cnt;
    }
    //rrn에 record 저장하고 header 수정
    if((result = writeRecord(fp, s, rrn)) == 1) { //정상적으로 수행
        rewind(fp);
        new_record_cnt = record_cnt + 1;
        fwrite(&new_record_cnt, sizeof(int), 1, fp);
    }
    return result;
}

int writeRecord(FILE *fp, const STUDENT *s, int rrn)
{
    //주어진 rrn 위치에 recordbuf에 저장된 레코드를 저장
    char recordbuf[RECORD_SIZE];
    memset(recordbuf, 0, sizeof(recordbuf));
    pack(recordbuf, s);
    fseek(fp, HEADER_SIZE + RECORD_SIZE * rrn, SEEK_SET);

    //성공적으로 수행하면 1을 리턴, 실패 시 0을 리턴
    if((fwrite(recordbuf, sizeof(recordbuf), 1, fp)) == 1)
        return 1;
    else return 0;
}

void searchRecord(FILE *fp, enum FIELD f, char *keyval)
{
    int record_cnt = 0; //파일에 저장된 레코드 수

    //학생 구조체 생성 및 초기화
    STUDENT *s;
    s = (STUDENT *)malloc(sizeof(STUDENT));
    memset(s, 0, sizeof(STUDENT));

    fread(&record_cnt, sizeof(int), 1, fp); //파일에 저장된 총 레코드 수를 읽음
 
    
    //모든 레코드에 대해 sequential search를 수행
    for(int rrn = 0; rrn < record_cnt; rrn++) {
        rewind(fp);
        int result = readRecord(fp, s, rrn);
        if(result == 1) {
            //삭제된 레코드라면 검색에서 제외
            if(strchr(s->id, '*') != NULL)
                continue;
            //검색값이 일치하는 레코드를 찾아 만족하는 모든 레코드를 출력
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
    //파일에서 주어진 rrn 위치에 해당하는 레코드를 읽어 recordbuf에 저장 후 field별 구분
    char recordbuf[RECORD_SIZE];
    memset(recordbuf, 0, sizeof(recordbuf));

    fseek(fp, HEADER_SIZE + RECORD_SIZE * rrn, SEEK_SET);
    int result = fread(recordbuf, sizeof(recordbuf), 1, fp);
    unpack(recordbuf, s);
    
    return result;
}

int deleteRecord(FILE *fp, enum FIELD f, char *keyval)
{
    int record_cnt = 0; //파일에 저장된 총 레코드 수(header)
    int deleted_rrn = 0; //가장 최근에 삭제된 레코드 번호(header)
    int rrn;
    char deleteMark = '*';
    int d_list = -1;
    char header[HEADER_SIZE]; //헤더

    
    //학생 구조체 생성 및 초기화
    STUDENT *s;
    s = (STUDENT *)malloc(sizeof(STUDENT));
    memset(s, 0, sizeof(STUDENT));

    
    //헤더레코드 읽기
    fread(&record_cnt, sizeof(int), 1, fp);
    fread(&deleted_rrn, sizeof(int), 1, fp);
   
    //모든 레코드에 대해 sequential search를 수행
    for(rrn = 0; rrn < record_cnt; rrn++) {
        rewind(fp);
        int result = readRecord(fp, s, rrn);
        if(result == 1) {
            //검색값이 일치하는 레코드를 찾음
            if(strcmp(keyval, s->id) == 0)
                break;
        }
        else {
            fprintf(stderr, "searchRecord error\n");
            exit(1);
        }
    }
    

    if(rrn == record_cnt) //삭제 대상 찾기 실패
        return 0;
    
    else { //삭제 대상 찾기 성공

        //찾은 rrn 첫번째 바이트에 deleteMark를 저장
        fseek(fp, HEADER_SIZE + RECORD_SIZE * rrn, SEEK_SET);
        fwrite(&deleteMark, sizeof(char), 1, fp);
        //deleteMark뒤에는 가장 최근에 삭제된 레코드임을 알리는 -1을 저장
        fwrite(&d_list, sizeof(char), 1, fp);
        
        //전에 삭제된 레코드가 있었다면 해당 레코드의 '*-1' 값을 '*rrn' 형식으로 업데이트하여 삭제 레코드 리스트 관리
        if(deleted_rrn != -1) {
            fseek(fp, HEADER_SIZE + RECORD_SIZE * deleted_rrn + 1, SEEK_SET);
            fwrite(&rrn, sizeof(char), 1, fp);
        }
        
        //헤더레코드 예약공간에 이진정수 형태로 삭제한 rrn 저장
        rewind(fp);
        fseek(fp, sizeof(int), SEEK_SET);
        fwrite(&rrn, sizeof(int), 1, fp);
        
        return 1;
    }
}

int insertRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email)
{

}
