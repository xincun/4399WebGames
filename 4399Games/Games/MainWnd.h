#pragma once
#include <string>
using std::string;
using std::wstring;
#include <map>
using std::map;
#include "Data.h"


enum InstStep
{
	Is_Init = 0, //Ê×Ò³
	Is_Insting,
	Is_Finish,
};


enum 
{
	WM_MWMSG_BASE = WM_USER + 200,
	WM_MWMSG_LOADLIST,
	WM_MWMSG_LOADIMG,
};

class CMainWnd
	:public WindowImplBase
{
public:
	CMainWnd();
	~CMainWnd();
	virtual LPCTSTR		GetWindowClassName()const			{ return L"MainWndClass"; }
	bool	StartLoadImgs();
	void	StopThread();

protected:
	
	virtual UILIB_RESOURCETYPE	GetResourceType()const;
	virtual	LPCTSTR		GetResourceID()const				{ return MAKEINTRESOURCE(0); }
	virtual CDuiString	GetSkinFolder()						{ return L"skin"; }
	virtual CDuiString	GetSkinFile()						{ return L"MainWnd.xml"; }
	virtual void		InitWindow();
	virtual void		OnCloseWnd();
	virtual	LRESULT		OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT		HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLoadListMsg(WPARAM wParam, LPARAM lParam);
	LRESULT OnLoadImgMsg(WPARAM wParam, LPARAM lParam);
	void	AddGame(const GAME_DATA& game);

	bool	OnNotifyBtnGame(void* lpParam);
	bool	OnNotifyEditSearch(void* lpParam);

	static UINT WINAPI ThreadLoadList(void* lpParam);

private:
	HANDLE	m_hThLoadImg;
	CEditUI	*m_pEditSearch;
	CTabLayoutUI	*m_pTabGame;
	CTileLayoutUI	*m_pLayout;
	map<int, CControlUI*>	m_gameMap;
	static  BOOL	m_bExit;
};



