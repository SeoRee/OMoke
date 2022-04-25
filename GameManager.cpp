#include "GameManager.h"

GameManager::GameManager()
{
	ifstream load;
	string loadStr;
	load.open("Data.txt");
	getline(load, loadStr);
	if (load.is_open() && !loadStr.empty())
	{
		m_ePlayMode = PLAYMODE_CONTINUE;
		m_bIsPlaying = false;
		m_iUndoCount = DEFAULT_UNDOCOUNT;

		//GameManager Initialize
		{
			m_iTurnCount = stoi(loadStr);
			getline(load, loadStr);
			int commaIndex = loadStr.find(",");
			string widthStr = loadStr.substr(0, commaIndex);
			string heightStr = loadStr.substr(commaIndex + 1, loadStr.length() - commaIndex);
			m_iWidth = stoi(widthStr);
			m_iHeight = stoi(heightStr);
			getline(load, loadStr);
			m_eCursor = (MARKLIST)stoi(loadStr);
			getline(load, loadStr);
			m_eStone = (MARKLIST)stoi(loadStr);
		}

		//Player Initialize
		{
			getline(load, loadStr);
			string p1_NameStr = loadStr;
			getline(load, loadStr);
			string p2_NameStr = loadStr;
			getline(load, loadStr);
			int commaIndex = loadStr.find(",");
			string p1_UndoStr = loadStr.substr(0, commaIndex);
			string p2_UndoStr = loadStr.substr(commaIndex + 1, loadStr.length() - commaIndex);
			getline(load, loadStr);
			string p1_HistoryStr = loadStr;
			getline(load, loadStr);
			string p2_HistoryStr = loadStr;

			m_cPlayer1 = Player(stoi(p1_UndoStr), GetStoneMarkForPlayer1(m_eStone), GetCursorMarkForPlayer1(m_eCursor), GetCenterCoordinate(), m_iWidth, m_iHeight, p1_NameStr, p1_HistoryStr);
			m_cPlayer2 = Player(stoi(p2_UndoStr), GetStoneMarkForPlayer2(m_eStone), GetCursorMarkForPlayer2(m_eCursor), GetCenterCoordinate(), m_iWidth, m_iHeight, p2_NameStr, p2_HistoryStr);
		}

	}
	else
	{
		m_ePlayMode = PLAYMODE_NEW;
		m_iWidth = MAPRANGE_WIDTH_MIN;
		m_iHeight = MAPRANGE_HEIGHT_MIN;
		m_iTurnCount = 1;
		m_iUndoCount = DEFAULT_UNDOCOUNT;
		m_bIsPlaying = false;
		m_eStone = MARKLIST_FIRST;
		m_eCursor = MARKLIST_FIRST;
		m_cPlayer1 = Player(m_iUndoCount, GetStoneMarkForPlayer1(), GetCursorMarkForPlayer1(), GetCenterCoordinate(), m_iWidth, m_iHeight);
		m_cPlayer2 = Player(m_iUndoCount, GetStoneMarkForPlayer2(), GetCursorMarkForPlayer2(), GetCenterCoordinate(), m_iWidth, m_iHeight);
	}
}

string GameManager::GetStoneMarkForPlayer1(MARKLIST markList)
{
	switch (markList)
	{
	case MARKLIST_FIRST:
		return "○";
	case MARKLIST_SECOND:
		return "♡";
	case MARKLIST_THIRD:
		return "☏";
	case MARKLIST_FOURTH:
		return "①";
	}
}

string GameManager::GetStoneMarkForPlayer2(MARKLIST markList)
{
	switch (markList)
	{
	case MARKLIST_FIRST:
		return "●";
	case MARKLIST_SECOND:
		return "♥";
	case MARKLIST_THIRD:
		return "☎";
	case MARKLIST_FOURTH:
		return "②";
	}
}

string GameManager::GetCursorMarkForPlayer1(MARKLIST markList)
{
	switch (markList)
	{
	case MARKLIST_FIRST:
		return "○";
	case MARKLIST_SECOND:
		return "♡";
	case MARKLIST_THIRD:
		return "☞";
	case MARKLIST_FOURTH:
		return "①";
	}
}

string GameManager::GetCursorMarkForPlayer2(MARKLIST markList)
{
	switch (markList)
	{
	case MARKLIST_FIRST:
		return "●";
	case MARKLIST_SECOND:
		return "♥";
	case MARKLIST_THIRD:
		return "☜";
	case MARKLIST_FOURTH:
		return "②";
	}
}

