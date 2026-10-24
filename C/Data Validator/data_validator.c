/* 
 *  ゲームのセーブデータとマップ情報をファイルから読み込み、
 * JSON形式の構造、各ステータスの値、座標の有効性を検証する。
 * 正常なセーブデータと破損したセーブデータを分類し、
 * タイムスタンプの昇順にソートして別々のファイルへ出力する。
 *
 * Reads game save records and map information from input files.
 * Validates the save record format, character statistics,
 * and coordinates based on map constraints.
 * Separates valid and corrupted save records, sorts them
 * by timestamp, and writes them to separate output files.
 *
 * Concepts: File I/O, Pointers, Arrays, String Manipulation,
 * Data Validation, 2D Arrays, Bubble Sort
 */


// 課題で使用可能なライブラリ
// Available Library
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

typedef int bool;
# define false 0
# define true 1

# define NUM_STRING 100

// Function Declarations 関数宣言
void read_json(FILE *database, FILE *output, FILE *broken, FILE *map);
bool read_line(FILE *database, char *line_info);
bool broken_line (char *line_info, int line_value[]);
void read_map(FILE *map, int num_rows, int num_cols, int map_info[num_rows][num_cols]);
bool illegal_line (int line_value[], int num_rows, int num_cols, int map_info[num_rows][num_cols], int n);
void copy_string (char *line_info, char *info);
void sort_incorrect_info (int num_rows, int use_rows, int info[num_rows]);
void sort_correct_info (int num_rows, int use_rows, char info[num_rows][NUM_STRING]);
void output_correct (FILE *output, int num_correct, char correct_info[num_correct][NUM_STRING]);
void output_broken (FILE *broken, int num_incorrect, int incorrect_info[num_incorrect]);
void init_array (int *array, int n);
void init_string_array (int rows, int cols, char array[rows][cols]);

/*
 * Opens the save file, map file, and two output files.
 * Calls the validation process and closes all files.
 *
 * セーブファイル、マップファイル、2つの出力ファイルを開く。
 * データ検証処理を呼び出した後、すべてのファイルを閉じる。
 */
int main (int argc, char *argv[]) {
    /*
    コンパイル方法 / How to Compile:
    gcc -Wall -Werror -std=c99 data_validator.c -o data_validator.exe
    
    実行方法 / How to Run:
    .\data_validator.exe save_files.txt output.txt broken_output.txt map.txt
    */

    FILE *database = fopen(argv[1], "r");
    if (database == NULL) {
        fprintf(stderr, "ERROR: save_files.txt does not exist\n");
        exit(EXIT_FAILURE);
    }

    FILE *output = fopen(argv[2], "w");
    if (output == NULL) {
        fprintf(stderr, "ERROR: output.txt does not exist\n");
        exit(EXIT_FAILURE);
    }

    FILE *broken = fopen(argv[3], "w");
    if (broken == NULL) {
        fprintf(stderr, "ERROR: broken.txt does not exist\n");
        exit(EXIT_FAILURE);
    }

    FILE *map = fopen(argv[4], "r");
    if (map == NULL) {
        fprintf(stderr, "ERROR: map.txt does not exist\n");
        exit(EXIT_FAILURE);
    }

    // Validate and organize game save records
    // ゲームのセーブデータを検証・分類する
    read_json(database, output, broken, map);

    // Close all files / ファイルを閉じる
    fclose(database);
    fclose(output);
    fclose(broken);
    fclose(map);
    return 0;
}

/*
 * Processes game save records and separates valid and broken saves.
 * Reads map dimensions, validates each record, sorts the results
 * by timestamp, and writes them to the corresponding output files.
 *
 * セーブデータを読み込み、正常なデータと破損データを分類する。
 * マップ情報と各フィールドの値を検証し、
 * タイムスタンプ順にソートして出力する。
 */
