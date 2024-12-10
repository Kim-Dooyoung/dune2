/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"


#define _CRT_SECURE_NO_WARNINGS // 경고 비활성화
#include <string.h> // 헤더 포함

#define STATUS_X (MAP_WIDTH + 2)
#define STATUS_Y 2

#define COMMANDS_X (MAP_WIDTH + 2)
#define COMMANDS_Y (MAP_HEIGHT + 2)

#define MAX_SYSTEM_MESSAGES 5
char system_messages[MAX_SYSTEM_MESSAGES][50] = { { 0 } };
int message_count = 0;


// 출력할 내용들의 좌상단(topleft) 좌표
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
void display_system_message(const char* message);
void display_commands(const char* commands);
void display_status(const char* status);
void init_buildings(BUILDING buildings[]);
void init_units(UNIT units[]);

void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor)
{
	display_resource(resource);
	display_map(map);
	display_cursor(cursor);
	display_system_message("All systems operational.");
	display_commands("Move, Attack, Defend");
	display_status("Unit: Worker, HP: 100/100");
}

void display_resource(RESOURCE resource) {
	set_color(COLOR_RESOURCE);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

// subfunction of draw_map()
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) {
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}

void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf);

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {
				POSITION pos = { i, j };
				int color = COLOR_DEFAULT;

				switch (backbuf[i][j]) {
				case 'B':
					color = (i < MAP_HEIGHT / 2) ? COLOR_RED : COLOR_BLUE; // AI(빨간색), 플레이어(파란색)
					break;
				case 'H':
					color = (i < MAP_HEIGHT / 2) ? COLOR_RED : COLOR_BLUE; // AI(빨간색), 플레이어(파란색)
					break;
				case 'S': color = COLOR_ORANGE; break; // Spice
				case 'P': color = COLOR_BLACK; break;  // Plate
				case 'R': color = COLOR_GRAY; break;   // Rock
				case 'W': color = COLOR_YELLOW; break; // Sandworm
				case ' ': color = COLOR_SAND; break;   // 빈 칸
				}

				printc(padd(map_pos, pos), backbuf[i][j], color);
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}




// frontbuf[][]에서 커서 위치의 문자를 색만 바꿔서 그대로 다시 출력
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	char ch = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), ch, COLOR_DEFAULT);

	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}

void display_system_message(const char* message) {
	if (message_count < MAX_SYSTEM_MESSAGES) {
		strncpy_s(system_messages[message_count], sizeof(system_messages[message_count]), message, _TRUNCATE);
		message_count++;
	}
	else {
		for (int i = 1; i < MAX_SYSTEM_MESSAGES; i++) {
			strncpy_s(system_messages[i - 1], sizeof(system_messages[i - 1]), system_messages[i], _TRUNCATE);
		}
		strncpy_s(system_messages[MAX_SYSTEM_MESSAGES - 1], sizeof(system_messages[MAX_SYSTEM_MESSAGES - 1]), message, _TRUNCATE);
	}

	// 화면에 출력
	for (int i = 0; i < message_count; i++) {
		POSITION message_pos = { MAP_HEIGHT + 1 + i, 0 };
		gotoxy(message_pos);
		printf("SYSTEM: %-50s", system_messages[i]);
	}
}

void display_commands(const char* commands) {
	POSITION command_pos = { MAP_HEIGHT + 1, MAP_WIDTH + 4 }; // 명령창의 고정 좌표 (오른쪽 하단)
	gotoxy(command_pos);                                      // 커서를 명령창 위치로 이동
	printf("COMMANDS: %-50s", commands);                      // 명령창에 메시지 출력 (50칸 고정)
}



void display_status(const char* status) {
	POSITION status_pos = { 0, MAP_WIDTH + 4 }; // 상태창의 고정 좌표 (오른쪽 상단)
	gotoxy(status_pos);                        // 커서를 상태창 위치로 이동
	printf("STATUS: %-50s", status);           // 상태창에 메시지 출력 (50칸 고정)
}


void init_buildings(BUILDING buildings[]) {
	buildings[0] = (BUILDING){ "Base", 0, 50, 1 };          // 본진
	buildings[1] = (BUILDING){ "Plate", 0, 0, 0 };          // 장판
	buildings[2] = (BUILDING){ "Dormitory", 2, 10, 0 };     // 숙소
	buildings[3] = (BUILDING){ "Garage", 2, 10, 0 };        // 창고
	buildings[4] = (BUILDING){ "Barracks", 4, 20, 1 };      // 병영
	buildings[5] = (BUILDING){ "Shelter", 5, 30, 1 };       // 은신처
	buildings[6] = (BUILDING){ "Arena", 4, 20, 1 };         // 투기장
	buildings[7] = (BUILDING){ "Factory", 5, 30, 1 };       // 공장
}

void init_units(UNIT units[]) {
	units[0] = (UNIT){ "Harvester", 5, 0, 0, 2000, 70 };    // 하베스터
	units[1] = (UNIT){ "Soldier", 5, 15, 5, 400, 25 };      // 솔저
	units[2] = (UNIT){ "Fremen", 5, 25, 8, 1000, 30 };      // 프레멘
	units[3] = (UNIT){ "Fighter", 6, 20, 10, 800, 30 };     // 투사
	units[4] = (UNIT){ "Heavy Tank", 10, 40, 20, 1200, 60 }; // 중전차
	units[5] = (UNIT){ "Sandworm", 0, 10000, 0, 2500, 10000 }; // 샌드웜
}

