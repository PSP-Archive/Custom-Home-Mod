#ifndef CUSHOME_H
#define CUSHOME_H

/*
 CustomHOMEの開閉状態を取得
 
 @return :
			0 : Close
			1 : Open
*/
int cusHomeGetStatus();

/*
 CustomHOMEのメニューを閉じます

 @return :
			0 : success
			-1 :Not open
*/
int cusHomeClose();


#endif