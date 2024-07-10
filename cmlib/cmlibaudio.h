#ifndef _CM_LIB_AUDIO_
#define	_CM_LIB_AUDIO_


#include <pspimpose_driver.h>
#include <pspkernel.h>
#include <pspsdk.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/**
 * 現在の音量を取得する
 *
 * @return 現在の音量
 */
int libaudioGetVolume(void);

/**
 * 音量を設定する
 * 
 * @param value - 設定したい音量
 *
 * @return < 0 失敗
 */
int libaudioSetVolume(int value);

/**
 * ミュートを有効にする
 * 
 * @param status - それぞれのプラグイン内での有効/無効を保存するポインタ。
 * 		libaudioMuteOnとlibaudioMuteOff以外でこの値を変更するとおかしくなります。
 *
 * @return 0 成功, < 0 失敗（既に有効になっている）
 */
int libaudioMuteOn(bool *status);

/**
 * ミュートを無効にする
 * 
 * @param status - それぞれのプラグイン内での有効/無効を保存するポインタ。
 * 		libaudioMuteOnとlibaudioMuteOff以外でこの値を変更するとおかしくなります。
 *
 * @return 0 成功, < 0 失敗（既に無効になっている）
 */
int libaudioMuteOff(bool *status);

/**
 * 現在ミュートかどうか調べる
 *
 * @return 1 有効, 0 無効
 */
int libaudioMuteGetStatus(void);

/**
 * ミュートの有効/無効を再セットする
 * 他のプラグインもこの関数を使っている場合、
 * 一つでも有効にしているプラグインがあったらボタンの入力は無効化され、
 * すべてのプラグインが無効になっていたらボタンの入力は有効になります。
 *
 * @return true 有効, false 無効
 */
bool libaudioMuteSetAgain(void);

/**
 * 音の出力を有効にする
 * ミュートとは少し違います。
 * 
 * @param status - それぞれのプラグイン内での有効/無効を保存するポインタ。
 * 		libaudioIoEnableとlibaudioIoDisable以外でこの値を変更するとおかしくなります。
 *
 * @return 0 成功, < 0 失敗（既に有効になっている）
 */
int libaudioIoEnable(bool *status);

/**
 * 音の出力を無効にする
 * ミュートとは少し違います。
 * 
 * @param status - それぞれのプラグイン内での有効/無効を保存するポインタ。
 * 		libaudioIoEnableとlibaudioIoDisable以外でこの値を変更するとおかしくなります。
 *
 * @return 0 成功, < 0 失敗（既に無効になっている）
 */
int libaudioIoDisable(bool *status);

/**
 * 音の出力の有効/無効を調べる
 * 
 * @return true 有効, false 無効
 */
bool libaudioIoGetStatus(void);

/**
 * 音の出力の有効/無効を再セットする
 * 他のプラグインもこの関数を使っている場合、
 * 一つでも有効にしているプラグインがあったらボタンの入力は無効化され、
 * すべてのプラグインが無効になっていたらボタンの入力は有効になります。
 *
 * @return true 有効, false 無効
 */
bool libaudioIoSetAgain(void);


#endif