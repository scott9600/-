#include <iostream>
#include <string>

// 뱀 게임 헤더 파일
#include "SnakeGame.hpp"

// 윈도우 및 리눅스 호환성을 위한 헤더 파일
#ifdef _WIN32
#include <Windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#endif

int kbhitAll()
{
#ifdef _WIN32
	return _kbhit();
#else
	struct termios oldattr, newattr;
	int ch, oldf;
	tcgetattr(STDIN_FILENO, &oldattr);
	newattr = oldattr;
	newattr.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
	if (EOF != ch) {
		ungetc(ch, stdin);
		return 1;
	}
	return 0;
#endif
}

int getchAll()
{
#ifdef _WIN32
	return _getch();
#else
	struct termios oldattr, newattr;
	int ch;
	tcgetattr(STDIN_FILENO, &oldattr);
	newattr = oldattr;
	newattr.c_lflag &= ~(ICANON | ECHO);
	newattr.c_cc[VMIN] = 1;
	newattr.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
	return ch;
#endif
}

void sleepAll(int milliseconds)
{
#ifdef _WIN32
	Sleep(milliseconds);
#else
	usleep(milliseconds * 1000);
#endif
}

using namespace std;

int main()
{
	SnakeGame snakeGame;

	// 갱신 속도 설정
	unsigned int speed = 125;
	
	while (true) {
		// 충돌 발생 체크
		if (snakeGame.checkCollision()) {
			break;
		}

		// 먹이 먹으면 먹이 생성 및 뱀 길이 추가
		// 게임의 재미를 위해 먹이를 먹을 때마다 갱신 속도 증가
		if (snakeGame.checkEatFeed())  {
			snakeGame.addSnake();
			snakeGame.createFeed();
			speed -= 5;
		}

		// 방향키로 방향 제어
		if (kbhitAll()) {
			int key = getchAll();
#ifdef _WIN32
			if (key == 0 || key == 224) {
				switch (getchAll()) {
					case 72: snakeGame.setDirection(UP); break;
					case 80: snakeGame.setDirection(DOWN); break;
					case 75: snakeGame.setDirection(LEFT); break;
					case 77: snakeGame.setDirection(RIGHT); break;
					default: break;
				}
			}
#else
			if (key == 0 || key == 27) {
				if (getchAll() == 91) {
					switch (getchAll()) {
						case 65: snakeGame.setDirection(UP); break;
						case 66: snakeGame.setDirection(DOWN); break;
						case 68: snakeGame.setDirection(LEFT); break;
						case 67: snakeGame.setDirection(RIGHT); break;
						default: break;
					}
				}
			}
#endif
		}

		// 뱀 이동 및 맵 출력
		snakeGame.moveSnake();
		snakeGame.printMap();
		
		// 갱신 속도 조절
		sleepAll(speed);
	}

	return 0;
}
