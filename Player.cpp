#include "Player.h"

Player::Player(int undoCount, string stone, string cursor, Stone coord, int width, int height, string name, string history)
{
	m_iUndoCount = undoCount;
	ChangeStoneMark(stone);
	ChangeCursorMark(cursor);
	m_sCursorCoord = coord;
	m_iMapWidht = width;
	m_iMapHeight = height;
	SetPlayerName(name);
	ConvertStringToList(history);
}

void Player::ConvertStringToList(string history)
{
	if (!history.empty())
	{
		int start = 0;
		int spaceIndex = history.find(" ", start);
		string strCoord = history.substr(start, spaceIndex - start);

		while (spaceIndex != -1)
		{
			int commaIndex = strCoord.find(",");
			string strX = strCoord.substr(0, commaIndex);
			string strY = strCoord.substr(commaIndex + 1, strCoord.length() - commaIndex);
			m_listHistory.push_back({ stoi(strX), stoi(strY) });

			start = spaceIndex + 1;
			spaceIndex = history.find(" ", start);
			strCoord = history.substr(start, spaceIndex - start);
		}
	}
}

INPUTKEY Player::Playing(Player* opponent)
{
	while (true)
	{
		MapDraw::DrawPoint(m_strCursor, m_sCursorCoord.m_iX, m_sCursorCoord.m_iY);

		char input = getch();
		switch (input)
		{
		case INPUTKEY_LEFT:
			if (m_sCursorCoord.m_iX > 0)
			{
				MovePoint(opponent);
				m_sCursorCoord.m_iX--;
			}
			break;
		case INPUTKEY_RIGHT:
			if (m_sCursorCoord.m_iX < m_iMapWidht)
			{
				MovePoint(opponent);
				m_sCursorCoord.m_iX++;
			}
			break;
		case INPUTKEY_UP:
			if (m_sCursorCoord.m_iY > 0)
			{
				MovePoint(opponent);
				m_sCursorCoord.m_iY--;
			}
			break;
		case INPUTKEY_DOWN:
			if (m_sCursorCoord.m_iY < m_iMapHeight - 1)
			{
				MovePoint(opponent);
				m_sCursorCoord.m_iY++;
			}
			break;
		case INPUTKEY_ENTER:
			if (CanPlayGo(m_sCursorCoord) && opponent->CanPlayGo(m_sCursorCoord))
			{
				DrawStone(m_sCursorCoord);
				m_listHistory.push_back(m_sCursorCoord);
				return INPUTKEY_ENTER;
			}
			break;
		case INPUTKEY_UNDO:
			if (CheckUndoCount() && opponent->CheckStoneList())
			{
				m_iUndoCount--;
				MovePoint(opponent);
				opponent->Undo();
				return INPUTKEY_UNDO;
			}
			break;
		case INPUTKEY_OPTION:
			return INPUTKEY_OPTION;
		case INPUTKEY_ESC:
			return INPUTKEY_ESC;
		}
	}
}

void Player::MovePoint(Player* opponent)
{
	bool isMyStone = CheckStoneCoordinate(m_sCursorCoord);
	bool isOpponentStone = opponent->CheckStoneCoordinate(m_sCursorCoord);

	if (isMyStone && !isOpponentStone)
	{
		DrawStone(m_sCursorCoord);
	}
	else if (!isMyStone && isOpponentStone)
	{
		opponent->DrawStone(m_sCursorCoord);
	}
	else
	{
		MapDraw::ErasePoint(m_sCursorCoord.m_iX, m_sCursorCoord.m_iY, m_iMapWidht, m_iMapHeight);
	}
}

bool Player::CheckStoneCoordinate(Stone coord)
{
	return FindStone(coord.m_iX, coord.m_iY);
}

void Player::DrawStone(Stone coord)
{
	MapDraw::DrawPoint(m_strStone, coord.m_iX, coord.m_iY);
}

bool Player::CanPlayGo(Stone coord)
{
	for (list<Stone>::iterator iter = m_listHistory.begin(); iter != m_listHistory.end(); iter++)
	{
		if (iter->m_iX == coord.m_iX && iter->m_iY == coord.m_iY)
		{
			return false;
		}
	}

	return true;
}

