/*
 
 INI Library Portable ver. 1.01   by hiroi01
 
 thanks to
 mp3play by plum 
 and
 bright.prx by plum
 and
 iso tool by takka
 
 */

#ifndef __INI_LIBRARY_PORTABLE_H__
#define	__INI_LIBRARY_PORTABLE_H__
#include <pspkernel.h>
#include <stdbool.h>

#define ILPISSPACE(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) =='\r' )

typedef enum {
	ILP_ERROR_OPEN          = -1,
	ILP_ERROR_READ          = -2,
	ILP_ERROR_WRITE         = -3,
} ILP_error_num;


typedef enum{
	ILP_TYPE_HEX = 0,
	ILP_TYPE_DEC,
	ILP_TYPE_BUTTON,
	ILP_TYPE_STRING,
	ILP_TYPE_LIST,
	ILP_TYPE_BOOL,
} ILP_type_num ;

/*
 このライブラリ内で各情報を管理するための構造体
 読み/書きする項目数分必要(最大32?)
*/
typedef struct ILP_Key_
{
	char *name;//左辺

	//読み/書きする値の格納される変数のアドレス
	void *value;//右辺値

	union{
		int int_;
		char *str_;
		u32 u32_;
		bool bool_;
	}defaultValue;//デフォルト値
	
	u32 type;//種類
	
	//拡張(種類によって使ったり使わなかったり)
	void *ex;
//	void **exx;

	int buf;//ライブラリ外からこの変数をいじらないように

	
} ILP_Key;

/*
 ILPで読み込むボタンの情報
 フラグ, 名前
 の順番に列挙していき最後は
 {0, "+"}
 のように{0, "区切り記号"}で終わる必要があります
 
 e.g)
 ILP_Button_Info button_info[] {
 {PSP_CTRL_LEFT,     "LEFT"},
 {PSP_CTRL_RTRIGGER, "RTRIGGER",},
 {PSP_CTRL_RTRIGGER, "R_TRI"},
 {PSP_CTRL_SELECT,   "SELECT"},
 {PSP_CTRL_START,    "START"},
 {0,"+"}
 };
 
 
 */

typedef struct ILP_Button_Info_
{
	u32 flag;
	char *name;
}ILP_Button_Info;

/*
 ILP_Key構造体の初期化
 このライブラリの他のどの処理をするよりも前に初期化をする必要があります
 @param : key
 ILP_Key 構造体のアドレスを渡して下さい
 
 @return :
           == 0 : no problem
 */
int ILPInitKey(ILP_Key *key);

/*
 すべての項目にデフォルト値をセットします
 
 @param : key
 ILP_Key 構造体のアドレスを渡して下さい
 */
void ILPSetDefault(ILP_Key *key);

/*
 ILP_Key構造体に各項目の情報を登録します
 
 
 --共通--
 @param : key
 ILP_Key 構造体のアドレスを渡して下さい
 @param : keyName
 項目名(左辺)を渡してください
 @param : value
 値(右辺)を入れるための変数のアドレスを渡してください
 @param : defaultValue
 デフォルト値を渡してください
 
 --ILPRegisterList--
 @param : list
 char *list = { "one" "two" "three" NULL };
 となるような配列のアドレスを渡してください。
 その配列のスコープはセットとなるILP_Key構造体と同じであることが望ましいです。
 
---ILPRegisterButton--
 @param : buttonInfo
 特別に使用するボタン情報を指定する場合は、ILP_Button_Info構造体のアドレスを渡してください
 (lite版ではない場合)特に指定しない場合はNULLを渡すことで、このライブラリのデフォルトのボタン情報を使用します
 
 */
void ILPRegisterHex(ILP_Key *key, char *keyName, u32 *value, u32 defaultValue);
void ILPRegisterDec(ILP_Key *key, char *keyName, int *value, int defaultValue);
void ILPRegisterButton(ILP_Key *key, char *keyName,  u32 *value, u32 defaultValue, ILP_Button_Info *buttonInfo);
void ILPRegisterString(ILP_Key *key, char *keyName, char *value, char *defaultValue);
void ILPRegisterList(ILP_Key *key, char *keyName, int *value, int defaultValue, char *list[] );
void ILPRegisterBool(ILP_Key *key, char *keyName, bool *value, bool defaultValue);


/*
 INI_Key構造体に登録された情報に基づいて、ファイルから設定を読み込みます

 @param : key
 ILP_Key構造を渡して下さい
 @param : path
 読み込むファイルのパスを渡して下さい
 @return : 
 == 0 : no problem
 != 0 : on error(ILP_error_numの値を返します)
*/
int ILPReadFromFile(ILP_Key *key, const char *path);

/*
 INI_Key構造体に登録された情報に基づいて、ファイルへ書き込みます。
 もとのファイルに上書きする事も可能で、その場合はコメントや項目の順番を保持して上書きします。
 
 @param : key
 ILP_Key構造を渡して下さい
 @param : path
 書き込むファイルのパスを渡して下さい
 @param : lineFeedCode
 ファイルを書き込む際にに使用する改行コードを指定して下さい
 CR+LF -> "\r\n"
 CR    -> "\r"
 LF    -> "\n"

 @return : 
 == 0 : no problem
 != 0 : on error(ILP_error_numの値を返します)
*/
int ILPWriteToFile(ILP_Key *key, const char *path, const char *lineFeedCode);

