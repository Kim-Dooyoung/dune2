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

/* ================= ��ġ�� ���� =================== */
// �ʿ��� ��ġ�� ��Ÿ���� ����ü
typedef struct {
    int row;    // �� ��ǥ
    int column; // �� ��ǥ
} POSITION;

// Ŀ�� ��ġ
typedef struct {
    POSITION previous;  // ���� ��ġ
    POSITION current;   // ���� ��ġ
} CURSOR;

// �Է� ������ Ű ����
typedef enum {
    k_none = 0, k_up, k_right, k_left, k_down,
    k_quit,
    k_undef,
} KEY;

// DIRECTION�� KEY�� �κ����������� �ǹ̸� ��Ȯ�� �ϱ� ���� �ٸ� Ÿ������ ����
typedef enum {
    d_stay = 0, d_up, d_right, d_left, d_down
} DIRECTION;

/* ================= ��ġ�� ����(���� �Լ�) =================== */
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

/* ================= ���� ������ =================== */
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

/* ================= �Լ� ���� =================== */
POSITION find_sandworm();
POSITION find_nearest_unit(POSITION sandworm_pos);
DIRECTION calculate_direction(POSITION from, POSITION to);
POSITION get_random_position();
POSITION get_random_adjacent_position(int row, int col);

#endif
