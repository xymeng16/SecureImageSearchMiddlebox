//
// Created by xymeng on 7/11/19.
//

#include "data_handler.h"
#include <exception>
#include <iostream>
#include <cstring>
#include <cassert>
#include <netinet/in.h>

using namespace std;

// pay attention to the endianness
// little-endian for host and big-endian for net
void unpack(byte *buf, int img_n, data_pack *data, int data_len)
{
    assert(buf != nullptr && data != nullptr);

    byte *buf_head = buf;

    // Allocate memory
    data->img_num = img_n;
    data->img_size = new SIZE[img_n];
//    data->a_hash = new byte[img_n][LSH_L][DM_LEN];
    data->a_hash = new SHA256[img_n];
    data->p_hash = new SHA256[img_n];
    data->d_hash = new SHA256[img_n];
    data->img_aes_len = new int[img_n];
    data->img_aes = new AES[img_n];


    // extract img_size
    for (int i = 0; i < img_n; i++)
    {
        data->img_size[i][0] = (*(buf + 1) & 0xFF) | (*buf & 0xFF) << 8;
        data->img_size[i][1] = (*(buf + 3) & 0xFF) | (*(buf + 2) & 0xFF) << 8;
        buf += 4;
    }

    // extract hash value
    memcpy(data->a_hash, buf, img_n * LSH_L * DM_LEN);
    buf += img_n * LSH_L * DM_LEN;
    memcpy(data->p_hash, buf, img_n * LSH_L * DM_LEN);
    buf += img_n * LSH_L * DM_LEN;
    memcpy(data->d_hash, buf, img_n * LSH_L * DM_LEN);
    buf += img_n * LSH_L * DM_LEN;

    // extract img_aes_len
    memcpy(data->img_aes_len, buf, sizeof(int) * img_n);
    for (int i = 0; i < img_n; i++)
    {
        data->img_aes_len[i] = ntohl(data->img_aes_len[i]);
    }
    buf += sizeof(int) * img_n;

    // extract img_aes
    for (int i = 0; i < img_n; i++)
    {
        data->img_aes[i] = new byte[data->img_aes_len[i]];
        memcpy(data->img_aes[i], buf, data->img_aes_len[i]);
        buf += data->img_aes_len[i];
    }

    // finished, check the buf cursor
    assert((buf - buf_head == data_len));
}
