#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

int kbhit(void)
{
  struct termios oldt, newt; 
  int ch;
  int oldf;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
  if(EOF != ch)
  {
    ungetc(ch, stdin);
    return 1;
  }
  return 0;
}

int getch(void)
{
    int ch;
    struct termios buf, save;
    tcgetattr(0,&save);
    buf = save;
    buf.c_lflag &= ~(ICANON|ECHO);
    buf.c_cc[VMIN] = 1;
    buf.c_cc[VTIME] = 0;
    tcsetattr(0, TCSAFLUSH, &buf);
    ch = getchar();
    tcsetattr(0, TCSAFLUSH, &save);
    return ch;
}

int main()
{
	SnakeGame snakeGame;
	snakeGame.printMap();
	int key;
	while (1)
	{
		if (snakeGame.checkCollision()) // 충돌 발생 break
			break;
		if (snakeGame.checkEatFeed()) // 먹이 먹으면 먹이 생성 및 뱀 길이 추가
		{
			snakeGame.addSnake();
			snakeGame.createFeed();
		}
		if (kbhit())
		{
			key = getch();
			if (key == 224 || key == 0)
			{
				key = getch();
				switch (key)
				{
				case 72: // ↑
					snakeGame.setDirection(UP);
					break;
				case 75: // ←
					snakeGame.setDirection(LEFT);
					break;
				case 77: // →
					snakeGame.setDirection(RIGHT);
					break;
				case 80: // ↓
					snakeGame.setDirection(DOWN);
					break;
				default:
					break;
				}
			}
		}
		snakeGame.moveSnake();
		snakeGame.printMap();
		sleep(100);
	}
	return 0;
}
