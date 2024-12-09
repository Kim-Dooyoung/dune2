/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// ǥ���� ���� ����. ���� ���� �״µ�, ���ⲯ �߰��ϰų� �����ϱ�
#define COLOR_DEFAULT	15
#define COLOR_CURSOR	112
#define COLOR_RESOURCE  112

#define COLOR_BLUE   9    // �÷��̾� Base, Harvester
#define COLOR_RED    12   // AI Base, Harvester
#define COLOR_ORANGE 14   // Spice
#define COLOR_BLACK   0   // Plate
#define COLOR_GRAY    8   // Rock
#define COLOR_YELLOW 14   // Sandworm
#define COLOR_SAND   15   // �� ĭ




void display_commands(const char* commands);
void display_system_message(const char* message);
void display_status(const char* status);


// ������ �ڿ�, ��, Ŀ���� ǥ��
// ������ ȭ�鿡 ǥ���� ����� ���⿡ �߰��ϱ�
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor
);

#endif
