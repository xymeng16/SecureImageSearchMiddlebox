//
// Created by xymeng on 7/11/19.
//

#include <cstdlib>
#include <cstring>
#include "clustering.h"

int equ_dist(SHA256 a, SHA256 b, int lsh_l, int dm_l)
{
    int dist = 0;
    for (int l = 0; l < lsh_l; l++)
    {
        if (memcmp(a[l], b[l], dm_l))
        {
            dist++;
        }
    }

    return dist;
}