void GameManager::Reset()
{
	m_ePlayMode = PLAYMODE_NEW;
	m_bIsPlaying = false;
	m_iTurnCount = 1;

	ofstream save;
	save.open("Data.txt");
	if (save.is_open())
	{
		save << "";
		save.close();
	}
}

void GameManager::OpenGame()
{
	while (true)
	{
		system("cls");
		MapDraw::DrawMap(m_iWidth, m_iHeight);
		ShowMainMenu();

		int label;
		cin >> label;
		switch (label)
		{
		case MAINMENU_START_GAME:
			StartGame();
			break;
		case MAINMENU_REPLAY:
			LoadReplay();
			break;
		case MAINMENU_OPTIONS:
			OpenOptions();
			break;
		case MAINMENU_CLOSE:
			return;
		}
	}
}

void GameManager::ShowMainMenu()
{
	int startX = m_iWidth / 2;
	int startY = m_iHeight / 4;
	int yInterval = m_iHeight / 10;
	MapDraw::ShowString("★ 오 목 ★", m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("1.게임 시작", m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("2.다시 보기", m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("3.옵션 설정", m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("4.게임 종료", m_iWidth, startY);
	startY += yInterval;
	MapDraw::DrawBox(startX, startY, startX, 3);
	MapDraw::Gotoxy(m_iWidth, startY + 1);
}

void GameManager::StartGame()
{
	system("cls");
	m_bIsPlaying = true;
	if (m_ePlayMode == PLAYMODE_NEW)
	{
		InputPlayerName();
		m_cPlayer1.Reset(m_iUndoCount, GetCenterCoordinate(), m_iWidth, m_iHeight);
		m_cPlayer2.Reset(m_iUndoCount, GetCenterCoordinate(), m_iWidth, m_iHeight);
		DrawGameMap(m_iWidth, m_iHeight);
	}
	else
	{
		DrawGameMap(m_iWidth, m_iHeight);
		m_cPlayer1.DrawHistory();
		m_cPlayer2.DrawHistory();
	}
	ShowKeyInfo();
	Playing();
}

void GameManager::Playing()
{
	bool isPlayer1Turn = m_iTurnCount % 2 == 1;
	Player* controlPlayer = isPlayer1Turn ? &m_cPlayer1 : &m_cPlayer2;
	Player* opponentPlayer = isPlayer1Turn ? &m_cPlayer2 : &m_cPlayer1;

	ShowPlayerInfo(controlPlayer);

	while (true)
	{
		switch (controlPlayer->Playing(opponentPlayer))
		{
		case INPUTKEY_ENTER:
			if (controlPlayer->CheckVictory())
			{
				int startY = m_iHeight / 2;
				MapDraw::ShowString(controlPlayer->GetPlayerName() + "팀 승리!!", m_iWidth, startY++);
				SaveReplay();

				if (getch()) { }
				Reset();
				return;
			}
			else
			{
				m_iTurnCount++;
				SwapPlayer(&controlPlayer, &opponentPlayer);
				ShowPlayerInfo(controlPlayer);
			}
			break;
		case INPUTKEY_UNDO:
			m_iTurnCount--;
			SwapPlayer(&controlPlayer, &opponentPlayer);
			ShowPlayerInfo(controlPlayer);
			break;
		case INPUTKEY_OPTION:
			OpenOptions();
			DrawGameMap(m_iWidth, m_iHeight);
			ShowKeyInfo();
			ShowPlayerInfo(controlPlayer);
			opponentPlayer->DrawHistory();
			controlPlayer->DrawHistory();
			break;
		case INPUTKEY_ESC:
			if (m_iTurnCount == 1)
			{
				Reset();
			}
			else
			{
				Save();
			}
			return;
		}
	}
}

void GameManager::SwapPlayer(Player** p1, Player** p2)
{
	Player* temp = *p1;
	*p1 = *p2;
	*p2 = temp;
}

void GameManager::InputPlayerName()
{
	MapDraw::DrawBox(0, 0, m_iWidth, m_iHeight);

	string name;
	int startY = m_iHeight / 3;
	MapDraw::ShowString("P1 이름", m_iWidth, startY);
	MapDraw::ShowString("입력 : ", m_iWidth, startY + 1);
	cin >> name;
	m_cPlayer1.SetPlayerName(name);

	startY = m_iHeight / 2;
	MapDraw::ShowString("P2 이름", m_iWidth, startY);
	MapDraw::ShowString("입력 : ", m_iWidth, startY + 1);
	cin >> name;
	m_cPlayer2.SetPlayerName(name);
}

void GameManager::DrawGameMap(int width, int height)
{
	system("cls");
	MapDraw::DrawMap(width, height);
}

void GameManager::DrawPlayerStones()
{
	m_cPlayer1.DrawHistory();
	m_cPlayer2.DrawHistory();
}

void GameManager::ShowKeyInfo()
{
	int startY = m_iHeight;
	MapDraw::ShowString("====조작키====", m_iWidth, startY++);
	MapDraw::ShowString("이동 : A,S,W,D 돌 놓기 : ENTER", m_iWidth, startY++);
	MapDraw::ShowString("무르기 : N 옵션 : P 종료 : ESC", m_iWidth, startY);
}

void GameManager::ShowPlayerInfo(Player* player)
{
	int startY = m_iHeight + 3;
	MapDraw::ShowString(
		" Player Name : " + player->GetPlayerName() + "     무르기 : " + to_string(player->GetUndoCount()) + " ",
		m_iWidth, startY++);
	MapDraw::ShowString(" Turn : " + to_string(m_iTurnCount) + " ", m_iWidth, startY);
}

void GameManager::LoadReplay()
{
	ifstream load;
	string loadStr;
	load.open("GameLog.txt");
	getline(load, loadStr);
	if (load.is_open() && !loadStr.empty())
	{
		int commaIndex;
		int turnCount, width, height;
		MARKLIST stone;
		string p1Name, p2Name;
		string player1Log, player2Log;

		turnCount = stoi(loadStr);
		getline(load, loadStr);
		commaIndex = loadStr.find(",");
		width = stoi(loadStr.substr(0, commaIndex));
		height = stoi(loadStr.substr(commaIndex + 1, loadStr.length() - commaIndex));
		getline(load, loadStr);
		stone = (MARKLIST)stoi(loadStr);
		getline(load, loadStr);
		p1Name = loadStr;
		getline(load, loadStr);
		p2Name = loadStr;
		getline(load, loadStr);
		player1Log = loadStr;
		getline(load, loadStr);
		player2Log = loadStr;

		Replay(turnCount, width, height, stone, p1Name, p2Name, player1Log, player2Log);
	}
	else
	{
		system("cls");
		MapDraw::DrawBox(0, 0, m_iWidth, m_iHeight);
		MapDraw::ShowString("데이터가 없습니다.", m_iWidth, m_iHeight / 2);
		MapDraw::Gotoxy(0, m_iHeight);
		system("pause");
	}
}

void GameManager::Replay(int turnCount, int width, int height, MARKLIST stone, string p1Name, string p2Name, string p1Log, string p2Log)
{
	DrawGameMap(width, height);
	ShowReplayKeyInfo(width, height);

	int compareTurnCount = 1;
	Replayer replayer1(p1Name, GetStoneMarkForPlayer1(stone), p1Log);
	Replayer replayer2(p2Name, GetStoneMarkForPlayer2(stone), p2Log);
	Replayer* controlPlayer = &replayer1;

	controlPlayer->ReplayStone();
	ShowReplayPlayerInfo(controlPlayer, compareTurnCount, width, height);

	while (true)
	{
		switch (getch())
		{
		case INPUTKEY_SPACE:
			compareTurnCount++;
			if (compareTurnCount <= turnCount)
			{
				controlPlayer = compareTurnCount % 2 == 1 ? &replayer1 : &replayer2;
				controlPlayer->ReplayStone();
				ShowReplayPlayerInfo(controlPlayer, compareTurnCount, width, height);

				break;
			}
			else
			{
				int startY = height / 2;
				MapDraw::ShowString(controlPlayer->GetPlayerName() + "팀 승리!!", width, startY++);
				MapDraw::ShowString("다시보기 완료", width, startY);

				if (getch()) { }
				return;
			}
		case INPUTKEY_ESC:
			return;
		}
	}
}

void GameManager::ShowReplayKeyInfo(int width, int height)
{
	int startY = height;
	MapDraw::ShowString("====조작키====", width, startY++);
	MapDraw::ShowString("다음 수 보기 : SPACE    종료 : ESC", width, startY);
}

void GameManager::ShowReplayPlayerInfo(Replayer* player, int turnCount, int width, int height)
{
	int startY = height + 2;
	MapDraw::ShowString(" Player Name : " + player->GetPlayerName() + " ", width, startY++);
	MapDraw::ShowString(" Turn : " + to_string(turnCount) + " ", width, startY);
}

void GameManager::OpenOptions()
{
	while (true)
	{
		MapDraw::DrawBox(0, 0, m_iWidth, m_iHeight);
		ShowOptionMenu();

		int label;
		cin >> label;
		switch (label)
		{
		case OPTIONS_SIZE:
			if (m_bIsPlaying || m_ePlayMode == PLAYMODE_CONTINUE)
			{
				Block();
			}
			else
			{
				SetMapSize();
			}
			break;
		case OPTIONS_CURSOR:
			SetCursor();
			break;
		case OPTIONS_STONE:
			SetStone();
			break;
		case OPTIONS_UNDO:
			if (m_bIsPlaying || m_ePlayMode == PLAYMODE_CONTINUE)
			{
				Block();
			}
			else
			{
				SetUndo();
			}
			break;
		case OPTIONS_END:
			return;
		}
	}
}

void GameManager::ShowOptionMenu()
{
	int startY = m_iHeight / 5;
	int yInterval = m_iHeight / 10;
	MapDraw::ShowString("= Option =", m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("1.Map Size Set", m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("2.Cursor Custom", m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("3.Stone Custom", m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("4.Undo Count Set", m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("5.Return", m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("입력 : ", m_iWidth, startY);
}

void GameManager::SetMapSize()
{
	while (true)
	{
		MapDraw::DrawBox(0, 0, m_iWidth, m_iHeight);

		int width, height;
		int startY = m_iHeight / 2.5f;
		int yInterval = m_iHeight / 5;
		MapDraw::ShowString("Width : ", m_iWidth, startY);
		cin >> width;
		MapDraw::ShowString("Height : ", m_iWidth, startY + yInterval);
		cin >> height;

		if (width >= MAPRANGE_WIDTH_MIN && width <= MAPRANGE_WIDTH_MAX
			&& height >= MAPRANGE_HEIGHT_MIN && height <= MAPRANGE_HEIGHT_MAX)
		{
			m_iWidth = width;
			m_iHeight = height;

			system("cls");
			return;
		}
		else
		{
			MapDraw::DrawBox(0, 0, m_iWidth, m_iHeight);

			yInterval = m_iHeight / 10;
			string str = "(가로 : " + to_string(MAPRANGE_WIDTH_MIN) + " ~" + to_string(MAPRANGE_WIDTH_MAX) + ", 세로 : "
				+ to_string(MAPRANGE_HEIGHT_MIN) + " ~ " + to_string(MAPRANGE_HEIGHT_MAX) + ")";
			MapDraw::ShowString("변경 불가능", m_iWidth, startY);
			MapDraw::ShowString(str, m_iWidth, startY + yInterval);

			MapDraw::Gotoxy(0, m_iHeight);
			if (getch()) { }
		}
	}
}

void GameManager::SetCursor()
{
	while (true)
	{
		MapDraw::DrawBox(0, 0, m_iWidth, m_iHeight);
		ShowCursorMenu();
		int label;
		cin >> label;

		switch (label)
		{
		case MARKLIST_FIRST:
		case MARKLIST_SECOND:
		case MARKLIST_THIRD:
		case MARKLIST_FOURTH:
			m_eCursor = (MARKLIST)label;
			m_cPlayer1.ChangeCursorMark(GetCursorMarkForPlayer1(m_eCursor));
			m_cPlayer2.ChangeCursorMark(GetCursorMarkForPlayer2(m_eCursor));
			system("pause");
			return;
		case MARKLIST_END:
			return;
		}

	}
}

void GameManager::ShowCursorMenu()
{
	int startY = m_iHeight / 5;
	int yInterval = m_iHeight / 10;
	MapDraw::ShowString("= Set Cursor =", m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("1." + GetCursorMarkForPlayer1(MARKLIST_FIRST) + "," + GetCursorMarkForPlayer2(MARKLIST_FIRST), m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("2." + GetCursorMarkForPlayer1(MARKLIST_SECOND) + "," + GetCursorMarkForPlayer2(MARKLIST_SECOND), m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("3." + GetCursorMarkForPlayer1(MARKLIST_THIRD) + "," + GetCursorMarkForPlayer2(MARKLIST_THIRD), m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("4." + GetCursorMarkForPlayer1(MARKLIST_FOURTH) + "," + GetCursorMarkForPlayer2(MARKLIST_FOURTH), m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("5.Return", m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("입력 : ", m_iWidth, startY);
}

void GameManager::SetStone()
{
	while (true)
	{
		MapDraw::DrawBox(0, 0, m_iWidth, m_iHeight);
		ShowStoneMenu();
		int label;
		cin >> label;

		switch (label)
		{
		case MARKLIST_FIRST:
		case MARKLIST_SECOND:
		case MARKLIST_THIRD:
		case MARKLIST_FOURTH:
			m_eStone = (MARKLIST)label;
			m_cPlayer1.ChangeStoneMark(GetStoneMarkForPlayer1(m_eStone));
			m_cPlayer2.ChangeStoneMark(GetStoneMarkForPlayer2(m_eStone));
			system("pause");
			return;
		case MARKLIST_END:
			return;
		}
	}
}

void GameManager::ShowStoneMenu()
{
	int startY = m_iHeight / 5;
	int yInterval = m_iHeight / 10;
	MapDraw::ShowString("= Set Stone =", m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString(
		"1." + GetStoneMarkForPlayer1(MARKLIST_FIRST) + "," + GetStoneMarkForPlayer2(MARKLIST_FIRST), 
		m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString(
		"2." + GetStoneMarkForPlayer1(MARKLIST_SECOND) + "," + GetStoneMarkForPlayer2(MARKLIST_SECOND), 
		m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString(
		"3." + GetStoneMarkForPlayer1(MARKLIST_THIRD) + "," + GetStoneMarkForPlayer2(MARKLIST_THIRD), 
		m_iWidth, 
		startY);
	startY += yInterval;
	MapDraw::ShowString(
		"4." + GetStoneMarkForPlayer1(MARKLIST_FOURTH) + "," + GetStoneMarkForPlayer2(MARKLIST_FOURTH),
		m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("5.Return", m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("입력 : ", m_iWidth, startY);
}

void GameManager::SetUndo()
{
	while (true)
	{
		MapDraw::DrawBox(0, 0, m_iWidth, m_iHeight);
		ShowUndoMenu();
		int label;
		cin >> label;

		switch (label)
		{
		case UNDO_SET:
			m_iUndoCount = InputUndoCount();
			break;
		case UNDO_OFF:
			m_iUndoCount = 0;
			break;
		case UNDO_END:
			return;
		}
	}
}

void GameManager::ShowUndoMenu()
{
	int startY = m_iHeight / 5;
	int yInterval = m_iHeight / 10;
	MapDraw::ShowString("= Set Undo =", m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("1.Set Undo Count", m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("2.Undo Off", m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("3.Return", m_iWidth, startY);
	startY += yInterval;
	MapDraw::ShowString("입력 : ", m_iWidth, startY);
}

int GameManager::InputUndoCount()
{
	while (true)
	{
		MapDraw::DrawBox(0, 0, m_iWidth, m_iHeight);
		int count;
		int startY = m_iHeight / 2;
		MapDraw::ShowString("무르기 횟수 입력(최대 " + to_string(UNDOCOUNT_MAX) + "회) : ", m_iWidth, startY);
		cin >> count;

		if (count >= UNDOCOUNT_MIN && count <= UNDOCOUNT_MAX)
		{
			return count;
		}
		else
		{
			int yInterval = m_iHeight / 10;
			MapDraw::ShowString(
				"범위가 맞지 않습니다 ( " + to_string(UNDOCOUNT_MIN) + " ~ " + to_string(UNDOCOUNT_MAX) + " )", 
				m_iWidth, startY + yInterval);
			if(getch()) { }
		}
	}
}

void GameManager::Block()
{
	system("cls");

	int startY = m_iHeight / 2.5f;
	int yInterval = m_iHeight / 10;
	MapDraw::ShowString("접근 불가능", m_iWidth, startY);
	MapDraw::ShowString("(Game Play중)", m_iWidth, startY + yInterval);
	MapDraw::Gotoxy(0, m_iHeight);

	system("pause");
}

void GameManager::Save()
{
	if (m_iTurnCount != 1)
	{
		m_ePlayMode = PLAYMODE_CONTINUE;
		ofstream save;
		save.open("Data.txt");
		if (save.is_open())
		{
			save << m_iTurnCount << endl;
			save << m_iWidth << "," << m_iHeight << endl;
			save << m_eCursor << endl;
			save << m_eStone << endl;
			save << m_cPlayer1.GetPlayerName() << endl;
			save << m_cPlayer2.GetPlayerName() << endl;
			save << m_cPlayer1.GetUndoCount() << "," << m_cPlayer2.GetUndoCount() << endl;
			save << m_cPlayer1.GetHistoryString() << endl;
			save << m_cPlayer2.GetHistoryString() << endl;

			save.close();
		}
	}
}

void GameManager::SaveReplay()
{
	ofstream save;
	save.open("GameLog.txt");
	if (save.is_open())
	{
		save << m_iTurnCount << endl;
		save << m_iWidth << "," << m_iHeight << endl;
		save << m_eStone << endl;
		save << m_cPlayer1.GetPlayerName() << endl;
		save << m_cPlayer2.GetPlayerName() << endl;
		save << m_cPlayer1.GetHistoryString() << endl;
		save << m_cPlayer2.GetHistoryString() << endl;

		save.close();
	}
}