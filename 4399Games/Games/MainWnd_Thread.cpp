#include "stdafx.h"
#include "MainWnd.h"
#include "Pinyin.h"




UINT WINAPI CMainWnd::ThreadLoadList(void* lpParam)
{
	HWND hWnd = (HWND)lpParam;
	Json::Reader reader;
	Json::Value vRoot;
	CHttpClient http;
	string strJson = http.Request(L"http://ent.mb.baidu.com/games?qq-pf-to=pcqq.c2c", Get);
	if ( strJson.empty() )
	{		
		goto __FAILED;
	}
	if ( !reader.parse(strJson, vRoot) )
	{
		goto __FAILED;
	}
	if (m_bExit)
		return 0;
	try
	{
		Json::Value& vCode = vRoot["code"];
		if ( vCode.asInt() != 0 )
		{
			goto __FAILED;
		}
		Json::Value& vDataArray = vRoot["result"]["data"];
		Json::Value& vArrayCount = vRoot["result"]["total"];
		Json::Value& vArraySize = vRoot["result"]["size"];
		size_t nCount = vDataArray.size();
		bool bInit = false;
		wstring strSaveDir = g_strExePath + L"Cache\\Image\\";
		SHCreateDirectoryEx(NULL, strSaveDir.c_str(), NULL);
		for ( size_t i=0; i<nCount && !m_bExit; ++i)
		{
			Json::Value& vItemArray = vDataArray[i];
			size_t nSize = vItemArray.size();
			if ( 0 == nSize )
				continue;
			if ( !bInit )
			{
				g_gameList.reserve(nCount*nSize);
				bInit = true;
			}
			for ( size_t j=0; j<nSize; ++j )
			{
				Json::Value& vItem = vItemArray[j];
				Json::Value& vSource = vItem["source"];
				string str = vSource.asString();
				if ( str != "4399" )
					continue;
				{
					Json::Value& vId		= vItem["id"];
					Json::Value& vName		= vItem["name"];
					Json::Value& vCategory	= vItem["category"];
					Json::Value& vImgUrl	= vItem["img_url"];
					Json::Value& vWidth		= vItem["media_width"];
					Json::Value& vHeight	= vItem["media_height"];
					GAME_DATA item;
					item.nId = vId.asInt();
					item.nWidth = vWidth.asInt();
					item.nHeight = vHeight.asInt();
					item.strName = Utf2U(vName.asString());
					item.strType = Utf2U(vCategory.asString());
					item.strImgUrl	= Utf2U(vImgUrl.asString());
					item.strPath	= strSaveDir + GetNameByPathW(item.strImgUrl, true);
					string strPinyin = GetPinyin(U2A(item.strName));
					item.strPinyin	= A2U(strPinyin);
					g_gameList.push_back(item);
				}
				//g_gameMap.insert(std::make_pair(item.nId, item));
			}
		}
	}
	catch(...)
	{
		//½âÎöJSONÊ§°Ü
		goto __FAILED;
	}
	goto __FINISH;
__FAILED:
	if ( IsWindow(hWnd) )
	{
		::PostMessage(hWnd, WM_MWMSG_LOADLIST, 0, 0);
	}
	return 0;

__FINISH:
	if (IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_MWMSG_LOADLIST, 1, 0);
		for( size_t i=0; i<g_gameList.size() && !m_bExit; ++i )
		{
			if ( (NULL == g_hMainWnd) || !IsWindow(g_hMainWnd) )
				return 0;
			if ( PathFileExists(g_gameList[i].strPath.c_str()) )
				continue;
			CHttpClient http;
			if ( http.DownLoadFile(g_gameList[i].strImgUrl.c_str(), g_gameList[i].strPath.c_str()) )
			{
				::PostMessage(g_hMainWnd, WM_MWMSG_LOADIMG, g_gameList[i].nId, (LPARAM)g_gameList[i].strPath.c_str());
			}
		}
	}
	return 0;
}