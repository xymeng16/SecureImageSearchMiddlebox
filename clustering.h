//
// Created by xymeng on 7/11/19.
//

#ifndef SECUREIMAGESEARCHMIDDLEBOX_CLUSTERING_H
#define SECUREIMAGESEARCHMIDDLEBOX_CLUSTERING_H

#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "data_handler.h"

#define NOT_USED  0 /* node is currently not used */
#define LEAF_NODE 1 /* node contains a leaf node */
#define A_MERGER  2 /* node contains a merged pair of root clusters */
#define MAX_LABEL_LEN 16

#define AVERAGE_LINKAGE  'a' /* choose average distance */
#define CENTROID_LINKAGE 't' /* choose distance between cluster centroids */
#define COMPLETE_LINKAGE 'c' /* choose maximum distance */
#define SINGLE_LINKAGE   's' /* choose minimum distance */

typedef struct coord
{
    float x, y;
};

struct neighbour {
    int target; /* the index of cluster node representing neighbour */
    float distance; /* distance between the nodes */
    neighbour *next, *prev; /* linked list entries */
};

struct cluster_node {
    int type; /* type of the cluster node */
    int is_root; /* true if cluster hasn't merged with another */
    int height; /* height of node from the bottom */
    coord centroid; /* centroid of this cluster */
    char *label; /* label of a leaf node */
    int *merged; /* indexes of root clusters merged */
    int num_items; /* number of leaf nodes inside new cluster */
    int *items; /* array of leaf nodes indices inside merged clusters */
    neighbour *neighbours; /* sorted linked list of distances to roots */
};

struct cluster_s {
    int num_items; /* number of items that was clustered */
    int num_clusters; /* current number of root clusters */
    int num_nodes; /* number of leaf and merged clusters */
    cluster_node *nodes; /* leaf and merged clusters */
    float **distances; /* distance between leaves */
};

struct item {
    coord coord; /* coordinate of the input data point */
    char label[MAX_LABEL_LEN]; /* label of the input data point */
};

template<typename T>
T **get_dist_mat(SHA256 *data, int n, T (*dist)(SHA256, SHA256, int, int))
{
    T **dist_mat = new T *[n];

    for (int i = 0; i < n; i++)
    {
        dist_mat[i] = new T[n];

        for (int j = 0; j < n; j++)
        {
            dist_mat[i][j] = (*dist)(data[i], data[j], LSH_L, DM_LEN);
        }
    }

    return dist_mat;
}

int equ_dist(SHA256 a, SHA256 b, int l, int dm_l);

#endif //SECUREIMAGESEARCHMIDDLEBOX_CLUSTERING_H
