#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <conio.h>
#include <assert.h>

/* ================= System Parameters =================== */
#define TICK 10  // Time unit (ms)

#define N_LAYER 2
#define MAP_WIDTH 60
#define MAP_HEIGHT 18

/* ================= 위치와 방향 =================== */
// 맵에서 위치를 나타내는 구조체
typedef struct {
    int row;    // 행 좌표
    int column; // 열 좌표
} POSITION;

// 커서 위치
typedef struct {
    POSITION previous;  // 직전 위치
    POSITION current;   // 현재 위치
} CURSOR;

// 입력 가능한 키 종류
typedef enum {
    k_none = 0, k_up, k_right, k_left, k_down,
    k_quit,
    k_undef,
} KEY;

// DIRECTION은 KEY의 부분집합이지만 의미를 명확히 하기 위해 다른 타입으로 정의
typedef enum {
    d_stay = 0, d_up, d_right, d_left, d_down
} DIRECTION;

/* ================= 위치와 방향(편의 함수) =================== */
inline POSITION padd(POSITION p1, POSITION p2) {
    POSITION p = { p1.row + p2.row, p1.column + p2.column };
    return p;
}

inline POSITION psub(POSITION p1, POSITION p2) {
    POSITION p = { p1.row - p2.row, p1.column - p2.column };
    return p;
}

#define is_arrow_key(k)     (k_up <= (k) && (k) <= k_down)
#define ktod(k)             ((DIRECTION)(k))
inline POSITION dtop(DIRECTION d) {
    static POSITION direction_vector[] = { {0, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 0} };
    return direction_vector[d];
}
#define pmove(p, d)         (padd((p), dtop(d)))

/* ================= 게임 데이터 =================== */
typedef struct {
    int spice;
    int spice_max;
    int population;
    int population_max;
} RESOURCE;

typedef struct {
    char* name;
    int cost;
    int capacity;
    int production;
} BUILDING;

typedef struct {
    char* name;
    int cost;
    int attack;
    int defense;
    int speed;
    int health;
} UNIT;

typedef struct {
    POSITION pos;
    POSITION dest;
    char repr;
    int move_period;
    int next_move_time;
} OBJECT_SAMPLE;

/* ================= 함수 선언 =================== */
POSITION find_sandworm();
POSITION find_nearest_unit(POSITION sandworm_pos);
DIRECTION calculate_direction(POSITION from, POSITION to);
POSITION get_random_position();
POSITION get_random_adjacent_position(int row, int col);

#endif
