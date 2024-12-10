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
void select_object();       // select_object �Լ� ����
void cancel_selection();    // cancel_selection �Լ� ����
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

char selected_object = '\0'; // ���� ���õ� ������Ʈ ����


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

POSITION find_sandworm() {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (map[1][i][j] == 'W') { // ������� ã���� ��ġ ��ȯ
				return (POSITION) { i, j };
			}
		}
	}
	// ������� ������ �⺻�� ��ȯ
	return (POSITION) { -1, -1 };
}

POSITION find_nearest_unit(POSITION sandworm_pos) {
	int min_distance = INT_MAX;
	POSITION nearest_unit = { -1, -1 };

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (map[0][i][j] == 'H' || map[0][i][j] == 'S' || map[0][i][j] == 'B') { // �����̳� �ǹ�
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

	// �� ��� �ȿ� �ִ��� Ȯ��
	if (new_pos.row >= 0 && new_pos.row < MAP_HEIGHT &&
		new_pos.column >= 0 && new_pos.column < MAP_WIDTH) {
		return new_pos;
	}

	// ��踦 ����� ���� ��ġ ��ȯ
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
			case ' ': // �����̽��� �Է�
				select_object();
				break;
			case 'X': // ���� ���
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
	// �� �ʱ�ȭ
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

	// Base (B) - �÷��̾�(�Ķ���)�� AI(������)
	// �÷��̾� Base (���� �ϴ�)
	map[0][MAP_HEIGHT - 2][1] = 'B';
	map[0][MAP_HEIGHT - 2][2] = 'B';
	map[0][MAP_HEIGHT - 3][1] = 'B';
	map[0][MAP_HEIGHT - 3][2] = 'B';

	// AI Base (���� ���)
	map[0][1][MAP_WIDTH - 3] = 'B';
	map[0][2][MAP_WIDTH - 3] = 'B';
	map[0][1][MAP_WIDTH - 4] = 'B';
	map[0][2][MAP_WIDTH - 4] = 'B';

	// Harvester (H) - �÷��̾�(�Ķ���)�� AI(������)
	map[1][MAP_HEIGHT - 4][1] = 'H';  // �÷��̾�
	map[1][3][MAP_WIDTH - 3] = 'H';   // AI

	// Spice (S) - �����̽� ������
	map[0][MAP_HEIGHT - 6][4] = '5';  // ���� �ϴ�
	map[0][4][MAP_WIDTH - 6] = '5';   // ���� ���

	// Plate (P) - ����
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

	// layer 1(map[1])�� ������ �κ� -1�� �ʱ�ȭ
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (map[1][i][j] == 0) {
				map[1][i][j] = -1;
			}
		}
	}
}


// (�����ϴٸ�) ������ �������� Ŀ�� �̵�
void cursor_move(DIRECTION dir) {
	static clock_t last_time = 0;   // ������ �Է� �ð�
	static DIRECTION last_dir = d_stay; // ������ �Է� ����

	POSITION curr = cursor.current;
	POSITION new_pos = pmove(curr, dir);

	// validation check
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 &&
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

		// ���� �ð��� ���� �ð��� ���� ���
		clock_t current_time = clock();
		double time_diff = (double)(current_time - last_time) / CLOCKS_PER_SEC;

		if (dir == last_dir && time_diff < 0.5) {
			// ����Ŭ������ �߰� �̵� ó��
			new_pos = pmove(new_pos, dir);

			// �ٽ� ��ȿ�� �˻�
			if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 &&
				1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {
				cursor.previous = cursor.current;
				cursor.current = new_pos;
			}
		}
		else {
			// �Ϲ� �̵�
			cursor.previous = cursor.current;
			cursor.current = new_pos;
		}

		// ������ �Է� ���� ������Ʈ
		last_dir = dir;
		last_time = current_time;
	}
}



