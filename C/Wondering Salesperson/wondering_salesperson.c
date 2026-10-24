/*
 * Title: Wandering Salesperson - Grid Movement and Leaderboards
 * タイトル：Wandering Salesperson - グリッド上の移動とランキング
 *
 * 日本語：
 * 乱数シードからマップの幅と高さを生成し、都市と営業担当者を配置する。
 * 移動コマンドに従って座標を更新し、端を越えた場合は反対側に回り込む。
 * 訪問都市数と累積移動距離でランキングを作成し、ゲーム終了時に勝者を表示する。
 *
 * English:
 * Generates a grid from a random seed and initializes cities and salespeople.
 * Processes movement commands with wraparound, tracks travel statistics,
 * displays leaderboards, and announces the final winners.
 *
 * Commands: m (move), lc (cities leaderboard), ld (distance leaderboard),
 *           e (end game)
 * Concepts: C structs, arrays, pointers, modular arithmetic, Euclidean
 *           distance, qsort, formatted I/O, and ranking with ties.
 *
 * Compile / コンパイル:
 * gcc -Wall -Werror -std=c99 wandering_salesperson.c -o wandering_salesperson.exe -lm
 * Run / 実行 (MSYS2 Bash):
 * ./wondering_salesperson.exe < input.txt > output.txt
 *
 */

// Available Libraries
# include <stdio.h>
# include <stdlib.h>
# include <math.h>

typedef int bool;
# define false 0
# define true 1

/*
 * Stores the ID, initial/current coordinates, cities visited and distance.
 * ID・初期位置・現在位置・訪問都市数・移動距離を管理する。
 */
typedef struct {
    int id;
    int init_x;
    int init_y;
    int x_now;
    int y_now;
    int num_city;
    float distance;
} PersonID;

/*
 * Stores the values displayed in a leaderboard.
 * ランキング表に表示する順位・ID・記録を管理する。
 */
typedef struct {
    int place;
    int id;
    int num_city;
    float distance;
} PersonRank;

/*
 * Stores the width and height of the rectangular map.
 * 長方形マップの幅と高さを保持する。
 */
typedef struct {
    int W;
    int H;
} Grid;

// Function Declaration 関数宣言
void calculate_grid (unsigned seed, Grid *grid);
bool read_position(int num_person, PersonID sales_person[num_person], Grid grid);
void salesperson_option (int num_person, PersonID sales_person[num_person], PersonRank sales_city[num_person], PersonRank sales_distance[num_person], Grid grid);
void move_salesperson (int num_person, PersonID sales_person[num_person], Grid grid, int move_info[3]);
bool visit_city (int num_person, int id, PersonID sales_person[num_person]);
float calculate_distance (int dx, int dy);
void cities_visited (int num_person, PersonRank sales_city[num_person]);
int compare_city (const void* a, const void *b);
void city_ranking (int num_person, PersonRank sales_city[num_person]);
void distance_travelled (int num_person, PersonRank sales_distance[num_person]);
int compare_distance(const void* a, const void* b);
void distance_ranking (int num_person, PersonRank sales_distance[num_person]);
void print_winner (int num_person, PersonRank sales_city[num_person], PersonRank sales_distance[num_person]);
void init_person_id (int num_person, PersonID sales_person[num_person]);
void init_person_rank (int num_person, PersonRank sales_person[num_person]);
void init_grid (Grid *grid);
void copy_person (int num_person, PersonID sales_person[num_person], PersonRank sales_rank[num_person]);
void print_line_column (void);
void print_line_plus (void);
void print_line_minus (void);

/*
 * Reads the random seed and initial city data,
 * then starts the game command loop.
 *
 * 乱数シードと都市の初期情報を読み込み、
 * ゲームのコマンド処理を開始する。
 */
int main (void) {
    unsigned int seed;
    scanf(" %d", &seed);
    Grid grid;
    init_grid(&grid);
    calculate_grid (seed, &grid);
    
    int num_person;
    scanf(" %d", &num_person);
    if (scanf(" %d", &num_person) != 1 || num_person <= 0) {
        fprintf(stderr, "Error: invalid number of salespeople\n");
        return EXIT_FAILURE;
    }

    PersonID sales_person[num_person];
    PersonRank sales_city[num_person];
    PersonRank sales_distance[num_person]; 
    init_person_id(num_person, sales_person);
    init_person_rank(num_person, sales_city);
    init_person_rank(num_person, sales_distance);

    bool valid = read_position(num_person, sales_person, grid);
    if (!valid) return 0;

    salesperson_option(num_person, sales_person, sales_city, sales_distance, grid);
    return 0;
}

/*
 * Generates the grid width and height from the random seed.
 * 乱数シードからマップの幅と高さを生成する。
 */
void calculate_grid (unsigned seed, Grid *grid) {
    srand(seed);
    int x_max = abs((rand() % 1000) + 1);
    int y_max = abs((rand() % 1000) + 1);
    grid->W = x_max;
    grid->H = y_max;
}

/*
 * Reads the initial positions of all salespeople
 * and validates their coordinates against the grid boundaries.
 *
 * 各営業担当者の初期座標を読み込み、
 * マップの範囲内にあるか確認する。
 */
