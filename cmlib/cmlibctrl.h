
#ifndef _CM_LIB_CTRL_
#define	_CM_LIB_CTRL_


#include <pspkernel.h>
#include <pspsdk.h>
#include <pspctrl.h>
#include <pspctrl_kernel.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>


#define LIB_CTRL_ALL (PSP_CTRL_SELECT | PSP_CTRL_START | PSP_CTRL_UP | PSP_CTRL_RIGHT | \
PSP_CTRL_DOWN | PSP_CTRL_LEFT | PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER | PSP_CTRL_TRIANGLE | \
PSP_CTRL_CIRCLE | PSP_CTRL_CROSS | PSP_CTRL_SQUARE | PSP_CTRL_NOTE | PSP_CTRL_HOME | \
PSP_CTRL_SCREEN | PSP_CTRL_VOLUP | PSP_CTRL_VOLDOWN )

/**
 * ユーザーモードで使えるボタンの入力をすべて無効にする
 * 他のプラグインもこの関数を使っている場合、
 * 一つでもオンにしているプラグインがあったらボタンの入力は無効化されます。
 * 
 * @param status - それぞれのプラグイン内でのオン/オフを保存するポインタ。
 * 		libctrlMaskAllButtonOnとlibctrlMaskAllButtonOff以外でこの値を変更するとおかしくなります。
 *
 * @return 0 成功, < 0 失敗（既にオンになっている）
 */
int libctrlMaskAllButtonOn(bool *status);

/**
 * ユーザーモードで使えるボタンの入力をすべて有効にする
 * 他のプラグインもこの関数を使っている場合、
 * すべてのプラグインがオフにしてないとボタンの入力が有効になりません。
 * 
 * @param status - それぞれのプラグイン内でのオン/オフを保存するポインタ。
 * 		libctrlMaskAllButtonOnとlibctrlMaskAllButtonOff以外でこの値を変更するとおかしくなります。
 *
 * @return 0 成功, < 0 失敗（既にオフになっている）
 */
int libctrlMaskAllButtonOff(bool *status);

/**
 * ボタンの入力がすべて有効かどうか調べる
 *
 * @return true 有効, false 無効
 */
bool libctrlMaskAllButtonStatus();

/**
 * ボタンの入力の有効/無効を再セットする
 * 他のプラグインもこの関数を使っている場合、
 * 一つでもオンにしているプラグインがあったらボタンの入力は無効化され、
 * すべてのプラグインがオフになっていたらボタンの入力は有効になります。
 *
 * @return true 有効, false 無効
 */
bool libctrlMaskAllButtonAgain();

/**
 * 指定したボタンの入力を無効にする（他のプラグインと競合する可能性があります）
 * 複数渡すときは「+」か「|」でつないで下さい。
 * 
 * @param PspCtrlButtons - 無効にしたいボタン
 *
 * @return 常に0
 */
int libctrlMaskButtonOn(int PspCtrlButtons);

/**
 * 指定したボタンの入力を有効にする（他のプラグインと競合する可能性があります）
 * 複数渡すときは「+」か「|」でつないで下さい。
 * 
 * @param PspCtrlButtons - 有効にしたいボタン
 *
 * @return 常に0
 */
int libctrlMaskButtonOff(int PspCtrlButtons);

/**
 * 指定したボタンの有効/無効を調べます
 * 引数に複数渡すとどうなるかは不明
 * 
 * @param PspCtrlButtons - 調べたいボタン
 *
 * @return true 有効, false 無効
 */
bool libctrlMaskButtonStatus(int PspCtrlButtons);

/**
 * 指定したボタンが長押しされているかどうかを調べます
 * 引数に複数渡すときは「+」か「|」でつないで下さい。
 * 
 * @param buttons - 調べたいボタン
 * @param count - 長押しの秒数
 *
 * @return 1 指定時間まで押した , 0 指定時間になる前に離した
 *
 */
int libctrlCountButtons(u32 buttons, int count);

/**
 * 指定したボタンが離されるまで待つ
 * 引数に複数渡すときは「+」か「|」でつないで下さい。
 * 
 * @param buttons - ボタン
 *
 * @return 0
 *
 */
int libctrlWaitButtonUp(u32 buttons);

/**
 * 指定したボタンが押されるまで待つ
 * 引数に複数渡すときは「+」か「|」でつないで下さい。
 * 
 * @param buttons - ボタン
 *
 * @return 0
 *
 */
int libctrlWaitButtonDown(u32 buttons);


#endif
