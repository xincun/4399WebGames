#include "StdAfx.h"
#include "GameWnd.h"



CGameWnd::CGameWnd()
: m_pWeb(NULL)
, m_pLblTitle(NULL)
, m_pBtnClose(NULL)
{
	
}

CGameWnd::~CGameWnd(void)
{
	
}

void CGameWnd::InitWindow()
{
	SetIcon(IDI_GAMES);   
	m_pWeb = static_cast<CCefWebkitUI*>(m_PaintManager.FindControl(L"web"));
	assert(m_pWeb);
	m_pLblTitle = m_PaintManager.FindControl(L"title");
	assert(m_pLblTitle);
	m_pBtnClose = m_PaintManager.FindControl(L"btn_sys_close");
	assert(m_pBtnClose);
}

void CGameWnd::OnFinalMessage( HWND hWnd )
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void CGameWnd::Init(const GAME_DATA& game)
{
	wchar_t szUrl[200] = {0};
	swprintf_s(szUrl, L"http://ent.mb.baidu.com/gamepage/GameFloatWindows/FloatWindow.html?tn=SE_Baiduclient_9vpgkwv8&returnhttps=1&isnew=1&ver=2.4.0.1848&id=%d&width=%d&height=%d", \
	game.nId, game.nWidth, game.nHeight);
	::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, game.nWidth, game.nHeight+36, SWP_NOMOVE|SWP_HIDEWINDOW);
	m_pWeb->Navigate(szUrl);
	m_pLblTitle->SetText(game.strName.c_str());
	m_pLblTitle->SetToolTip(game.strName.c_str());
	RECT rcPos = {game.nWidth-28, 0, game.nWidth, 30};
	m_pBtnClose->SetPos(rcPos);
}


CControlUI* CGameWnd::CreateControl( LPCTSTR pstrClass )
{
	if ( _tcscmp(pstrClass, _T("CefWebkit")) == 0 )
		return new CCefWebkitUI;
	return NULL;
}