void Player::Undo()
{
	Stone tempCoord = m_listHistory.back();
	MapDraw::ErasePoint(tempCoord.m_iX, tempCoord.m_iY, m_iMapWidht, m_iMapHeight);
	m_listHistory.pop_back();
}

bool Player::FindStone(int x, int y)
{
	for (list<Stone>::iterator iter = m_listHistory.begin(); iter != m_listHistory.end(); iter++)
	{
		if (iter->m_iX == x && iter->m_iY == y)
		{
			return true;
		}
	}

	return false;
}

bool Player::CheckVictory()
{
	int leftX = m_sCursorCoord.m_iX + GetDirX(DIR_LEFT);
	int rightX = m_sCursorCoord.m_iX + GetDirX(DIR_RIGHT);
	int upY = m_sCursorCoord.m_iY + GetDirY(DIR_UP);
	int downY = m_sCursorCoord.m_iY + GetDirY(DIR_DOWN);

	int omok = CalculateStones(m_sCursorCoord.m_iX, upY, DIR_UP) + CalculateStones(m_sCursorCoord.m_iX, downY, DIR_DOWN);
	if (omok + 1 == VICTORY)
	{
		return true;
	}

	omok = CalculateStones(leftX, m_sCursorCoord.m_iY, DIR_LEFT) + CalculateStones(rightX, m_sCursorCoord.m_iY, DIR_RIGHT);
	if (omok + 1 == VICTORY)
	{
		return true;
	}

	omok = CalculateStones(leftX, upY, DIR_UPLEFT) + CalculateStones(rightX, downY, DIR_DOWNRIGHT);
	if (omok + 1 == VICTORY)
	{
		return true;
	}

	omok = CalculateStones(leftX, downY, DIR_DOWNLEFT) + CalculateStones(rightX, upY, DIR_UPRIGHT);
	if (omok + 1 == VICTORY)
	{
		return true;
	}

	return false;
}

int Player::CalculateStones(int x, int y, DIR dir)
{
	if (FindStone(x, y))
	{
		return 1 + CalculateStones(x + GetDirX(dir), y + GetDirY(dir), dir);
	}
	else
	{
		return 0;
	}
}

int Player::GetDirX(DIR dir)
{
	switch (dir)
	{
	case DIR_UP:
	case DIR_DOWN:
		return 0;
	case DIR_LEFT:
	case DIR_UPLEFT:
	case DIR_DOWNLEFT:
		return -1;
	case DIR_RIGHT:
	case DIR_UPRIGHT:
	case DIR_DOWNRIGHT:
		return 1;
	}
}

int Player::GetDirY(DIR dir)
{
	switch (dir)
	{
	case DIR_LEFT:
	case DIR_RIGHT:
		return 0;
	case DIR_UP:
	case DIR_UPLEFT:
	case DIR_UPRIGHT:
		return -1;
	case DIR_DOWN:
	case DIR_DOWNLEFT:
	case DIR_DOWNRIGHT:
		return 1;
	}
}

void Player::DrawHistory()
{
	for (list<Stone>::iterator iter = m_listHistory.begin(); iter != m_listHistory.end(); iter++)
	{
		MapDraw::DrawPoint(m_strStone, iter->m_iX, iter->m_iY);
	}
}

void Player::Reset(int undoCount, Stone coord, int width, int height)
{
	m_iUndoCount = undoCount;
	m_sCursorCoord = coord;
	m_listHistory.clear();
	m_iMapWidht = width;
	m_iMapHeight = height;
}

string Player::GetHistoryString()
{
	string returnValue;
	for (list<Stone>::iterator iter = m_listHistory.begin(); iter != m_listHistory.end(); iter++)
	{
		returnValue += to_string(iter->m_iX) + "," + to_string(iter->m_iY) + " ";
	}

	return returnValue;
}

string Player::GetReplayHistoryString()
{
	string returnValue;
	for (list<Stone>::iterator iter = m_listReplayHistory.begin(); iter != m_listReplayHistory.end(); iter++)
	{
		returnValue += to_string(iter->m_iX) + "," + to_string(iter->m_iY) + " ";
	}

	return returnValue;
}