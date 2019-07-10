//
// Created by xymeng on 7/11/19.
//

#ifndef SECUREIMAGESEARCHMIDDLEBOX_DATA_HANDLER_H
#define SECUREIMAGESEARCHMIDDLEBOX_DATA_HANDLER_H
typedef char byte;
const int LSH_L = 10, DM_LEN = 32;
// Data format
// data_length:int | img_num:int | img_w:short,img_h:short ... * img_num | APDHashSHA256Value:byte[3][img_num][LSH_L][32] |
// imgAESLength:int ... * img_num | imageAES:byte[img_num][padded_img_size](variant)
struct data_pack
{
    int img_num;
    short (*img_size)[2]; // pointers of 1d-array containing 2 elements
    byte (*a_hash)[LSH_L][DM_LEN], (*p_hash)[LSH_L][DM_LEN], (*d_hash)[LSH_L][DM_LEN];
    int *img_aes_len;
    byte **img_aes;
};

void unpack(byte *buf, int img_n, data_pack *data, int data_len);
#endif //SECUREIMAGESEARCHMIDDLEBOX_DATA_HANDLER_H
