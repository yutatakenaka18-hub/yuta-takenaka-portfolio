/*
 * Title: Travelling Salesman Problem (TSP) - Algorithm Comparison
 * File: travelling_salesman.c
 *
 * 日本語:
 * 2次元座標で与えられた都市を一度ずつ訪れ、出発都市0に戻る経路を探索する。
 * 全探索 (Brute Force)、最近傍法 (Greedy)、幅2のビームサーチ (Beam2) を
 * 比較し、各経路、総移動距離、実行時間を表示する。
 *
 * English:
 * Finds tours that start at city 0, visit each city once, and return to 0.
 * Compares brute-force search, the greedy nearest-neighbor heuristic, and
 * beam search with width 2. Prints tours, distances, and CPU elapsed time.
 *
 * Concepts: C99, structs, pointers, dynamic memory, recursion,
 * permutations, heuristics, beam search, and Euclidean distance.
 *
 * Compile / コンパイル (MSYS2 UCRT64 Bash):
 * gcc -Wall -Werror -std=c99 travelling_salesman.c -o travelling_salesman.exe -lm
 *
 * Run / 実行 (MSYS2 Bash):
 * ./travelling_salesman.exe < input.txt > output.txt
 *
 * Note / 注意:
 * Timing values depend on the system and may differ on each run.
 * 実行時間はコンピュータや実行ごとに変化する。
 */

# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <math.h>
# include <float.h>
# include <time.h>
# include "travelling_salesman.h"

/*
 * Represents a city using its numeric ID and Cartesian coordinates.
 * 都市のIDと2次元座標（x, y）を保持する。
 */
typedef struct {
    int id;
    float x;
    float y;
} City;

// Function declarations / 関数宣言
void brute_force (City *bf_cities, int n);
void bf_result(int n, int path1[n+1], float shortest);
void swap (int *num1, int *num2);
void permutation(City *bf_cities, int n, int currentpath[n+1], int bestpath[n+1], int start, int end, float *shortest);
void find_shortest (City *bf_cities, int n, int currentpath[n+1], int bestpath[n+1], float *shortest);
float calc_distance (City *cities, int n, int path[n+1]);
void greedy(City *g_cities, int n);
void g_result (int n, int bestpath[n+1], float shortest);
void g_calc_closest(City *g_cities, int loc, int n, int bestpath[n+1]);
void two_beam(City *two_cities, int n);
void find_first_num (City *two_cities, int n, int path[2][n+1]);
void tb_path_choice(City *two_cities, int n, int depth, int path[2][n+1]);
bool city_used(int path[], int depth, int id);
float calc_two_distance (City *two_cities, int loc, int path[]);
void final_two_path (City *two_cities, int n, int path[2][n+1], int best_path[n+1], float *shortest);
void two_result(int n, int bestpath[n+1], float shortest);
void input_cities (City *cities, int n);
void copy_cities (City *cities1, City *cities2, int n);
float two_path_distance(City *two_cities, int depth, int path[depth]);
void copy_paths2 (int len, int path1[len], int path2[len]);
void copy_paths1 (int num1, int num2, int end, int path1[num1], int path2[num2]);
void init_path (int n, int path[n+1]);

/*
 * Reads input, runs each search algorithm, reports CPU time, and frees memory.
 * 入力を読み込み、3つの探索手法を実行して処理時間を表示し、メモリを解放する。
 */
int main (void)  {
    int n = 0;
    scanf(" %d", &n);
    City *cities = (City *) malloc (n * sizeof(City));
    City *bf_cities = (City *) malloc (n * sizeof(City));
    City *g_cities = (City *) malloc (n * sizeof(City));
    City *two_cities = (City *) malloc (n * sizeof(City));
    input_cities(cities, n);
    copy_cities(cities, bf_cities, n);
    copy_cities(cities, g_cities, n);
    copy_cities(cities, two_cities, n);

    // Factorial exhaustive search is intentionally limited to small inputs.
    // 全探索は計算量が階乗的に増えるため、小規模入力のみに実行する。
    if (n <= 12) {
        clock_t bf_start, bf_end;
        bf_start = clock();
        brute_force(bf_cities, n);
        bf_end = clock();
        double time_bf = (double)(bf_end - bf_start) / CLOCKS_PER_SEC;
        printf("%lfs\n\n", time_bf);
    }

    clock_t g_start, g_end;
    g_start = clock();
    greedy(g_cities, n);
    g_end = clock();
    double time_g = (double)(g_end - g_start) / CLOCKS_PER_SEC;
    printf("%lfs\n\n", time_g);

    clock_t two_start, two_end;
    two_start = clock();
    two_beam(two_cities, n);
    two_end = clock();
    double time_two = (double)(two_end - two_start) / CLOCKS_PER_SEC;
    printf("%lfs\n", time_two);

    free(cities);
    free(bf_cities);
    free(g_cities);
    free(two_cities);
    return 0;
}

