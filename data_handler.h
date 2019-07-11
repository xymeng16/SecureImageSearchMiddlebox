//
// Created by xymeng on 7/11/19.
//

#ifndef SECUREIMAGESEARCHMIDDLEBOX_DATA_HANDLER_H
#define SECUREIMAGESEARCHMIDDLEBOX_DATA_HANDLER_H

const int LSH_L = 10, DM_LEN = 32;

typedef char byte;
typedef short SIZE[2];
typedef byte SHA256[LSH_L][DM_LEN];
typedef byte* AES;

// Data format
// data_length:int | img_num:int | img_w:short,img_h:short ... * img_num | APDHashSHA256Value:byte[3][img_num][LSH_L][32] |
// imgAESLength:int ... * img_num | imageAES:byte[img_num][padded_img_size](variant)
struct data_pack
{
    int img_num;
    SIZE *img_size; // pointers of 1d-array containing 2 elements
    SHA256 *a_hash, *p_hash, *d_hash;
    int *img_aes_len;
    AES *img_aes;
};

void unpack(byte *buf, int img_n, data_pack *data, int data_len);
#endif //SECUREIMAGESEARCHMIDDLEBOX_DATA_HANDLER_H
