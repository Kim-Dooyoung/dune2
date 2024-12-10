#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"
#include <time.h>

void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);
void sample_obj_move(void);
void select_object();       // select_object 함수 선언
void cancel_selection();    // cancel_selection 함수 선언
POSITION sample_obj_next_position(void);
DIRECTION last_dir = d_stay;
clock_t last_time = 0;
POSITION find_sandworm();
POSITION find_nearest_unit(POSITION sandworm_pos);
DIRECTION calculate_direction(POSITION from, POSITION to);
POSITION get_random_position();
POSITION get_random_adjacent_position(int row, int col);
void produce_unit(char unit_type);

int sandworm_size = 1;

char selected_object = '\0'; // 현재 선택된 오브젝트 저장


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

POSITION find_sandworm() {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (map[1][i][j] == 'W') { // 샌드웜을 찾으면 위치 반환
				return (POSITION) { i, j };
			}
		}
	}
	// 샌드웜이 없으면 기본값 반환
	return (POSITION) { -1, -1 };
}

POSITION find_nearest_unit(POSITION sandworm_pos) {
	int min_distance = INT_MAX;
	POSITION nearest_unit = { -1, -1 };

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (map[0][i][j] == 'H' || map[0][i][j] == 'S' || map[0][i][j] == 'B') { // 유닛이나 건물
				int distance = abs(sandworm_pos.row - i) + abs(sandworm_pos.column - j);
				if (distance < min_distance) {
					min_distance = distance;
					nearest_unit = (POSITION){ i, j };
				}
			}
		}
	}

	return nearest_unit;
}

POSITION get_random_position() {
	int row = rand() % MAP_HEIGHT;
	int col = rand() % MAP_WIDTH;
	return (POSITION) { row, col };
}

POSITION get_random_adjacent_position(int row, int col) {
	POSITION directions[] = {
		{ -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }
	};

	int index = rand() % 4;
	POSITION new_pos = { row + directions[index].row, col + directions[index].column };

	// 맵 경계 안에 있는지 확인
	if (new_pos.row >= 0 && new_pos.row < MAP_HEIGHT &&
		new_pos.column >= 0 && new_pos.column < MAP_WIDTH) {
		return new_pos;
	}

	// 경계를 벗어나면 현재 위치 반환
	return (POSITION) { row, col };
}




DIRECTION calculate_direction(POSITION from, POSITION to) {
	int row_diff = to.row - from.row;
	int col_diff = to.column - from.column;

	if (abs(row_diff) > abs(col_diff)) {
		return (row_diff > 0) ? d_down : d_up;
	}
	else {
		return (col_diff > 0) ? d_right : d_left;
	}
}


RESOURCE resource = {
	.spice = 0,
	.spice_max = 0,
	.population = 0,
	.population_max = 0
};

OBJECT_SAMPLE obj = {
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'o',
	.move_period = 300,
	.next_move_time = 300
};

/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();
	display(resource, map, cursor);

	while (1) {
		KEY key = get_key();

		if (is_arrow_key(key)) {
			cursor_move(ktod(key));
		}
		else {
			switch (key) {
			case k_quit:
				outro();
				return 0;
			case ' ': // 스페이스바 입력
				select_object();
				break;
			case 'X': // 선택 취소
				cancel_selection();
				break;
			default:
				break;
			}
		}

		display(resource, map, cursor);
		Sleep(TICK);
	}
}


/* ================= subfunctions =================== */
void intro(void) {
	printf("DUNE 1.5\n");
	Sleep(2000);
	system("cls");
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}