/*
 * Initializes exhaustive search and handles the one-city case.
 * 全探索の初期化を行い、都市が1つの場合も処理する。
 */
void brute_force (City *bf_cities, int n) {
    float shortest = FLT_MAX;
    int bestpath[n+1], currentpath[n+1];
    init_path(n, currentpath);

    // A single city produces the tour 0 -> 0 with distance zero.
    if (n == 1) {
        bestpath[0] = 0;
        bestpath[1] = 0;
        bf_result(n, bestpath, 0.0f);
        return;
    }

    permutation(bf_cities, n, currentpath, bestpath, 1, n-1, &shortest);
    bestpath[n] = 0;
    bf_result(n, bestpath, shortest);
}

/*
 * Prints the best brute-force tour and its total distance.
 * 全探索で得た最短巡回経路と移動距離を出力する。
 */
void bf_result(int n, int path1[n+1], float shortest) {
    printf("BF\n");
    for (int i = 0; i < n; i++) {
        printf("%d, ", path1[i]);
    } printf("%d\n", path1[n]);
    printf("%.2f\n", shortest);
}

/*
 * Swaps two integer values via pointers.
 * ポインタを使って2つの整数を交換する。
 */
void swap (int *num1, int *num2) {
    int temp = *num1;
    *num1 = *num2;
    *num2 = temp;
}

/*
 * Recursively enumerates tours while keeping city 0 fixed at the start.
 * 出発都市0を固定し、残りの都市の訪問順を再帰的に列挙する。
 */

void permutation(City *bf_cities, int n, int currentpath[n+1], int bestpath[n+1], int start, int end, float *shortest) {
    // A complete permutation has been formed; evaluate its tour length.
    // 訪問順が確定したら、その経路の総距離を評価する。
    if (start == end) {
        find_shortest(bf_cities, n, currentpath, bestpath, shortest);
    }
    else {
        for (int i = start; i <= end; i++) {
            swap(&currentpath[start], &currentpath[i]);
            permutation(bf_cities, n, currentpath, bestpath, start+1, end, shortest);
            swap(&currentpath[start], &currentpath[i]);
        }
    }
}

/*
 * Computes a candidate tour length and retains the better tour using pathcmp.
 * 候補経路の距離を計算し、pathcmpを使ってより適切な経路を記録する。
 */
void find_shortest (City *bf_cities, int n, int currentpath[n+1], int bestpath[n+1], float *shortest) {
    float distance = calc_distance(bf_cities, n, currentpath);
    float compare = pathcmp(currentpath, bestpath, distance, *shortest, n+1);
    if (compare < 0) {
        *shortest = distance;
        copy_paths2(n, bestpath, currentpath);
    }
}

/*
 * Sums Euclidean distances between consecutive cities in a closed tour.
 * 閉路を構成する連続した都市間のユークリッド距離を合計する。
 */
float calc_distance (City *cities, int n, int path[n+1]) {
    float distance = 0.0f;
    float dx, dy;
    for (int i = 0; i < n; i++) {
        dx = cities[path[i+1]].x - cities[path[i]].x;
        dy = cities[path[i+1]].y - cities[path[i]].y;
        distance += sqrtf(dx*dx + dy*dy);
    }
    return distance;
}

/*
 * Constructs a nearest-neighbor tour starting and ending at city 0.
 * 都市0から最近傍の都市を選び、最後に都市0へ戻る経路を構築する。
 */
void greedy(City *g_cities, int n) {
    float shortest = FLT_MAX;
    int bestpath[n+1];
    init_path(n, bestpath);

    for (int loc = 0; loc < n-1; loc++) {
        g_calc_closest(g_cities, loc, n, bestpath);
    } 

    shortest = calc_distance(g_cities, n, bestpath);
    g_result (n, bestpath, shortest);
}

/*
 * Prints the greedy tour and its total distance.
 * 最近傍法で求めた巡回経路と総移動距離を出力する。
 */
void g_result (int n, int bestpath[n+1], float shortest) {
    printf("Greedy\n");
    for (int i = 0; i < n; i++) {
        printf("%d, ", bestpath[i]);
    } printf("%d\n", bestpath[n]);
    printf("%.2f\n", shortest);
}

