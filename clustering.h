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
#include <iostream>
#include "data_handler.h"

using namespace std;
#define NOT_USED  0 /* node is currently not used */
#define LEAF_NODE 1 /* node contains a leaf node */
#define A_MERGER  2 /* node contains a merged pair of root clusters */
#define MAX_LABEL_LEN 16

#define AVERAGE_LINKAGE  'a' /* choose average distance */
#define CENTROID_LINKAGE 't' /* choose distance between cluster centroids */
#define COMPLETE_LINKAGE 'c' /* choose maximum distance */
#define SINGLE_LINKAGE   's' /* choose minimum distance */

struct Coord
{
    float x, y;
};
template<typename T>
struct Neighbor
{
    int target; /* the index of cluster node representing neighbour */
    T distance; /* distance between the nodes */
    Neighbor<T> *next, *prev; /* linked list entries */
};
template<typename T>
struct ClusterNode
{
    int type; /* type of the cluster node */
    int is_root; /* true if cluster hasn't merged with another */
    int height; /* height of node from the bottom */
    Coord centroid; /* centroid of this cluster */
    char *label; /* label of a leaf node */
    int *merged; /* indexes of root clusters merged */
    int num_items; /* number of leaf nodes inside new cluster */
    int *items; /* array of leaf nodes indices inside merged clusters */
    Neighbor<T> *neighbours; /* sorted linked list of distances to roots */
};
template<typename T>
struct Cluster
{
    int num_items; /* number of items that was clustered */
    int num_clusters; /* current number of root clusters */
    int num_nodes; /* number of leaf and merged clusters */
    ClusterNode<T> *nodes; /* leaf and merged clusters */
    T **distances; /* distance between leaves */
};

struct item
{
    Coord coord; /* coordinate of the input data point */
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

template<typename T>
T single_linkage(T **distances, const int a[], const int b[], int m, int n)
{
    T min = 10000000, d;
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            d = distances[a[i]][b[j]];
            if (d < min)
            {
                min = d;
            }
        }
    }
    return min;
}

template<typename T>
T complete_linkage(T **distances, const int a[], const int b[], int m, int n)
{
    T d, max = 0; // assuming distances are positive
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            d = distances[a[i]][b[j]];
            if (d > max)
            {
                max = d;
            }
        }
    }
    return max;
}

template<typename T>
T average_linkage(T **distances, const int a[], const int b[], int m, int n)
{
    T total = 0;
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            total += distances[a[i]][b[j]];
        }
    }
    return total / (m * n);
}

template<typename T>
T get_distance(Cluster<T> *cluster, int index, int target,
               float (*distance_fptr)(T **, const int *, const int *, int, int))
{
    // if both are leaves, just use the distances matrix
    if (index < cluster->num_items && target < cluster->num_items)
        return cluster->distances[index][target];
    else
    {
        ClusterNode<T> *a = &(cluster->nodes[index]);
        ClusterNode<T> *b = &(cluster->nodes[target]);
        return distance_fptr(cluster->distances,
                             a->items, b->items,
                             a->num_items, b->num_items);
    }
}

template<typename T>
void free_neighbours(Neighbor<T> *node)
{
    Neighbor<T> *t;
    while (node)
    {
        t = node->next;
        free(node);
        node = t;
    }
}

template<typename T>
void free_cluster_nodes(Cluster<T> *cluster)
{
    for (int i = 0; i < cluster->num_nodes; ++i)
    {
        ClusterNode<T> *node = &(cluster->nodes[i]);
        if (node->label)
            free(node->label);
        if (node->merged)
            free(node->merged);
        if (node->items)
            free(node->items);
        if (node->neighbours)
            free_neighbours(node->neighbours);
    }
    free(cluster->nodes);
}

template<typename T>
void free_cluster(Cluster<T> *cluster)
{
    if (cluster)
    {
        if (cluster->nodes)
            free_cluster_nodes(cluster);
        if (cluster->distances)
        {
            for (int i = 0; i < cluster->num_items; ++i)
                free(cluster->distances[i]);
            free(cluster->distances);
        }
        free(cluster);
    }
}

template<typename T>
void insert_before(Neighbor<T> *current, Neighbor<T> *neighbours,
                   ClusterNode<T> *node)
{
    neighbours->next = current;
    if (current->prev)
    {
        current->prev->next = neighbours;
        neighbours->prev = current->prev;
    } else
        node->neighbours = neighbours;
    current->prev = neighbours;
}

template<typename T>
void insert_after(Neighbor<T> *current, Neighbor<T> *neighbours)
{
    neighbours->prev = current;
    current->next = neighbours;
}

template<typename T>
void insert_sorted(ClusterNode<T> *node, Neighbor<T> *neighbours)
{
    Neighbor<T> *temp = node->neighbours;
    while (temp->next)
    {
        if (temp->distance >= neighbours->distance)
        {
            insert_before(temp, neighbours, node);
            return;
        }
        temp = temp->next;
    }
    if (neighbours->distance < temp->distance)
        insert_before(temp, neighbours, node);
    else
        insert_after(temp, neighbours);
}

template<typename T>
Neighbor<T> *add_neighbour(Cluster<T> *cluster, int index, int target)
{
    Neighbor<T> *neighbour = new Neighbor<T>;
    if (neighbour)
    {
        neighbour->target = target;
        neighbour->distance = get_distance(cluster, index, target);
        ClusterNode<T> *node = &(cluster->nodes[index]);
        if (node->neighbours)
            insert_sorted(node, neighbour);
        else
            node->neighbours = neighbour;
    } else

        return neighbour;
}

template<typename T>
Cluster<T> *update_neighbours(Cluster<T> *cluster, int index)
{
    ClusterNode<T> *node = &(cluster->nodes[index]);
    if (node->type == NOT_USED)
    {
        cerr << "Invalid cluster node at index " << index << "." << endl;
        cluster = NULL;
    } else
    {
        int root_clusters_seen = 1, target = index;
        while (root_clusters_seen < cluster->num_clusters)
        {
            ClusterNode<T> *temp = &(cluster->nodes[--target]);
            if (temp->type == NOT_USED)
            {
                cerr << "Invalid cluster node at index " << index << "." << endl;
                cluster = NULL;
                break;
            }
            if (temp->is_root)
            {
                ++root_clusters_seen;
                add_neighbour(cluster, index, target);
            }
        }
    }
    return cluster;
}

#define init_leaf(cluster, node, item, len)             \
        do {                                            \
                strncpy(node->label, item->label, len); \
                node->centroid = item->coord;           \
                node->type = LEAF_NODE;                 \
                node->is_root = 1;                      \
                node->height = 0;                       \
                node->num_items = 1;                    \
                node->items[0] = cluster->num_nodes++;  \
        } while (0)                                     \

//inline void init_leaf(Cluster cluster, ClusterNode node, )

#endif //SECUREIMAGESEARCHMIDDLEBOX_CLUSTERING_H