bool read_position (int num_person, PersonID sales_person[num_person], Grid grid) {
    int x_loc = 0, y_loc = 0;
    for (int i = 0; i < num_person; i++) {
        scanf(" (%d, %d)", &x_loc, &y_loc);
        if (x_loc > grid.W) return false;
        if (y_loc > grid.H) return false;
        sales_person[i].init_x = x_loc;
        sales_person[i].init_y = y_loc;
        sales_person[i].x_now = x_loc;
        sales_person[i].y_now = y_loc;
    }
    return true;
}

/*
 * Processes movement, leaderboard, and end-game commands.
 * Continues until the end-game command is received.
 *
 * 移動、ランキング表示、ゲーム終了のコマンドを処理する。
 * 終了コマンドが入力されるまで繰り返す。
 */
void salesperson_option (int num_person, PersonID sales_person[num_person], PersonRank sales_city[num_person], PersonRank sales_distance[num_person], Grid grid) {
    char option[3];
    int move_info[3] = {0, 0, 0};

    for (;;){
        scanf(" %2s", option);
        if (option[0] == 'm') {
            scanf(" %d : (%d, %d)", &(move_info[0]), &(move_info[1]), &(move_info[2]));
            move_salesperson(num_person, sales_person, grid, move_info);
        };

        if (option[0] == 'l' && option[1] == 'c') {
            copy_person(num_person, sales_person, sales_city);
            cities_visited(num_person, sales_city);
        };

        if (option[0] == 'l' && option[1] == 'd') {
            copy_person(num_person, sales_person, sales_distance);
            distance_travelled(num_person, sales_distance);
        };

        if (option[0] == 'e') {
            copy_person(num_person, sales_person, sales_city);
            copy_person(num_person, sales_person, sales_distance);
            print_winner(num_person, sales_city, sales_distance);
            break;
        }
    }
}

/*
 * Updates a salesperson's position with wraparound and records travel.
 * 折り返しを考慮して位置を更新し、移動距離と訪問都市数を記録する。
 */
void move_salesperson (int num_person, PersonID sales_person[num_person], Grid grid, int move_info[3]) {
    int id = move_info[0];
    int x_after = (sales_person[id].x_now + move_info[1]) % grid.W;
    int y_after = (sales_person[id].y_now + move_info[2]) % grid.H;
    if (x_after < 0) x_after = grid.W + x_after;
    if (y_after < 0) y_after = grid.H + y_after;

    sales_person[id].x_now = x_after;
    sales_person[id].y_now = y_after;
    sales_person[id].distance += calculate_distance(move_info[1], move_info[2]);
    bool visiting = visit_city(num_person, id, sales_person);
    if (visiting) sales_person[id].num_city += 1;
}

/*
 * Calculates Euclidean distance from the requested DX and DY.
 * 入力された移動量DXとDYからユークリッド距離を計算する。
 */
float calculate_distance (int dx, int dy) {
    return sqrt(pow(dx, 2) + pow(dy, 2));
}

/*
 * Checks whether the current position matches another city's location.
 * 現在位置が別の都市の座標と一致するか調べる。
 */
bool visit_city (int num_person, int id, PersonID sales_person[num_person]) {
    for (int i = 0; i < num_person; i++) {
        if (i == id) continue;
        if (sales_person[id].x_now == sales_person[i].init_x) {
            if (sales_person[id].y_now == sales_person[i].init_y) {
                return true;
            }
        }
    }
    return false;
}

/*
 * Displays up to 10 salespeople ranked by cities visited.
 * 訪問都市数の多い順に最大10人のランキングを表示する。
 */
void cities_visited (int num_person, PersonRank sales_city[num_person]) {
    qsort(sales_city, num_person, sizeof(PersonRank), compare_city);
    city_ranking(num_person, sales_city);

    print_line_plus();
    print_line_column();
    print_line_minus();
    for (int i = 0; i < num_person && i < 10; i++) {
        printf("| %4d. | %-11d | %-14d | %-18.1f |\n", sales_city[i].place, sales_city[i].id, sales_city[i].num_city, sales_city[i].distance);
    }
    print_line_plus();
}

/*
 * Compares city counts descending, breaking ties by ID ascending.
 * 訪問都市数の降順、同点の場合はIDの昇順で比較する。
 */
int compare_city (const void* a, const void *b) {
    const PersonRank *p1 = (const PersonRank *)a;
    const PersonRank *p2 = (const PersonRank *)b;
    return (p2->num_city - p1->num_city);
}

/*
 * Assigns the same leaderboard place to equal city counts.
 * 訪問都市数が等しい場合、同じ順位を割り当てる。
 */
void city_ranking (int num_person, PersonRank sales_city[num_person]) {
    int city_place = 1;
    for (int i = 0; i < num_person-1; i++) {
        sales_city[i].place = city_place;
        if (sales_city[i+1].num_city < sales_city[i].num_city) {
            city_place++;
        }
    } sales_city[num_person-1].place = city_place;
}

