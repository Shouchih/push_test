#ifndef BGT_MOTION_FILE_H
#define BGT_MOTION_FILE_H

#include <stdint.h>

#define BGT_MOTION_FILE_MAGIC   ("@Brogent00")

// version 2

struct bgt_frame {
    unsigned short axis[6];
    unsigned char  sfx[4];
};

struct bgt_motion_file_header {
    char magic[10];
    unsigned char sha1[20];
    uint16_t file_ver;
    uint16_t type;
    unsigned char motion_name[32];  // end with \0
    uint16_t motion_ver;
    uint16_t motion_num;
    uint16_t mps;                           // mps * 100, eg. 59.97 mps = 5997
    char video_name[256];           // end with \0
	char project_id[32];
	uint16_t effect[4];                     // 0:no-use, 6:water,7:scent, 8:wind,9:fog,10:snow,11:leg_tickle
    char updateDate[16];              // date to save , yyyy-mm-dd hh:mm
	char guid[36];                           //  guid code, same as origent source data
	char sysUUID[37];                   //  mother board UUID + '\0'  
	char expireDate[11];               //  date to save , yyyy-mm-dd  + '\0'  
    unsigned char reserved[44];    // must be \0
};

struct bgt_cue_file_header{
    char magic[10];
    unsigned char sha1[20];
    uint16_t header_ver;
	uint16_t frame_num;
    uint16_t fps;									 // fps * 100, eg. 59.97 fps = 5997
    uint16_t motion_exist;
	uint16_t effect_channel_num;
    unsigned char file_desc[32];		// end with \0
    char video_name[256];					// end with \0
	char project_id[32];						// end with \0
	char updateDate[16];					// date to save
	char guid[36];									//  guid code, same as origent source data
	char sysUUID[37];                         //  mother board UUID + '\0'  
	char expireDate[11];                     //  date to save  , yyyy-mm-dd  + '\0'  
    unsigned char reserved[52];        // must be \0
 };

// 0: success
// 1: checksum error
// 2: file format error
//

int bgt_motion_file_check(struct bgt_motion_file_header *hdr, int filesize);
int bgt_motion_file_hash(struct bgt_motion_file_header *hdr, int filesize);

int bgt_cue_file_check(struct bgt_cue_file_header *hdr,int bytePerFrame);
int bgt_cue_file_hash(struct bgt_cue_file_header *hdr,int bytePerFrame) ;

#endif // BGT_MOTION_FILE_H