/*
 * Selects the nearest unused city, using pathcmp for equal-distance ties.
 * 未訪問都市から最も近い都市を選択し、同距離の場合はpathcmpで比較する。
 */
void g_calc_closest(City *g_cities, int loc, int n, int bestpath[n+1]) {
    float closest = FLT_MAX;
    float distance = 0.0f;
    float dx, dy;
    int closest_pt = loc;
    for (int i = loc+1; i < n; i++) {
        dx = g_cities[bestpath[i]].x - g_cities[bestpath[loc]].x;
        dy = g_cities[bestpath[i]].y - g_cities[bestpath[loc]].y;
        distance = sqrtf(dx*dx + dy*dy);

        int candidate_path[2] = {bestpath[loc], bestpath[i]};
        int closest_path[2] = {bestpath[loc], bestpath[closest_pt]};
        if (closest_pt == loc ||
            pathcmp(candidate_path, closest_path, distance, closest, 2) < 0) {
            closest = distance;
            closest_pt = i;
        }
    } 
    // Place the chosen city immediately after the current city.
    // 選択した都市を現在地の直後に移動する。
    swap(&bestpath[loc+1], &bestpath[closest_pt]);
}

/*
 * Builds tours using beam search that retains at most two partial paths.
 * 部分経路を最大2つ保持するビームサーチで巡回経路を構築する。
 */
void two_beam(City *two_cities, int n) {
    int path[2][n+1];
    init_path(n, path[0]);
    init_path(n, path[1]);
    
    find_first_num(two_cities, n, path);

    for (int depth = 2; depth < n; depth++) {
        tb_path_choice(two_cities, n, depth, path);
    }

    int best_path[n+1];
    float shortest = FLT_MAX;
    final_two_path(two_cities, n, path, best_path, &shortest);
    
    two_result(n, best_path, shortest);
}

/*
 * Chooses the first two candidate moves from city 0, if available.
 * 都市0からの最初の移動候補を最大2つ選択する。
 */
void find_first_num (City *two_cities, int n, int path[2][n+1]) {
    float distance_first = FLT_MAX;
    float distance_second = FLT_MAX;
    bool found_best = false, found_second = false;
    int best_id = 0, second_id = 0;

    for (int i = 1; i < n; i++) {
        int current_path[2] = {0, i};
        float current_distance = two_path_distance(two_cities, 2, current_path);

        int best_path[2] = {0, best_id};
        int second_path[2] = {0, second_id};

        float compare1 = pathcmp(current_path, best_path, current_distance, distance_first, 2);
        float compare2 = pathcmp(current_path, second_path, current_distance, distance_second, 2);
        if (!found_best || compare1 < 0) {
            if (found_best) {
                second_id = best_id;
                distance_second = distance_first;
                found_second = true;
            }
            best_id = i;
            distance_first = current_distance;
            found_best = true;
        }
        else if (!found_second || compare2 < 0) {
            second_id = i;
            distance_second = current_distance;
            found_second = true;
        }
    }

    path[0][1] = best_id;
    path[1][1] = found_second ? second_id : best_id;
}

/*
 * Extends the active beams by unused cities and retains two best partial paths.
 * 各候補経路を未訪問都市へ拡張し、評価の良い部分経路を2つ残す。
 */
void tb_path_choice(City *two_cities, int n, int depth, int path[2][n+1]) {
    float distance_first = FLT_MAX, distance_second = FLT_MAX;
    int best_path[n+1], second_path[n+1];
    bool found0 = false, found1 = false;

    for (int i = 0; i < 2; i++) {
        if (path[i][0] == -1) continue;
        
        for (int j = 0; j < n; j++) {
            if (city_used(path[i], depth, j) == false) {
                int current_path[n+1];
                copy_paths1(n+1, n+1, depth, current_path, path[i]);
                current_path[depth] = j;

                float current_distance = two_path_distance(two_cities, depth + 1, current_path);
                float compare1 = pathcmp(current_path, best_path, current_distance, distance_first, depth+1);
                float compare2 = pathcmp(current_path, second_path, current_distance, distance_second, depth+1);
                if (found0 == false || compare1 < 0) {
                    if (found0) {
                        copy_paths2(n+1, second_path, best_path);
                        distance_second = distance_first;
                        found1 = true;
                    }
                    copy_paths2(n+1, best_path, current_path);
                    distance_first = current_distance;
                    found0 = true;
                } 
                else if (found1 == false || compare2 < 0) {
                    copy_paths2(n+1, second_path, current_path);
                    distance_second = current_distance;
                    found1 = true;
                }
            }
        }
    }
    if (found0) copy_paths2(n+1, path[0], best_path);
    if (found1) copy_paths2(n+1, path[1], second_path);
    else if (found0) copy_paths2(n+1, path[1], best_path); 
}

