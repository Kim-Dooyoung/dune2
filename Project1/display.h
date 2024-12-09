/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// 표시할 색상 정의. 대충 맞춰 뒀는데, 취향껏 추가하거나 변경하기
#define COLOR_DEFAULT	15
#define COLOR_CURSOR	112
#define COLOR_RESOURCE  112

#define COLOR_BLUE   9    // 플레이어 Base, Harvester
#define COLOR_RED    12   // AI Base, Harvester
#define COLOR_ORANGE 14   // Spice
#define COLOR_BLACK   0   // Plate
#define COLOR_GRAY    8   // Rock
#define COLOR_YELLOW 14   // Sandworm
#define COLOR_SAND   15   // 빈 칸




void display_commands(const char* commands);
void display_system_message(const char* message);
void display_status(const char* status);


// 지금은 자원, 맵, 커서만 표시
// 앞으로 화면에 표시할 내용들 여기에 추가하기
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor
);

#endif
