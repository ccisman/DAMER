#pragma once
/*�ʷ�������*/
#include<iostream>
#include<string>
#include <gperftools/tcmalloc.h>

using namespace std;
#define ERROR 0
#define OK 1
#define $Neg 1     //! ��
#define $Impli 2   //-> �̺�
#define $Equ 3    //<-> �ȼ�
#define $And 4   //&& ��ȡ
#define $Or 5     //|| ��ȡ
#define $Alw 6    // [] G
#define $Eve 7    // <> F
#define $Until 8   // U
#define $Rele 9   // R
#define $Next 10  // X
#define $Lpar 11   // (
#define $Rpar 12  // )
#define $ID 13      //ԭ������
#define $End 14
#define $root 15
#define MAXFORMLEN 200000

extern bool STUBBORN;

typedef struct Lexer_word
{
	string character;
	int typenum;
}word;

class Lexer    //�ʷ�������
{
private:
	char formula[MAXFORMLEN];
	int pointer;
	int length;
public:
	Lexer(char *form, int len);
	int GetWord(word &w);
};