void read_json(FILE *database, FILE *output, FILE *broken, FILE *map) {
    char line_info[NUM_STRING];
    int n = 0, num_rows = 0, num_cols = 0;
    int line_value[5];
    init_array(line_value, 5);

    // Read the number of saves and map dimensions
    // セーブデータ数とマップの行数・列数を取得
    fscanf(database, "%d\n", &n);
    fscanf(map, "%d, %d\n", &num_rows, &num_cols);

    int map_info[num_rows][num_cols];
    read_map(map, num_rows, num_cols, map_info);

    // Store valid save records and timestamps of broken saves
    // 正常なセーブデータと破損データのタイムスタンプを保存
    int incorrect_info[n];
    char correct_info[n][NUM_STRING];
    init_array(incorrect_info, num_rows);
    init_string_array(num_rows, NUM_STRING, correct_info);

    int num_incorrect = 0, num_correct = 0;

    // Check each game save record
    // 各セーブデータを順番に検証
    for (int i = 0; i < n; i++) {
        bool end = false, broken = false, illegal = false;
        init_array(line_value, 5);

        end = read_line (database, line_info);
        if (end) break;

        // Check whether the save record follows the expected format
        // セーブデータが指定された形式に一致するか確認
        broken = broken_line(line_info, line_value);
        if (broken) {
            incorrect_info[num_incorrect++] = line_value[0];
            continue;
        }

        // Check character statistics and map coordinates
        // キャラクターのステータスとマップ座標を検証
        illegal = illegal_line(line_value, num_rows, num_cols, map_info, n);
        if (illegal) {
            incorrect_info[num_incorrect++] = line_value[0];
            continue;
        }

        // Store the original valid save record
        // 正常なセーブデータを保存
        strcpy(correct_info[num_correct++], line_info);
    }

    // Sort valid saves and broken timestamps in ascending order
    // 正常データと破損データのタイムスタンプを昇順にソート
    sort_correct_info(num_rows, num_correct, correct_info);
    sort_incorrect_info(num_rows, num_incorrect, incorrect_info);

    // Write valid saves and broken timestamps to separate files
    // 正常データと破損データのタイムスタンプを別々に出力
    output_correct(output, num_correct, correct_info);
    output_broken(broken, num_incorrect, incorrect_info);

    return;
}

/*
 * Reads one game save record from the input file.
 * 入力ファイルからセーブデータを1行読み込む。
 * Returns true when the end of the file is reached.
 */
bool read_line (FILE *database, char *line_info) {
    if (EOF == fscanf(database, "%100[^\n]\n", line_info)) return true;
    else return false;
}
 
/*
 * Checks whether the save record matches the expected field format.
 * Fields must follow the order: health, x_loc, y_loc, grade.
 * Returns true if the expected fields cannot be parsed.
 *
 * セーブデータの形式とフィールドの順序を確認する。
 * 指定された形式で読み取れない場合はtrueを返す。
 */
bool broken_line (char *line_info, int line_value[]) {
    if (sscanf(line_info, "%d {\"health\": %d, \"x_loc\": %d, \"y_loc\": %d, \"grade\": %d}", &(line_value[0]), &(line_value[1]), &(line_value[2]), &(line_value[3]), &(line_value[4])) != 5) {
        return true;
    } else return false;
}

/*
 * Checks game save values against the required constraints.
 * Validates timestamps, health, coordinates, grades,
 * and whether the player's position is blocked on the map.
 *
 * タイムスタンプ、体力、座標、成績の値を検証する。
 * また、プレイヤーの位置がマップ上の障害物と
 * 重なっていないかを確認する。
 */
