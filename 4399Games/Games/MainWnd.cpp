#include "stdafx.h"
#include "MainWnd.h"
#include "Resource.h"
#include "GameWnd.h"
#include "Pinyin.h"

//GameMap	g_gameMap;
GameList	g_gameList;
HWND	g_hMainWnd = NULL;
wstring g_strExePath;


CMainWnd::CMainWnd()
: m_hThLoadImg(NULL)
, m_pEditSearch(NULL)
, m_pTabGame(NULL)
{

}

CMainWnd::~CMainWnd()
{
	
}

bool CMainWnd::StartLoadImgs()
{
	m_hThLoadImg = (HANDLE)_beginthreadex(NULL, 0, ThreadLoadList, m_hWnd, 0, NULL);
	if ( m_hThLoadImg )
		SetThreadPriority(m_hThLoadImg, THREAD_PRIORITY_ABOVE_NORMAL);
	return NULL != m_hThLoadImg;
}

void CMainWnd::StopThread()
{
	if (WAIT_TIMEOUT == WaitForSingleObject(m_hThLoadImg, 5000))
	{
		TerminateThread(m_hThLoadImg, 0);
	}
	CloseHandle(m_hThLoadImg);
	m_hThLoadImg = NULL;
}

DuiLib::UILIB_RESOURCETYPE CMainWnd::GetResourceType()const
{
#ifdef _DEBUG
	return UILIB_FILE;
#else
	return UILIB_FILE;//UILIB_ZIPRESOURCE;//UILIB_FILE;//;
#endif
}

void CMainWnd::InitWindow()
{
	SetIcon(IDI_GAMES);
	m_pLayout = static_cast<CTileLayoutUI*>(m_PaintManager.FindControl(L"layout"));
	assert(m_pLayout);
	m_pEditSearch = dynamic_cast<CEditUI*>(m_PaintManager.FindControl(L"edit_search"));
	assert(m_pEditSearch);
	m_pEditSearch->OnNotify += MakeDelegate(this, &CMainWnd::OnNotifyEditSearch);
	m_pTabGame = dynamic_cast<CTabLayoutUI*>(m_PaintManager.FindControl(L"tab_game"));
	assert(m_pTabGame);
	g_hMainWnd = m_hWnd;
}

void CMainWnd::OnCloseWnd()
{
	m_bExit = TRUE;
	StopThread();
	g_hMainWnd = NULL;
	WindowImplBase::OnCloseWnd();
	PostQuitMessage(0);
}

LRESULT CMainWnd::OnSysCommand( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if ( SC_CLOSE == wParam )
	{
		OnCloseWnd();
		return 0;
	}
	return WindowImplBase::OnSysCommand(uMsg, wParam, lParam, bHandled);
}

LRESULT CMainWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch( uMsg )
	{
	case WM_MWMSG_LOADLIST:
		OnLoadListMsg(wParam, lParam);
		bHandled = TRUE;
		break;
	case WM_MWMSG_LOADIMG:
		OnLoadImgMsg(wParam, lParam);
		bHandled = TRUE;
		break;
	}
	return WindowImplBase::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}

LRESULT CMainWnd::OnLoadListMsg(WPARAM wParam, LPARAM lParam)
{
	switch( wParam )
	{
	case 0://Ê§°Ü
		break;
	case 1://³É¹¦
		{
			size_t nCount = g_gameList.size();
			for ( size_t i=0; i<nCount; ++i )
			{
				AddGame(g_gameList[i]);
			}
			m_pTabGame->SelectItem(1);
		}
		break;
	}
	return 0;
}

LRESULT CMainWnd::OnLoadImgMsg(WPARAM wParam, LPARAM lParam)
{
	map<int, CControlUI*>::iterator itor = m_gameMap.find(wParam);
	if ( itor != m_gameMap.end() )
	{
		LPCTSTR lpImg = (LPCTSTR)lParam;
		CDuiString strBkImg;
		strBkImg.Format(L"file='%s' dest='4,0,76,72'", lpImg);
		itor->second->SetBkImage(strBkImg);
	}
	return 0;
}

void CMainWnd::AddGame(const GAME_DATA& game)
{
	CButtonUI* pBtn = new CButtonUI;
	CDuiString strAttr;
	LPCTSTR lpBkImg = L"def_app.png";
	if ( PathFileExists(game.strPath.c_str()) )
		lpBkImg = game.strPath.c_str();
	strAttr.Format(L"width=\"90\" height=\"100\" padding=\"0,6,0,6\" bkimage=\"file='%s' dest='9,0,81,72'\" text=\"%s\" font=\"0\" \
					textcolor=\"#FF999999\" textpadding=\"0,74,0,0\" align=\"center+top\" showhand=\"true\" tooltip=\"%s\" endellipsis=\"true\" ", lpBkImg, game.strName.c_str(), game.strName.c_str());
	pBtn->ApplyAttributeList(strAttr);
	pBtn->SetTag(game.nId);
	pBtn->SetUserData(game.strPinyin.c_str());
	pBtn->OnNotify += MakeDelegate(this, &CMainWnd::OnNotifyBtnGame);
	m_pLayout->Add(pBtn);
	m_gameMap.insert(std::make_pair(game.nId, pBtn));
}

bool CMainWnd::OnNotifyBtnGame(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if ( pNotify->sType != DUI_MSGTYPE_CLICK )
		return true;
	int nId = pNotify->pSender->GetTag();
	GAME_DATA game;
	if (GetGameDataById(nId, game))
	{
		CGameWnd* pWnd = new CGameWnd;
		pWnd->Create(NULL, game.strName.c_str(), UI_WNDSTYLE_FRAME^WS_MAXIMIZEBOX, 0);
		pWnd->Init(game);
		pWnd->CenterWindow();
		pWnd->ShowWindow();
	}
	return false;
}

bool CMainWnd::OnNotifyEditSearch(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if ( pNotify->sType != DUI_MSGTYPE_TEXTCHANGED )
		return true;
	int nCount = m_pLayout->GetCount();
	CDuiString strText = m_pEditSearch->GetText();
	if (strText.IsEmpty())
	{
		for (int i=0; i<nCount; ++i)
		{
			CControlUI* pItem = m_pLayout->GetItemAt(i);
			if (pItem)
				pItem->SetVisible();
		}
		return true;
	}
	string strHanzi = U2A(wstring(strText.GetData()));
	wstring strFindPinyin = A2U(GetPinyin(strHanzi));
	wstring strUpperPinyin = ToUpper(strFindPinyin);
	for (int i=0; i<nCount; ++i)
	{
		CControlUI* pItem = m_pLayout->GetItemAt(i);
		if (pItem)
		{
			const CDuiString& strPinyin = pItem->GetUserData();
			bool bFind = strPinyin.Find(strUpperPinyin.c_str()) != -1;
			pItem->SetVisible(bFind);
		}
	}
	return true;
}

BOOL CMainWnd::m_bExit = FALSE;




