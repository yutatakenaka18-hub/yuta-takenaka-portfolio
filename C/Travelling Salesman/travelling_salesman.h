/*
 * Title: Travelling Salesman Problem - Path Comparison Helper
 * File: travelling_salesman.h
 *
 * 日本語:
 * 2つの経路を総移動距離と辞書順で比較する補助関数を提供する。
 * この関数は元の課題で提供されたコードを基にしている。
 *
 * English:
 * Provides a helper for comparing paths by total distance and,
 * when distances tie, by lexicographic city ID order.
 * This pathcmp implementation originated in the provided course header.
 */

#ifndef TRAVELLING_SALESMAN_H
#define TRAVELLING_SALESMAN_H

#include <math.h>

/**
 * @brief   Compares two paths of the same size. This function should be used by beam search methods when choosing to keep one of two paths.
 *          This function will also compare tours, but the distance parameters need to account for the distance between the first and last city for both tours.
 * @param   path1       (const int *)   Path 1 as an array of city IDs.
 * @param   path2       (const int *)   Path 2 as an array of city IDs.
 * @param   path1_dist  (float)         Total distance of path 1.
 * @param   path2_dist  (float)         Total distance of path 2.
 * @param   size        (int)           Size of both paths (the number of cities visited).
 * @return              (float)         If 0, then both paths are identical.
 *                                      If negative, then choose path 1.
 *                                      If positive, then choose path 2.
 */
float pathcmp(const int *path1, const int *path2, float path1_dist, float path2_dist, int size)
{
    if (fabs(path1_dist - path2_dist) > 1e-9)
        return path1_dist - path2_dist;
    int i = 0;
    while (i < size - 1 && path1[i] == path2[i]) i++;
    return (float)path1[i] - (float)path2[i];
}

#endif