/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
	// ���� ��ġ�� �������� ���ؼ� �̵� ���� ����	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;

	// ������ ����. ������ �ܼ��� ���� �ڸ��� �պ�
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright�� ������ ����
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft�� ������ ����
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}

	// ������, ������ �Ÿ��� ���ؼ� �� �� �� ������ �̵�
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos�� ���� ����� �ʰ�, (������ ������)��ֹ��� �ε����� ������ ���� ��ġ�� �̵�
	// ������ �浹 �� �ƹ��͵� �� �ϴµ�, ���߿��� ��ֹ��� ���ذ��ų� ���� ������ �ϰų�... ���
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj.pos;  // ���ڸ�
	}
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// ���� �ð��� �� ����
		return;
	}

	// ������Ʈ(�ǹ�, ���� ��)�� layer1(map[1])�� ����
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.move_period;
}

// engine.c

void select_object() {
	char obj = map[0][cursor.current.row][cursor.current.column];

	if (obj != ' ') { // �� ĭ�� �ƴ� ���
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
		// �� ĭ ���� ��
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
	POSITION sandworm_pos = find_sandworm(); // ����� ��ġ�� ã��
	POSITION nearest_unit = find_nearest_unit(sandworm_pos); // ���� ����� ���� ã��

	// �̵� ���� ���
	DIRECTION dir = calculate_direction(sandworm_pos, nearest_unit);

	// �̵�
	POSITION new_pos = pmove(sandworm_pos, dir);

	// ���� �浹 Ȯ��
	char target = map[0][new_pos.row][new_pos.column];
	if (target == 'H' || target == 'S' || target == 'B') { // �����̳� �ǹ�
		printf("������� %c�� ��ƸԾ����ϴ�!\n", target);
		map[0][new_pos.row][new_pos.column] = 'W'; // ����� �̵�
	}
	else {
		// �̵��� ����
		map[0][sandworm_pos.row][sandworm_pos.column] = ' ';
		map[0][new_pos.row][new_pos.column] = 'W';
	}
}

void sandworm_excrete_spice() {
	POSITION sandworm_pos = find_sandworm();
	POSITION random_pos = get_random_position();

	// �����̽� ����
	if (map[0][random_pos.row][random_pos.column] == ' ') {
		map[0][random_pos.row][random_pos.column] = 'S'; // �����̽� ����
		printf("������� �����̽��� �����߽��ϴ�! ��ġ: (%d, %d)\n", random_pos.row, random_pos.column);
	}
}

void update_sandworm_size(char action) {
	if (action == 'eat') {
		sandworm_size++;
		printf("������� ũ�Ⱑ �����߽��ϴ�! ���� ũ��: %d\n", sandworm_size);
	}
	else if (action == "excrete") {
		if (sandworm_size > 1) sandworm_size--;
		printf("������� ũ�Ⱑ �����߽��ϴ�! ���� ũ��: %d\n", sandworm_size);
	}
}

void generate_sandstorm() {
	POSITION random_pos = get_random_position();

	if (map[1][random_pos.row][random_pos.column] == -1) {
		map[1][random_pos.row][random_pos.column] = 'F'; // �� ��ǳ �߰�
		printf("�� ��ǳ�� �����Ǿ����ϴ�! ��ġ: (%d, %d)\n", random_pos.row, random_pos.column);
	}
}

void move_sandstorm() {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (map[1][i][j] == 'F') {
				POSITION new_pos = get_random_adjacent_position(i, j);
				map[1][i][j] = -1; // ���� ��ġ ����
				map[1][new_pos.row][new_pos.column] = 'F'; // ���ο� ��ġ
				printf("�� ��ǳ�� �̵��߽��ϴ�! ���ο� ��ġ: (%d, %d)\n", new_pos.row, new_pos.column);
			}
		}
	}
}

void produce_unit(char unit_type) {
	if (selected_object == 'B') { // Base ���� ���� Ȯ��
		if (unit_type == 'H') { // �Ϻ����� ����
			if (resource.spice >= 5 && resource.population < resource.population_max) {
				resource.spice -= 5;
				resource.population++;
				display_system_message("A new harvester is ready!");
				// �Ϻ����͸� Base ��ó�� ��ġ
				map[0][cursor.current.row + 1][cursor.current.column] = 'H';
			}
			else {
				display_system_message("Not enough spice or population full");
			}
		}
	}
}
