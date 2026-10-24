/*
 * Title: Cookie Search - Linear vs Binary Search in C
 *
 * 日本語:
 * クッキーIDを2次元配列に読み込み、昇順にソートする。
 * 線形探索と二分探索で指定されたIDを検索し、
 * ソート後の行・列の位置と探索に必要なアクセス回数を出力する。
 * 配列の要素アクセスにはポインタ演算を使用する。
 *
 * English:
 * Reads cookie IDs into a two-dimensional array, sorts them in
 * ascending order, and searches for requested IDs using linear
 * and binary search. Reports their positions in the sorted array
 * and the number of array accesses (peeks) for each algorithm.
 * Array elements are accessed using pointer arithmetic.
 *
 * Concepts: C Pointers, Dynamic Memory Allocation, 2D Arrays,
 *           Bubble Sort, Linear Search, Binary Search, File I/O
 *
 * Compile / コンパイル (MSYS2 Bash):
 * gcc -Wall -Werror -std=c99 cookie_search.c -o cookie_search.exe
 *
 * Run / 実行 (MSYS2 Bash):
 * ./cookie_search.exe cookies.txt input.txt output.txt
 */


// Available Libraries
// 使用可能ライブラリ
# include <stdio.h>
# include <stdlib.h>

// Function prototypes / 関数宣言
int read_cookies (FILE *cookie_file, int n, int m, int **cookies_list);
void sort_cookies (int n, int m, int num_cookies, int **cookies_list);
void init_cookies (FILE *cookie_file, int *n, int *m, int *num_cookies, int ***cookies_list);
void find_cookies (FILE *input_file, FILE *output_file, int n, int m, int num_cookies, int **cookies_list);
int linear_search (int n, int m, int cookie_id, int **cookies_list, int *index_row, int *index_col);
int binary_search (int n, int m, int num_cookies, int cookie_id, int **cookies_list);
void free_cookies (int ***cookies_list, int n);
void file_not_found (FILE *file, char filename[]);
void print_cookies (int n, int m, int **cookies_list);

/**
 * Opens input/output files, coordinates sorting and searching,
 * then frees allocated memory.
 * ファイルを開き、ソートと検索を実行し、確保したメモリを解放する。
 * @param argc Number of command-line arguments / 引数の個数
 * @param argv File paths in order: cookies, queries, output / ファイル名
 * @return 0 on normal completion; nonzero for detected read errors.
 */
int main (int argc, char **argv) {
    FILE *cookie_file = fopen(*(argv+1), "r");
    file_not_found(cookie_file, "cookie_file");
    FILE *input_file = fopen(*(argv+2), "r");
    file_not_found(input_file, "input_file");
    FILE *output_file = fopen(*(argv+3), "w");
    file_not_found(output_file, "output_file");

    int n = 0, m = 0, num_cookies = 0, **cookies_list;
    init_cookies(cookie_file, &n, &m, &num_cookies, &cookies_list);

    // Validate that the expected cookie IDs can be read.
    // 必要な数のクッキーIDを読み取れるか確認する。
    int end = read_cookies(cookie_file, n, m, cookies_list);
    if (end == 1) return 1;
    sort_cookies(n, m, num_cookies, cookies_list);
    find_cookies (input_file, output_file, n, m, num_cookies, cookies_list);

    free_cookies(&cookies_list, n);
    fclose(cookie_file);
    fclose(input_file);
    fclose(output_file);
    return 0;
}

/**
 * Reads cookie IDs from the file into the allocated matrix.
 * クッキーIDをファイルから読み込み、2次元配列に保存する。
 * 
 * @param cookie_file Input file positioned after the dimensions.
 * @param n Number of rows / 行数
 * @param m Number of columns / 列数
 * @param cookies_list Pointer-based 2D array / 2次元配列
 * @return 0 if all expected IDs are read, otherwise 1.
 */
int read_cookies (FILE *cookie_file, int n, int m, int **cookies_list) {
    int num = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (fscanf(cookie_file, " %d", &num) != 1) {
                return 1;
            };
            *(*(cookies_list+i)+j) = num;
        }
    }
    return 0;
}

/**
 * Sorts all cookies in ascending order using bubble sort.
 * バブルソートで全クッキーを昇順に並べ替える。
 *
 * Treats the matrix as a row-major sequence. A flat index i maps
 * to (i / m, i % m); matrix elements are accessed by pointers.
 * 2次元配列を行優先の1次元列として扱い、商と余りで座標を求める。
 * Time complexity / 計算量: O((n*m)^2).
 */
void sort_cookies (int n, int m, int num_cookies, int **cookies_list) {
    int row1 = 0, col1 = 0, row2 = 0, col2 = 0;
    for (int i = 0; i < num_cookies-1; i++) {
        for (int j = 0; j < num_cookies-i-1; j++) {
            // Convert a flattened position to a row/column pair.
            // 1次元インデックスを行番号と列番号に変換する。
            row1 = j / m, col1 = j % m;
            row2 = (j+1) / m, col2 = (j+1) % m;

            if ((*(*(cookies_list+row1)+col1)) > (*(*(cookies_list+row2)+col2))) {
                int temp = *(*(cookies_list+row1)+col1);
                *(*(cookies_list+row1)+col1) = *(*(cookies_list+row2)+col2);
                *(*(cookies_list+row2)+col2) = temp;
            }
        }
    }
}