void init(void) {
	// 맵 초기화
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
	}

	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';
		}
	}

	map[0][5][5] = 'o';

	// Base (B) - 플레이어(파란색)와 AI(빨간색)
	// 플레이어 Base (좌측 하단)
	map[0][MAP_HEIGHT - 2][1] = 'B';
	map[0][MAP_HEIGHT - 2][2] = 'B';
	map[0][MAP_HEIGHT - 3][1] = 'B';
	map[0][MAP_HEIGHT - 3][2] = 'B';

	// AI Base (우측 상단)
	map[0][1][MAP_WIDTH - 3] = 'B';
	map[0][2][MAP_WIDTH - 3] = 'B';
	map[0][1][MAP_WIDTH - 4] = 'B';
	map[0][2][MAP_WIDTH - 4] = 'B';

	// Harvester (H) - 플레이어(파란색)와 AI(빨간색)
	map[1][MAP_HEIGHT - 4][1] = 'H';  // 플레이어
	map[1][3][MAP_WIDTH - 3] = 'H';   // AI

	// Spice (S) - 스파이스 매장지
	map[0][MAP_HEIGHT - 6][4] = '5';  // 좌측 하단
	map[0][4][MAP_WIDTH - 6] = '5';   // 우측 상단

	// Plate (P) - 장판
	map[0][MAP_HEIGHT - 2][4] = 'P';
	map[0][MAP_HEIGHT - 3][4] = 'P';
	map[0][MAP_HEIGHT - 2][5] = 'P';
	map[0][MAP_HEIGHT - 3][5] = 'P';

	map[0][1][MAP_WIDTH - 5] = 'P';
	map[0][2][MAP_WIDTH - 5] = 'P';
	map[0][1][MAP_WIDTH - 6] = 'P';
	map[0][2][MAP_WIDTH - 6] = 'P';

	// Rock (R)
	map[0][5][5] = 'R';
	map[0][6][3] = 'R';
	map[0][6][7] = 'R';

	// Sandworm (W)
	map[1][3][8] = 'W';
	map[1][10][13] = 'W';

	// layer 1(map[1])의 나머지 부분 -1로 초기화
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (map[1][i][j] == 0) {
				map[1][i][j] = -1;
			}
		}
	}
}


// (가능하다면) 지정한 방향으로 커서 이동
void cursor_move(DIRECTION dir) {
	static clock_t last_time = 0;   // 마지막 입력 시간
	static DIRECTION last_dir = d_stay; // 마지막 입력 방향

	POSITION curr = cursor.current;
	POSITION new_pos = pmove(curr, dir);

	// validation check
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 &&
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

		// 현재 시간과 이전 시간의 차이 계산
		clock_t current_time = clock();
		double time_diff = (double)(current_time - last_time) / CLOCKS_PER_SEC;

		if (dir == last_dir && time_diff < 0.5) {
			// 더블클릭으로 추가 이동 처리
			new_pos = pmove(new_pos, dir);

			// 다시 유효성 검사
			if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 &&
				1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {
				cursor.previous = cursor.current;
				cursor.current = new_pos;
			}
		}
		else {
			// 일반 이동
			cursor.previous = cursor.current;
			cursor.current = new_pos;
		}

		// 마지막 입력 정보 업데이트
		last_dir = dir;
		last_time = current_time;
	}
}



/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright로 목적지 설정
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}

	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj.pos;  // 제자리
	}
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// 아직 시간이 안 됐음
		return;
	}

	// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.move_period;
}

// engine.c

void select_object() {
	char obj = map[0][cursor.current.row][cursor.current.column];

	if (obj != ' ') { // 빈 칸이 아닌 경우
		selected_object = obj;
		switch (obj) {
		case 'B':
			display_status("Base selected: Main headquarters");
			display_commands("Commands: Build, Repair");
			display_system_message("Base selected");
			break;
		case 'R':
			display_status("Rock selected: Impassable terrain");
			display_commands("Commands: None");
			display_system_message("Rock selected");
			break;
		case 'S':
			display_status("Spice field selected: Harvestable resource");
			display_commands("Commands: Harvest");
			display_system_message("Spice field selected");
			break;
		case 'H':
			display_status("Harvester selected: Resource collector");
			display_commands("Commands: Move, Collect");
			display_system_message("Harvester selected");
			break;
		case 'P':
			display_status("Plate selected: Buildable foundation");
			display_commands("Commands: Build structure");
			display_system_message("Plate selected");
			break;
		case 'W':
			display_status("Sandworm selected: Dangerous entity");
			display_commands("Commands: Avoid");
			display_system_message("Sandworm selected");
			break;
		default:
			display_status("Unknown object selected");
			display_commands("Commands: None");
			display_system_message("Unknown object selected");
		}
	}
	else {
		// 빈 칸 선택 시
		display_status("Empty tile selected");
		display_commands("Commands: None");
		display_system_message("Empty tile selected");
	}
}


