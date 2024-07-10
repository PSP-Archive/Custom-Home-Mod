#ifndef STRUCT_H
#define STRUCT_H

typedef struct String_
{
	char *date;
	char *week[7];
	char *month[12];
	char *lang[10];
	char *message[6];
} String;

#define VERSION 1

typedef struct CONFIG_
{
	char version;
	struct {
		int true_num;
		int menu_num;
		int bar_num;
		int x_addr;
		bool right;
		bool view;
	} item[9];
	struct {
		int true_num;
		int menu_num;
		bool view;
	}menu[8];
	struct {
		u32 main_b;
		u32 main_f;
		u32 nom_b1;
		u32 nom_b2;
		u32 nom_f1;
		u32 nom_f2;
		u32 act_b1;
		u32 act_f1;
		bool twin;
	}color;
	int menu_len;
	int menu_pos;
	int menu_itm;
	bool bar[4];
	bool swap;
	bool mute;
	bool stop;
} CONFIG;

#endif