/*
 * Displays up to 10 salespeople ranked by distance travelled.
 * 累積移動距離の多い順に最大10人のランキングを表示する。
 */
void distance_travelled (int num_person, PersonRank sales_distance[num_person]) {
    qsort(sales_distance, num_person, sizeof(PersonRank), compare_distance);
    distance_ranking(num_person, sales_distance);

    print_line_plus();
    print_line_column();
    print_line_minus();
    for (int i = 0; i < num_person && i < 10; i++) {
        printf("| %4d. | %-11d | %-14d | %-18.1f |\n", sales_distance[i].place, sales_distance[i].id, sales_distance[i].num_city, sales_distance[i].distance);
    }
    print_line_plus();
}

/*
 * Compares distances descending, breaking ties by ID ascending.
 * 累積移動距離の降順、同点の場合はIDの昇順で比較する。
 */
int compare_distance (const void* a, const void* b) {
    const PersonRank *p1 = (const PersonRank *)a;
    const PersonRank *p2 = (const PersonRank *)b;
    if ((p2->distance - p1->distance) > 0) return 1;
    if ((p2->distance - p1->distance) < 0) return -1;
    return 0;
}

/*
 * Assigns the same leaderboard place to equal distances.
 * 移動距離が等しい場合、同じ順位を割り当てる。
 */
void distance_ranking (int num_person, PersonRank sales_distance[num_person]) {
    int distance_place = 1;
    for (int i = 0; i < num_person-1; i++) {
        sales_distance[i].place = distance_place;
        if (sales_distance[i+1].distance < sales_distance[i].distance) {
            distance_place++;
        }
    } sales_distance[num_person-1].place = distance_place;
}

/*
 * Prints all salespeople tied for first place in both categories.
 * 訪問都市数と移動距離の各部門で1位の全員を表示する。
 */
void print_winner (int num_person, PersonRank sales_city[num_person], PersonRank sales_distance[num_person]) {
    qsort(sales_city, num_person, sizeof(PersonRank), compare_city);
    city_ranking(num_person, sales_city);
    qsort(sales_distance, num_person, sizeof(PersonRank), compare_distance);
    distance_ranking(num_person, sales_distance);

    printf("Winners:\n");

    printf("%-19s: ", "Cities Visited");
    for (int i = 0; i < num_person; i++) {
        if (sales_city[i].place == 1) {
            if (i == 0) printf("%d", sales_city[i].id);
            else printf(", %d", sales_city[i].id);
        }
        else break;
    }
    printf(" with %d\n", sales_city[0].num_city);

    printf("%-19s: ", "Distance Travelled");
    for (int i = 0; i < num_person; i++) {
        if (sales_distance[i].place == 1) {
            if (i == 0) printf("%d", sales_distance[i].id);
            else printf(", %d", sales_distance[i].id);
        }
        else break;
    }
    printf(" with %.1f\n", sales_distance[0].distance);
}

/*
 * Initializes each salesperson's ID, position, and counters.
 * 各営業担当者のID、位置、訪問都市数、移動距離を初期化する。
 */
void init_person_id (int num_person, PersonID sales_person[num_person]) {
    for (int i = 0; i < num_person; i++) {
        sales_person[i].id = i;
        sales_person[i].init_x = 0;
        sales_person[i].init_y = 0;
        sales_person[i].x_now = 0;
        sales_person[i].y_now = 0;
        sales_person[i].num_city = 1;
        sales_person[i].distance = 0;
    }
}

/*
 * Initializes leaderboard records for all salespeople.
 * 全営業担当者のランキング用データを初期化する。
 */
void init_person_rank (int num_person, PersonRank sales_person[num_person]) {
    for (int i = 0; i < num_person; i++) {
        sales_person[i].place = 1;
        sales_person[i].id = i;
        sales_person[i].num_city = 1;
        sales_person[i].distance = 0;
    }
}

/*
 * Copies the current statistics to the ranking array.
 * 現在の記録をランキング用の配列にコピーする。
 */
void copy_person (int num_person, PersonID sales_person[num_person], PersonRank sales_rank[num_person]) {
    for (int i = 0; i < num_person; i++) {
        sales_rank[i].id = sales_person[i].id;
        sales_rank[i].num_city = sales_person[i].num_city;
        sales_rank[i].distance = sales_person[i].distance;
    }
}

/*
 * Initializes the grid dimensions to zero.
 * マップの幅と高さを0で初期化する。
 */
void init_grid (Grid *grid) {
    grid->W = 0;
    grid->H = 0;
} 

/*
 * Prints the leaderboard column headings.
 * ランキング表の列見出しを表示する。
 */
void print_line_column (void) {
    printf("| Place | Salesperson | Cities Visited | Distance Travelled |\n");
}

/*
 * Prints a horizontal border around the leaderboard.
 * ランキング表の外側の横線を表示する。
 */
void print_line_plus (void) {
    printf("+-------+-------------+----------------+--------------------+\n");
}

/*
 * Prints the separator under the table headings.
 * ランキング表の見出しの区切り線を表示する。
 */
void print_line_minus (void) {
    printf("|-------+-------------+----------------+--------------------|\n");
}