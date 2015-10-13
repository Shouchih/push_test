#include "stdafx.h"
#include "bgt_motionfile.h"
#include "../polarssl/sha1.h"


int bgt_motion_file_check(struct bgt_motion_file_header *hdr, int filesize) {
    unsigned char *buf;
    unsigned char sha1res[20];
    int len;

    if (memcmp(hdr->magic, BGT_MOTION_FILE_MAGIC, 10) != 0)
        return 2;

    buf = (unsigned char *)hdr + 30; // skip magic number & sha1 hash
    len = sizeof(struct bgt_motion_file_header) - 30 + hdr->motion_num * sizeof(struct bgt_frame);

    sha1(buf, len, sha1res);

    if (memcmp(sha1res, hdr->sha1, 20) != 0)
        return 1;

    if ((int)(sizeof(struct bgt_motion_file_header) + hdr->motion_num * sizeof(struct bgt_frame)) != filesize)
        return 2;

    return 0;
}

int bgt_motion_file_hash(struct bgt_motion_file_header *hdr, int filesize) {
    unsigned char *buf;
    int len;

    if (memcmp(hdr->magic, BGT_MOTION_FILE_MAGIC, 10) != 0)
        return 2;
    if ((int)(sizeof(struct bgt_motion_file_header) + hdr->motion_num * sizeof(struct bgt_frame)) != filesize)
       return 2;

    buf = (unsigned char *)hdr + 30; // skip magic number & sha1 hash
    len = sizeof(struct bgt_motion_file_header) - 30 + hdr->motion_num * sizeof(struct bgt_frame);

    sha1(buf, len, hdr->sha1);

    return 0;
}


int bgt_cue_file_check(struct bgt_cue_file_header *hdr,int bytePerFrame) {
    unsigned char *buf;
    unsigned char sha1res[20];
    int len;

    if (memcmp(hdr->magic, BGT_MOTION_FILE_MAGIC, 10) != 0)
        return 2;

    buf = (unsigned char *)hdr + 30;
    len = sizeof(struct bgt_cue_file_header) - 30 + hdr->frame_num * bytePerFrame;

    sha1(buf, len, sha1res);

    if (memcmp(sha1res, hdr->sha1, 20) != 0)
        return 1;

    return 0;
}

int bgt_cue_file_hash(struct bgt_cue_file_header *hdr,int bytePerFrame) {
    unsigned char *buf;
    int len;

    if (memcmp(hdr->magic, BGT_MOTION_FILE_MAGIC, 10) != 0)
        return 2;

    buf = (unsigned char *)hdr + 30;
    len = sizeof(struct bgt_cue_file_header) - 30 + hdr->frame_num * bytePerFrame;


    sha1(buf, len, hdr->sha1);

    return 0;
}