void cancel_selection() {
	selected_object = '\0';
	display_status("No object selected");
	display_commands("Commands: None");
	display_system_message("Selection cancelled");
}


void move_sandworm() {
	POSITION sandworm_pos = find_sandworm(); // 샌드웜 위치를 찾음
	POSITION nearest_unit = find_nearest_unit(sandworm_pos); // 가장 가까운 유닛 찾기

	// 이동 방향 계산
	DIRECTION dir = calculate_direction(sandworm_pos, nearest_unit);

	// 이동
	POSITION new_pos = pmove(sandworm_pos, dir);

	// 유닛 충돌 확인
	char target = map[0][new_pos.row][new_pos.column];
	if (target == 'H' || target == 'S' || target == 'B') { // 유닛이나 건물
		printf("샌드웜이 %c를 잡아먹었습니다!\n", target);
		map[0][new_pos.row][new_pos.column] = 'W'; // 샌드웜 이동
	}
	else {
		// 이동만 수행
		map[0][sandworm_pos.row][sandworm_pos.column] = ' ';
		map[0][new_pos.row][new_pos.column] = 'W';
	}
}

void sandworm_excrete_spice() {
	POSITION sandworm_pos = find_sandworm();
	POSITION random_pos = get_random_position();

	// 스파이스 생성
	if (map[0][random_pos.row][random_pos.column] == ' ') {
		map[0][random_pos.row][random_pos.column] = 'S'; // 스파이스 생성
		printf("샌드웜이 스파이스를 생성했습니다! 위치: (%d, %d)\n", random_pos.row, random_pos.column);
	}
}

void update_sandworm_size(char action) {
	if (action == 'eat') {
		sandworm_size++;
		printf("샌드웜의 크기가 증가했습니다! 현재 크기: %d\n", sandworm_size);
	}
	else if (action == "excrete") {
		if (sandworm_size > 1) sandworm_size--;
		printf("샌드웜의 크기가 감소했습니다! 현재 크기: %d\n", sandworm_size);
	}
}

void generate_sandstorm() {
	POSITION random_pos = get_random_position();

	if (map[1][random_pos.row][random_pos.column] == -1) {
		map[1][random_pos.row][random_pos.column] = 'F'; // 모래 폭풍 추가
		printf("모래 폭풍이 생성되었습니다! 위치: (%d, %d)\n", random_pos.row, random_pos.column);
	}
}

void move_sandstorm() {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (map[1][i][j] == 'F') {
				POSITION new_pos = get_random_adjacent_position(i, j);
				map[1][i][j] = -1; // 이전 위치 제거
				map[1][new_pos.row][new_pos.column] = 'F'; // 새로운 위치
				printf("모래 폭풍이 이동했습니다! 새로운 위치: (%d, %d)\n", new_pos.row, new_pos.column);
			}
		}
	}
}

void produce_unit(char unit_type) {
	if (selected_object == 'B') { // Base 선택 여부 확인
		if (unit_type == 'H') { // 하베스터 생성
			if (resource.spice >= 5 && resource.population < resource.population_max) {
				resource.spice -= 5;
				resource.population++;
				display_system_message("A new harvester is ready!");
				// 하베스터를 Base 근처에 배치
				map[0][cursor.current.row + 1][cursor.current.column] = 'H';
			}
			else {
				display_system_message("Not enough spice or population full");
			}
		}
	}
}