/*
渡されたボタンの名称から、フラグを取得します
@param : name
ボタンの名称を渡してください。
例) "HOME"
複数のボタンを指定する事はできません。
なので、
"HOME + CROSS"
このように指定しても正しく処理されません。

@param : info
解析する元の情報として使用するILP_Button_Info構造体のアドレスを渡してください
(lite版ではない場合)特に指定しない場合はNULLを渡すことで、このライブラリのデフォルトのボタン情報を使用します

@return :
nameよりinfoを元に解析して得られたボタンのフラグを返します。
解析に失敗した場合は0を返します。

 
 ILPGetButtonFlagByName("HOME", NULL)とするとPSP_CTRL_HOMEと同じ値が返ってきます
*/
u32 ILPGetButtonFlagByName(const char* name, ILP_Button_Info *info);


/*
 
 
渡されたボタンのフラグから、そのボタンの名称を返します
 
@param : rtn
ボタンの名称を返すための変数のアドレスを渡してください
@param : flag
ボタンのフラグを渡してください
@param : info
解析する元の情報として使用するILP_Button_Info構造体のアドレスを渡してください
(lite版ではない場合)特に指定しない場合はNULLを渡すことで、このライブラリのデフォルトのボタン情報を使用します
@return :
rtnに渡されたアドレスを返します
 エラーの場合はNULLを返します
 
ILPCatButtonNameByFlagの場合はrtnの文字列の後ろにボタンの名称を結合します
ILPGetButtonNameByFlagの場合はrtnに名称を返します
 
 
 char str[128];
 ILPGetButtonNameByFlag(str, PSP_CTRL_HOME | PSP_CTRL_CROSS, NULL, " + ");
 とすると、printf("%s", str);の出力結果は"CROSS + HOME"となります
*/
char* ILPGetButtonNameByFlag(char *rtn, u32 flag, ILP_Button_Info *info);

/*
 渡された(Buttonとして登録されている) ILP_Key構造体からそのボタンの名称を返します。
 
 @param : rtn
 ボタンの名称を返すための変数のアドレスを渡してください。ここで渡された
 @param : key
 (Buttonとして登録されている) ILP_Key構造体のアドレスを渡してください
 @return :
 rtnに渡されたアドレスを返します
 エラーの場合はNULLを返します

*/
char* ILPGetButtonNameByKey(char *rtn, ILP_Key *key);


/*
 keynumberで指定した情報を所得します。
 (keynumber番目にRegisterで登録した情報をとりだす。keynumberは0から数える。)
 
 */

char *ILPGetNameAddressByKeynumber(ILP_Key *key, int keynumber);
void *ILPGetValueAddressByKeynumber(ILP_Key *key, int keynumber);
char *ILPGetStringFormatValueByKeynumber(ILP_Key *key, char *rtn, int keynumber);


/*
 これらは基本的にこのライブラリの内部で使用するものです
ファイルから読み込んだ右辺部分を渡すことで、それを値としてgetする関数


---common---
@param : rtn
渡された右辺部分をもとにgetした値を返却するための返却のアドレスを渡してください
@param :  str
右辺部分の文字列(改行や余分なスペースを除いたもの)を渡してください
@param : defaultValue
右辺部分を解析した結果、正しい値出なかった場合にrtnに返すデフォルト値を渡してください

---ILPGetButton---
@param : info
解析する元の情報として使用するILP_Button_Info構造体のアドレスを渡してください
(lite版ではない場合)特に指定しない場合はNULLを渡すことで、このライブラリのデフォルトのボタン情報を使用します
---ILPGetNumberFromList---
@param : list
使用するリストの配列のアドレスを渡してください。
リストの形式はILPRegisterListと同じです。

*/
void ILPGetButton(u32 *rtn, const char *str, u32 defaultValue, ILP_Button_Info *info);
void ILPGetString(char *rtn, const char *str , const char *defaultValue);
void ILPGetNumberFromList(int *rtn, const char *str, int defaultNum, char **list );
void ILPGetHex(u32 *rtn, const char *str, u32 defaultNum);
void ILPGetDec(int *rtn, const char *str ,int defaultNum);
void ILPGetBool(bool *rtn, const char *str, bool defaultValue);

/*
1行(1項目)を書き込みます
@param : fd
書き込むファイルのfile descriptor
@param : key
書き込む項目のILP_Key構造体の先頭アドレス
@param : lineFeedCode
改行コード(詳しくはILPWriteToFile参照)

@return :
書き込んだバイト数
*/
int ILPWriteLine(SceUID fd, ILP_Key *key, const char *lineFeedCode);





//---utils---

// strtok関数の代わり(NULL指定不可)
// delimで指定した文字列をstrから探して分断する
char *ILPChToken(char *str, const char *delim);
int ILPIsSkipLine(const char *str);
//base is in iso tool
//thanks to takka
int ILPReadLine(SceUID fp, char* line, int maxline);
//わたされた文字列のスペースおよび改行コードを取り除く
int ILPRemoveSpaceAndNewline(char *str);





#endif

