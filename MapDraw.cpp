#include "MapDraw.h"


void MapDraw::DrawMap(int width, int height)
{
	Gotoxy(0, 0);
	for (int y = 0; y < height; y++)
	{
		if (y == 0)
		{
			cout << "¦£";
			for (int x = 0; x < width - 1; x++)
			{
				cout << "¦¨";
			}
			cout << "¦¤" << endl;
		}
		else if (y == height - 1)
		{
			cout << "¦¦";
			for (int x = 0; x < width - 1; x++)
			{
				cout << "¦ª";
			}
			cout << "¦¥" << endl;
		}
		else
		{
			cout << "¦§";
			for (int x = 0; x < width - 1; x++)
			{
				cout << "¦«";
			}
			cout << "¦©" << endl;
		}
	}
}

void MapDraw::DrawBox(int startX, int startY, int width, int height)
{
	for (int y = 0; y < height; y++)
	{
		Gotoxy(startX, startY + y);
		if (y == 0)
		{
			cout << "¦£";
			for (int x = 0; x < width - 1; x++)
			{
				cout << "¦¡";
			}
			cout << "¦¤" << endl;
		}
		else if (y == height - 1)
		{
			cout << "¦¦";
			for (int x = 0; x < width - 1; x++)
			{
				cout << "¦¡";
			}
			cout << "¦¥" << endl;
		}
		else
		{
			cout << "¦¢";
			for (int x = 0; x < width - 1; x++)
			{
				cout << "  ";
			}
			cout << "¦¢" << endl;
		}
	}
}

void MapDraw::ShowString(string str, int x, int y)
{
	x -= str.size() / 2;
	Gotoxy(x, y);
	cout << str;
}

void MapDraw::DrawPoint(string str, int x, int y)
{
	Gotoxy(x * 2, y);
	cout << str;
	Gotoxy(-1, -1);
}

void MapDraw::ErasePoint(int x, int y, int width, int height)
{
	Gotoxy(x * 2, y);
	if (y == 0)
	{
		if (x == 0)
		{
			cout << "¦£";
		}
		else if (x == width)
		{
			cout << "¦¤";
		}
		else
		{
			cout << "¦¨";
		}
	}
	else if (y == height - 1)
	{
		if (x == 0)
		{
			cout << "¦¦";
		}
		else if (x == width)
		{
			cout << "¦¥";
		}
		else
		{
			cout << "¦ª";
		}
	}
	else
	{
		if (x == 0)
		{
			cout << "¦§";
		}
		else if (x == width)
		{
			cout << "¦©";
		}
		else
		{
			cout << "¦«";
		}
	}
	Gotoxy(-1, -1);
}

void MapDraw::Gotoxy(int x, int y)
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}