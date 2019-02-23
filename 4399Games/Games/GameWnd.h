#pragma once
#include "resource.h"
#include "Data.h"
#include "UICefWebkit.h"


enum{
	WM_MSG_BASE = WM_USER + 210,
	WM_MSG_THREAD,
	WM_MSG_HELPER,
};
class CGameWnd :
	public WindowImplBase
{
public:
	CGameWnd();
	~CGameWnd(void);
	virtual LPCTSTR		GetWindowClassName()const	{ return L"GameWndClass"; }
	virtual CDuiString	GetSkinFile()				{ return L"GameWnd.xml"; }
	virtual CDuiString	GetSkinFolder()				{ return L"Skin"; }
//	virtual UILIB_RESOURCETYPE GetResourceType()	const	{ return UILIB_ZIPRESOURCE; }
//	virtual LPCTSTR		GetResourceID() const		{ return MAKEINTRESOURCE(IDR_DAT1); }
	void	Init(const GAME_DATA& game);

protected:
	virtual CControlUI* CreateControl( LPCTSTR pstrClass );
	virtual void		InitWindow();
	virtual void		OnFinalMessage( HWND hWnd );


private:
	//CWebEventHandler	m_webEvent;
	CControlUI	*m_pLblTitle;
	CControlUI	*m_pBtnClose;
	//CWebBrowserUI	*m_pWeb;
	CCefWebkitUI	*m_pWeb;
};
