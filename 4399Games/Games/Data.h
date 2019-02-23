#pragma once
#include <list>
using std::list;
#include <vector>
using std::vector;



typedef struct _GAME_DATA
{
	int	nId;
	int nWidth;
	int nHeight;
	wstring strName;
	wstring strType;
	wstring strImgUrl;
	wstring strPath;
	wstring strPinyin;

}GAME_DATA,*LPGAME_DATA;


typedef vector<GAME_DATA>				GameList;
typedef vector<GAME_DATA>::iterator	GameListItor;
extern GameList	g_gameList;
extern HWND	g_hMainWnd;
extern wstring g_strExePath;

// typedef map<int, GAME_DATA>				GameMap;
// typedef map<int, GAME_DATA>::iterator	GameMapItor;
// extern GameMap	g_gameMap;

inline bool GetGameDataById(const int nId, OUT GAME_DATA& game)
{
	GameListItor itor = g_gameList.begin();
	for ( ; itor!=g_gameList.end(); ++itor )
	{
		if ( itor->nId == nId )
		{
			game = *itor;
			return true;
		}
	}
	return false;
}
