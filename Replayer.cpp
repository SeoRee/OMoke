#include "Replayer.h"

Replayer::Replayer(string name, string stone, string replayStr)
{
	m_strName = name;
	m_strStone = stone;
	ConvertStringToList(replayStr);
}

void Replayer::ConvertStringToList(string replayStr)
{
	if (!replayStr.empty())
	{
		int start = 0;
		int spaceIndex = replayStr.find(" ", start);
		string strCoord = replayStr.substr(start, spaceIndex - start);

		while (spaceIndex != -1)
		{
			int commaIndex = strCoord.find(",");
			string strX = strCoord.substr(0, commaIndex);
			string strY = strCoord.substr(commaIndex + 1, strCoord.length() - commaIndex);
			m_listReplay.push_back({ stoi(strX), stoi(strY) });

			start = spaceIndex + 1;
			spaceIndex = replayStr.find(" ", start);
			strCoord = replayStr.substr(start, spaceIndex - start);
		}
	}
}

void Replayer::ReplayStone()
{
	Stone coord = m_listReplay.front();
	MapDraw::DrawPoint(m_strStone, coord.m_iX, coord.m_iY);

	m_listReplay.pop_front();
}
