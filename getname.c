
#include "getname.h"
#include "cmlib/cmlibconv.h"


//EBOOT.PBP or PARAM.SFOからゲームタイトルを取得するもの
//Thanks hiroi01!!

unsigned char ebootpbpSignature[] = { 0x00, 0x50, 0x42, 0x50 };
unsigned char paramsfoMagic[] = { 0x00, 0x50, 0x53, 0x46 };

typedef struct ebootpbp_header_{
	char signature[4];
	int version;
	int offset[8];
}ebootpbp_header;

typedef struct psf_header_{
	char magic[4];
	char rfu000[4];
	int label_ptr;
	int data_ptr;
	int nsects;
}psf_header;

typedef struct psf_section_{
	short label_off;
	char rfu001;
	char data_type;
	int datafield_used;
	int datafield_size;
	int data_off;
}psf_section;


int readGameParam(const char *path, char *rtn, int rtnSize, char *labelType)
{
	int fd = sceIoOpen(path, PSP_O_RDONLY, 0777);
	ebootpbp_header pbpHeader;
	psf_header header;
	psf_section section;
	int i;
	off_t currentPosition;
	off_t offset = 0;
	char label[16];
	
	if(fd < 0)return -1;

	offset = lseek(fd, 0, SEEK_CUR);
	read(fd, &pbpHeader, sizeof(ebootpbp_header));
	if( memcmp(&pbpHeader.signature, ebootpbpSignature, sizeof(ebootpbpSignature)) == 0){
		offset += pbpHeader.offset[0];
	}

	lseek(fd, offset, SEEK_SET);
	read(fd, &header, sizeof(psf_header));
	if( memcmp(&header.magic, paramsfoMagic, sizeof(paramsfoMagic)) != 0){
		return -1;
	}

	currentPosition = lseek(fd, 0 ,SEEK_CUR);
	for( i = 0; i < header.nsects; i++ ){
		lseek(fd, currentPosition, SEEK_SET);
		read(fd, &section, sizeof(psf_section));
		currentPosition = lseek(fd, 0 ,SEEK_CUR);
		
		lseek(fd, offset + header.label_ptr + section.label_off, SEEK_SET);
		read(fd, label, sizeof(label)-1);
		label[sizeof(label)-1] = '\0';
	
		if(strcmp(label, labelType) == 0){
			lseek(fd, offset + header.data_ptr + section.data_off, SEEK_SET);
			read(fd, rtn, rtnSize - 1);
			rtn[rtnSize - 1] = '\0';
			close(fd);
			return 0;
		}
	}

	//not found labelType section
	return 1;
}

int GetGameTitle(char *title, const char *path)
{
	int rtn;
	char buf[256];
	
	rtn = readGameParam(path, buf, 256, "TITLE");
	if(rtn < 0) return -1;
	libConvUtf8_to_Sjis(title, buf);
	libConvFreeMem();
	
	return rtn;
}