bool illegal_line (int line_value[], int num_rows, int num_cols, int map_info[num_rows][num_cols], int n) {
    // Timestamp must be between 0 and n-1
    // タイムスタンプは0以上n-1以下
    if (line_value[0] < 0 || line_value[0] > (n-1)) return true;

    // Health must be between 0 and 5
    // 体力は0以上5以下
    if (line_value[1] < 0 || line_value[1] > 5) return true;

    // Coordinates must be within the map boundaries
    // 座標がマップの範囲内にあるか確認
    if (line_value[2] < 0 || line_value[2] >= num_rows) return true;
    if (line_value[3] < 0 || line_value[3] >= num_cols) return true;

    // Grade must be between 50 and 104
    // 成績は50以上104以下
    if (line_value[4] < 50 || line_value[4] > 104) return true;

    // Reject positions occupied by obstacles
    // 障害物（X）が存在する座標を無効とする
    if (map_info[line_value[2]][line_value[3]] == 1) return true;

    return false;
}

/*
 * Reads the game map into a two-dimensional array.
 * 'O' represents an available position.
 * 'X' represents a blocked position.
 *
 * ゲームマップを2次元配列に読み込む。
 * Oは移動可能な位置、Xは障害物を表す。
 */
void read_map (FILE *map, int num_rows, int num_cols, int map_info[num_rows][num_cols])  {
    char taken = '\0';
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            fscanf(map, " %c", &taken);
            if (taken == 'O') map_info[i][j] = 0;
            if (taken == 'X') map_info[i][j] = 1;
        }
    }
    return;
}

/*
 * Sorts valid game save records by timestamp using bubble sort.
 * 正常なセーブデータをバブルソートで
 * タイムスタンプの昇順に並べ替える。
 */
void sort_correct_info (int num_rows, int use_rows, char info[num_rows][NUM_STRING]) {
    int num1 = 0, num2 = 0;
    char temp[NUM_STRING];
    for (int i = 0; i < (use_rows - 1); ++i) {
        for (int j = 0; j < use_rows - 1 - i; ++j) {
            // Extract timestamps from save records
            // セーブデータからタイムスタンプを取得
            sscanf(info[j], "%d", &num1);
            sscanf(info[j+1], "%d", &num2);
            
            // Swap records if timestamps are in the wrong order
            // タイムスタンプの順序が逆の場合は入れ替える
            if (num1 > num2) {
                strcpy(temp, info[j+1]);
                strcpy(info[j+1], info[j]);
                strcpy(info[j], temp);
            }
        }
    }
}

/*
 * Sorts timestamps of corrupted saves using bubble sort.
 * 破損したセーブデータのタイムスタンプを
 * バブルソートで昇順に並べ替える。
 */
void sort_incorrect_info (int num_rows, int use_rows, int info[num_rows]) {
    int temp = 0;
    for (int i = 0; i < (use_rows - 1); i++) {
        for (int j = 0; j < use_rows - 1 - i; j++) {
            if (info[j] > info[j+1]) {
                temp = info[j+1];
                info[j+1] = info[j];
                info[j] = temp;
            }
        }
    }
}

/*
 * Writes valid game save records to the output file.
 * 正常なセーブデータを出力ファイルに書き込む。
 */
void output_correct (FILE *output, int num_correct, char correct_info[num_correct][NUM_STRING]) {
    for (int i = 0; i < num_correct; i++) {
        fprintf(output, "%s\n", correct_info[i]);
    }
}

/*
 * Writes only the timestamps of corrupted saves.
 * 破損したセーブデータのタイムスタンプのみを出力する。
 */
void output_broken (FILE *broken, int num_incorrect, int incorrect_info[num_incorrect]) {
    for (int i = 0; i < num_incorrect; i++) {
        fprintf(broken, "%d\n", incorrect_info[i]);
    }
}

/*
 * Initializes an integer array with zeros.
 * 整数配列の各要素を0で初期化する。
 */
void init_array (int *array, int n) {
    for (int i = 0; i < n; i++) array[i] = 0;
    return; 
}

/*
 * Initializes a two-dimensional character array
 * using null characters.
 * 2次元文字配列をヌル文字で初期化する。
 */
void init_string_array (int rows, int cols, char array[rows][cols]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            array[i][j] = '\0';
        }
    }
    return;
}
