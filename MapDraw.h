#pragma once
#include "Mecro.h"

class MapDraw
{
public:
	void static DrawMap(int width, int height);
	void static DrawBox(int startX, int startY, int width, int height);
	void static ShowString(string str, int x, int y);
	void static DrawPoint(string str, int x, int y);
	void static ErasePoint(int x, int y, int width, int height);
	void static Gotoxy(int x, int y);
};