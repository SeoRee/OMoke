#pragma once
#include "Mecro.h"
#include "MapDraw.h"
#include "Enums.h"
#include "Player.h"

class Replayer
{
private:
	string m_strName;
	string m_strStone;
	list<Stone> m_listReplay;

	void ConvertStringToList(string replayStr);

public:
	Replayer(string name, string stone, string replayStr);
	void ReplayStone();
	inline string GetPlayerName()
	{
		return m_strName;
	}
};