/**
 * Searches for each requested cookie and writes its search report.
 * 検索対象のIDごとに結果と探索回数をファイルへ出力する。
 *
 * Reports the sorted row/column index or (-1, -1) if not found.
 * 見つからない場合は座標 (-1, -1) を出力する。
 */
void find_cookies (FILE *input_file, FILE *output_file, int n, int m, int num_cookies, int **cookies_list) {
    int cookie_id = 0, linear_num = 0, binary_num = 0;
    int index_row = 0, index_col = 0;

    while (fscanf(input_file, " %d", &cookie_id) == 1) {
        // Run both algorithms on the same sorted cookie matrix.
        // 同じソート済み配列で線形探索と二分探索を比較する。
        linear_num = linear_search(n, m, cookie_id, cookies_list, &index_row, &index_col);
        binary_num = binary_search(n, m, num_cookies, cookie_id, cookies_list);

        fprintf(output_file, "cookieid: %d\n", cookie_id);
        fprintf(output_file, "cookie found?: ");
        if (index_row != -1 && index_col != -1) fprintf(output_file, "yes!\n");
        else fprintf(output_file, "no!\n");
        
        fprintf(output_file, "index: %d %d\n", index_row, index_col);
        fprintf(output_file, "peeks for linear search: %d\n", linear_num);
        fprintf(output_file, "peeks for binary search: %d\n", binary_num);
    }
}

/**
 * Scans the sorted matrix row by row until a cookie is found.
 * 配列を先頭から順番に調べ、指定IDを探す。
 * 
 * @param cookie_id Target cookie ID / 検索対象ID
 * @param index_row Receives row index, or -1 / 行番号の出力先
 * @param index_col Receives column index, or -1 / 列番号の出力先
 * @return Number of cookie IDs inspected / 比較した要素数
 * Time complexity / 計算量: O(n*m).
 */
int linear_search (int n, int m, int cookie_id, int **cookies_list, int *index_row, int *index_col) {
    int counter = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            counter++;
            if ((*(*(cookies_list+i)+j)) == cookie_id) {
                *index_row = i;
                *index_col = j;
                return counter;
            }
        }
    }

    *index_row = -1;
    *index_col = -1;
    return counter;
}

/**
 * Uses binary search over the sorted matrix (row-major order).
 * ソート済みの2次元配列を二分探索する。
 *
 * Compares the middle ID and narrows the search interval each step.
 * 中央の要素と比較して探索範囲を半分ずつ狭める。
 * @return Number of examined middle elements / 配列へのアクセス回数
 * Time complexity / 計算量: O(log(n*m)).
 */
int binary_search (int n, int m, int num_cookies, int cookie_id, int **cookies_list) {
    int left = 0, mid = 0, right = num_cookies-1;
    int row = 0, col = 0;
    int counter = 0;

    while (left <= right) {
        counter++;
        mid = left + (right - left) / 2;
        row = mid / m;
        col = mid % m;
        if (*(*(cookies_list+row)+col) == cookie_id) {
            return counter;
        }
        if (*(*(cookies_list+row)+col) < cookie_id) {
            left = mid + 1;
        }
        if (*(*(cookies_list+row)+col) > cookie_id) {
            right = mid - 1;
        }
    }
    return counter;
}

/**
 * Reads matrix dimensions, allocates rows, and initializes IDs.
 * 行数・列数を読み取り、動的メモリを確保して0で初期化する。
 *
 * A pointer to the matrix pointer allows this function to update
 * the caller's allocated matrix address.
 * 三重ポインタを使って呼び出し元の配列ポインタを更新する。
 */
void init_cookies (FILE *cookie_file, int *n, int *m, int *num_cookies, int ***cookies_list) {
    fscanf(cookie_file, " %d %d", n, m);
    *num_cookies = (*n) * (*m);

    // Allocate one pointer per row, then allocate each row.
    // 行ポインタの配列と各行のメモリ領域を確保する。
    *cookies_list = malloc ((*n) * sizeof(int *));
    for (int i = 0; i < (*n); i++){
        *((*cookies_list)+i) = malloc ((*m) * sizeof(int));
    }

    for (int i = 0; i < (*n); i++) {
        for (int j = 0; j < (*m); j++) {
            *(*((*cookies_list)+i)+j) = 0;
        }
    }
}

/**
 * Frees every matrix row and the array of row pointers.
 * 各行のメモリと行ポインタの配列を解放する。
 */
void free_cookies (int ***cookies_list, int n) {
    for (int i = 0; i < n; i++) {
        free(*((*cookies_list)+i));
    } free(*cookies_list);
}

/**
 * Prints an error and exits if an input/output file failed to open.
 * ファイルを開けなかった場合、エラーを表示して異常終了する。
 */
void file_not_found (FILE *file, char filename[]) {
    if (file == NULL) {
        fprintf(stderr, "ERROR: %s does not exist\n", filename);
        exit(EXIT_FAILURE);
    }
}

/**
 * Prints every cookie ID for debugging (not normally called).
 * デバッグ用に配列内のIDを表示する（通常の処理では未使用）。
 */
void print_cookies (int n, int m, int **cookies_list) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("%d\n", *(*(cookies_list+i)+j));
        }
    }
}