/*
 * Checks whether a city ID already occurs within the current path prefix.
 * 現在の部分経路に指定した都市IDが含まれるか確認する。
 */
bool city_used(int path[], int depth, int id) {
    for (int i = 0; i < depth; i++) {
        if (path[i] == id) return true;
    }
    return false;
}

/*
 * Closes each candidate tour at city 0 and chooses the better completed tour.
 * 候補経路の終点を都市0に戻し、完成した巡回経路の中から良い方を選ぶ。
 */
void final_two_path (City *two_cities, int n, int path[2][n+1], int best_path[n+1], float *shortest) {
    bool found = false;
    for (int i = 0; i < 2; i++) {
        int current_path[n+1];
        copy_paths2(n+1, current_path, path[i]);
        // Complete the round trip by returning to the starting city.
        // 最後に出発都市0へ戻って巡回経路を完成させる。
        current_path[n] = 0;
        
        float distance = two_path_distance(two_cities, n+1, current_path);
        float compare1 = pathcmp(current_path, best_path, distance, *shortest, n+1);
        if (!found || compare1 < 0) {
            *shortest = distance;
            copy_paths2(n+1, best_path, current_path);
            found = true;
        }
    }
}

/*
 * Calculates cumulative distance across the specified number of path edges.
 * 指定した本数の辺に沿った累積移動距離を計算する。
 */
float calc_two_distance (City *two_cities, int loc, int path[]) {
    float dx = 0, dy = 0, distance = 0;
    for (int i = 0; i < loc; i++) {
        dx = two_cities[path[i+1]].x - two_cities[path[i]].x;
        dy = two_cities[path[i+1]].y - two_cities[path[i]].y;
        distance += sqrtf(dx*dx + dy*dy);
    }
    return distance;
}

/*
 * Prints the width-two beam-search tour and its total distance.
 * 幅2のビームサーチで求めた経路と総移動距離を出力する。
 */
void two_result(int n, int bestpath[n+1], float shortest) {
    printf("Beam2\n");
    for (int i = 0; i < n; i++) {
        printf("%d, ", bestpath[i]);
    } printf("%d\n", bestpath[n]);
    printf("%.2f\n", shortest);
}

/*
 * Reads the city coordinates and assigns IDs in input order.
 * 入力された都市の座標を読み取り、入力順にIDを付ける。
 */
void input_cities (City *cities, int n) {
    float x_value = 0, y_value = 0;
    for (int i = 0; i < n; i++) {
        scanf(" (%f, %f)", &x_value, &y_value);
        cities[i].id = i;
        cities[i].x = x_value;
        cities[i].y = y_value;
    }
}

/*
 * Copies city IDs and coordinates into a separate array.
 * 都市IDと座標を別の配列へコピーする。
 */
void copy_cities (City *cities1, City *cities2, int n) {
    for (int i = 0; i < n; i++) {
        cities2[i].id = cities1[i].id;
        cities2[i].x = cities1[i].x;
        cities2[i].y = cities1[i].y;
    }
}

/*
 * Calculates the cumulative Euclidean distance of a partial path.
 * 部分経路の隣接する都市間のユークリッド距離を合計する。
 */
float two_path_distance(City *two_cities, int depth, int path[depth]) {
    float total = 0.0f;
    for (int i = 0; i < depth - 1; i++) {
        float dx = two_cities[path[i]].x - two_cities[path[i+1]].x;
        float dy = two_cities[path[i]].y - two_cities[path[i+1]].y;
        total += sqrtf(dx*dx + dy*dy);
    }
    return total;
}

/*
 * Copies a fixed number of city IDs from one path into another.
 * 経路配列から指定個数の都市IDを別配列へコピーする。
 */
void copy_paths2 (int len, int path1[len], int path2[len]) {
    for (int i = 0; i < len; i++) path1[i] = path2[i];
}

/*
 * Copies only the visited prefix before appending a new city.
 * 次の都市を追加する前に、訪問済みの部分経路だけをコピーする。
 */
void copy_paths1 (int num1, int num2, int end, int path1[num1], int path2[num2]) {
    for (int i = 0; i < end; i++) path1[i] = path2[i];
}

/*
 * Initializes the path as 0, 1, ..., n-1, 0.
 * 経路配列を0, 1, ..., n-1, 0で初期化する。
 */
void init_path (int n, int path[n+1]) {
    for (int i = 0; i < n; i++) path[i] = i; 
    path[n] = 0;
}