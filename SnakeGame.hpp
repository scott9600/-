#pragma once

#include <iostream>
#include <string>
#include <list>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cassert>

// 윈도우 및 리눅스 호환성을 위한 헤더 파일
#ifdef _WIN32
#include <Windows.h>
#else

#endif

// 맵 크기 설정
#define MAP_SIZE_X 81
#define MAP_SIZE_Y 21

// 콘솔에 표시되는 모양 설정
#define SHAPE_WALL '#'
#define SHAPE_SNAKE '@'
#define SHAPE_FEED 'F'

// 뱀 이동 방향 상수
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

class Point
{
public:
	int x;
	int y;

	Point()
	{
		this->x = 0;
		this->y = 0;
	}

	Point(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	bool operator==(const Point& p)
	{
		return this->x == p.x && this->y == p.y;
	}

	bool operator!=(const Point& p)
	{
		return !(*this == p);
	}

	Point operator+(const Point& p)
	{
		return Point(this->x + p.x, this->y + p.y);
	}

	Point operator-(const Point& p)
	{
		return Point(this->x - p.x, this->y - p.y);
	}
};

class SnakeGame
{
private:
	char map[MAP_SIZE_Y][MAP_SIZE_X + 1];
	std::list<Point> snake;
	std::size_t length;
	int direction;
	Point feedPos;

	void gotoConsoleCursor(int x, int y);

public:
	SnakeGame();

	void addSnake();
	void moveSnake();
	void setDirection(int direction);

	bool checkEatFeed();
	bool checkCollision();

	void createFeed();
	void printMap();
};

//==================================================
// Private
//==================================================

// 콘솔의 커서 좌표 이동
void SnakeGame::gotoConsoleCursor(int x, int y)
{
#ifdef _WIN32
	COORD cursor;
	cursor.X = x;
	cursor.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
#else
	printf("\033[%dd\033[%dG", y, x);
#endif
}

//==================================================
// Public
//==================================================

// 생성자
SnakeGame::SnakeGame()
{
	// 맵 공백으로 초기화
	for (int y = 0; y < MAP_SIZE_Y; y++) {
		for (int x = 0; x < MAP_SIZE_X; x++) {
			this->map[y][x] = ' ';
		}
	}

	// 맵 틀 그리기
	for (int x = 0; x < MAP_SIZE_X; x++) {
		this->map[0][x] = this->map[MAP_SIZE_Y - 1][x] = SHAPE_WALL;
	}

	for (int y = 0; y < MAP_SIZE_Y; y++) {
		this->map[y][0] = this->map[y][MAP_SIZE_X - 1] = SHAPE_WALL;
	}

	// 맵 NULL 추가 (게임과는 관련이 없으며 오직 콘솔에 출력하기 위한 용도)
	for (int y = 0; y < MAP_SIZE_Y; y++) {
		this->map[y][MAP_SIZE_X] = NULL;
	}

	// 뱀 초기 길이 설정
	this->length = 5;

	// 뱀 초기 위치 설정 (중앙에서 시작)
	for (std::size_t i = 0; i < this->length; i++) {
		int x = MAP_SIZE_X / 2 - this->length / 2 + i;
		int y = MAP_SIZE_Y / 2;

		this->snake.push_back(Point(x, y));
		this->map[y][x] = SHAPE_SNAKE;
	}

	// 뱀 초기 방향 설정 (왼쪽으로 시작)
	this->direction = LEFT;

	// 랜덤 시드 설정
	srand((unsigned int)time(NULL));

	// 먹이 생성
	createFeed();
}

// 뱀 길이 1 증가
void SnakeGame::addSnake()
{
	Point snakeTail = this->snake.back();
	Point addTail;
	
	// 추가할 수 있는 위치 계산
	if (this->map[snakeTail.y][snakeTail.x - 1] == ' ') {
		addTail.x = snakeTail.x - 1;
		addTail.y = snakeTail.y;
	}
	else if (this->map[snakeTail.y][snakeTail.x + 1] == ' ') {
		addTail.x = snakeTail.x + 1;
		addTail.y = snakeTail.y;
	}
	else if (this->map[snakeTail.y - 1][snakeTail.x] == ' ') {
		addTail.x = snakeTail.x;
		addTail.y = snakeTail.y - 1;
	}
	else if (this->map[snakeTail.y + 1][snakeTail.x] == ' ') {
		addTail.x = snakeTail.x;
		addTail.y = snakeTail.y + 1;
	}

	// 뱀 꼬리 추가
	this->snake.push_back(addTail);
	this->length++;
	this->map[addTail.y][addTail.x] = SHAPE_SNAKE;
}

// 뱀 이동
void SnakeGame::moveSnake()
{
	Point dp;

	// 좌표 변화량 설정
	switch (this->direction) {
		case UP:
			dp.x = 0;
			dp.y = -1;
			break;

		case DOWN:
			dp.x = 0;
			dp.y = 1;
			break;

		case LEFT:
			dp.x = -1;
			dp.y = 0;
			break;

		case RIGHT:
			dp.x = 1;
			dp.y = 0;
			break;
	}

	// 뱀 머리 추가
	Point snakeHead = this->snake.front() + dp;
	this->snake.push_front(snakeHead);
	this->map[snakeHead.y][snakeHead.x] = SHAPE_SNAKE;

	// 뱀 꼬리 제거
	Point snakeTail = this->snake.back();
	this->snake.pop_back();
	this->map[snakeTail.y][snakeTail.x] = ' ';
}

// 뱀 이동 방향 설정
void SnakeGame::setDirection(int direction)
{
	assert(direction == UP || direction == DOWN || direction == LEFT || direction == RIGHT);
	this->direction = direction;
}

// 먹이를 먹었으면 true, 아니면 false 반환
bool SnakeGame::checkEatFeed()
{
	return this->snake.front() == this->feedPos;
}

// 벽이나 자기 자신에게 부딪쳤으면 true, 아니면 false 반환
bool SnakeGame::checkCollision()
{
	Point snakeHead = this->snake.front();

	// 벽에 부딪쳤는지 체크
	if (snakeHead.x <= 0 || snakeHead.x >= MAP_SIZE_X - 1) {
		return true;
	}

	if (snakeHead.y <= 0 || snakeHead.y >= MAP_SIZE_Y - 1) {
		return true;
	}

	// 자기 자신에게 부딪쳤는지 체크
	std::list<Point>::iterator it = this->snake.begin();
	it++;

	while (it != this->snake.end()) {
		if (*it == snakeHead) {
			return true;
		}

		it++;
	}

	return false;
}

// 먹이를 랜덤한 위치에 생성
void SnakeGame::createFeed()
{
	int x;
	int y;

	// 뱀과 충돌하지 않도록 좌표 생성
	do
	{
		x = rand() % (MAP_SIZE_X - 1) + 1;
		y = rand() % (MAP_SIZE_Y - 1) + 1;
	} while (this->map[y][x] != ' ');

	// 먹이 설정
	feedPos = Point(x, y);
	this->map[y][x] = SHAPE_FEED;
}

// 맵 출력
void SnakeGame::printMap()
{
	// 덮어쓰기를 위한 콘솔의 커서 좌표 이동
	gotoConsoleCursor(0, 0);
	
	// 맵 출력
	for (int y = 0; y < MAP_SIZE_Y; y++) {
		puts(this->map[y]);
	}
}
