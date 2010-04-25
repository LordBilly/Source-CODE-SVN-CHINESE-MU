#include "MainRoot.h"
#include "D3D9RenderSystem.h"
#include "Config\Config.h"
#include "TextRender.h"

CMainRoot::CMainRoot()
{
	m_pRenderSystem = new CD3D9RenderSystem();
	SetRenderSystem(m_pRenderSystem);
	//
#if defined(_AUTO_FONT)
	UIGraph::InitFont(L"",GetConfig().m_nFontSize);
#else
	GetTextRender().registerFont(L"abc",GetConfig().m_strFontFilename);
	UIGraph::InitFont(L"abc",GetConfig().m_nFontSize);
#endif
	UISetDialogRes(GetConfig().m_strUIFilename);
	GetStyleMgr().Create(GetConfig().m_strThemeFilename);

	{
		HKEY hKey;
		if (ERROR_SUCCESS==RegCreateKeyExW(HKEY_LOCAL_MACHINE,L"software\\rpgsky\\modelview\\",
			NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&hKey,NULL))
		{
			std::wstring wstrFilename;
			{
				wchar_t szFilename[1024];
				ZeroMemory(szFilename,1024);
				GetModuleFileNameW(NULL,szFilename,1024);
				wstrFilename = szFilename;
			}
			RegSetValueExW(hKey,L"filename",0,REG_SZ,(LPBYTE)wstrFilename.c_str(),sizeof(wchar_t)*wstrFilename.length());
			RegCloseKey(hKey);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// Common Materials
	//////////////////////////////////////////////////////////////////////////
	CMaterial& material = m_pRenderSystem->getMaterialMgr().getItem("grid");
	//	static int nTex = R.GetTextureMgr().RegisterTexture("Data\\Textures\\grid.tga",1);// level is 1,let the grid more clear.
	//R.SetLightingEnabled(false);
	//R.SetTextureFactor(0xFF0000FF);
	//R.SetTextureColorOP(0,TBOP_SOURCE1,TBS_TFACTOR);
	material.setDiffuse("Data\\Textures\\grid.tga");
	material.uCull = 0;
	material.bDepthWrite = true;
	material.bAlphaTest = true;
	material.bBlend = false;
}

CMainRoot::~CMainRoot()
{
}

CDlgMainEditor& CMainRoot::getMainDialog()
{
	return m_dlgMain;
}

void CMainRoot::Run()
{
	CreateRenderWindow(L"RPGSkyEditor V0.7.0", 800, 500);
	if(!GetRenderSystem()->GetShaderMgr().createSharedShader("Data/fx/shared.fx"))
	{
		MessageBoxW(NULL,L"Can't find the shared fx",L"Error",0);
	}
	m_dlgMain.Create("IDD_MAIN");
	RECT rc={0,0,800, 500};
	m_dlgMain.OnSize(rc);
	CRoot::Run();
}

void CMainRoot::OnFrameMove(double fTime, float fElapsedTime)
{
	CRoot::OnFrameMove(fTime, fElapsedTime);
	m_dlgMain.OnFrameMove(fTime, fElapsedTime);
}

void CMainRoot::OnFrameRender(double fTime, float fElapsedTime)
{
	if(m_pRenderSystem->BeginFrame())
	{
		m_pRenderSystem->SetupRenderState();
		m_pRenderSystem->ClearBuffer(true,true,0x0);
		m_dlgMain.OnFrameRender(fTime, fElapsedTime);
		m_pRenderSystem->EndFrame();
	}
	CRoot::OnFrameRender(fTime, fElapsedTime);
}

bool CMainRoot::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Mouse messages
	switch(uMsg)
	{
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			switch(uMsg)
			{
			case WM_LBUTTONDBLCLK:
				m_dlgMain.OnLButtonDblClk(mousePoint);
				break;
			case WM_LBUTTONDOWN:
				m_dlgMain.OnLButtonDown(mousePoint);
				break;
			case WM_LBUTTONUP:
				m_dlgMain.OnLButtonUp(mousePoint);
				break;
			case WM_RBUTTONDBLCLK:
				m_dlgMain.OnRButtonDblClk(mousePoint);
				break;
			case WM_RBUTTONDOWN:
				m_dlgMain.OnRButtonDown(mousePoint);
				break;
			case WM_RBUTTONUP:
				m_dlgMain.OnRButtonUp(mousePoint);
				break;
			case WM_MBUTTONDBLCLK:
				m_dlgMain.OnMButtonDblClk(mousePoint);
				break;
			case WM_MBUTTONDOWN:
				m_dlgMain.OnMButtonDown(mousePoint);
				break;
			case WM_MBUTTONUP:
				m_dlgMain.OnMButtonUp(mousePoint);
				break;
			case WM_MOUSEMOVE:
				m_dlgMain.OnMouseMove(mousePoint);
				break;
			case WM_MOUSEWHEEL:
				//GetCursorPos(&mousePoint);
				ScreenToClient(UIGetHWND(),&mousePoint);
				m_dlgMain.OnMouseWheel(mousePoint,(short) HIWORD(wParam));
				break;
			}
		}
		break;
	}

	bool bNoFurtherProcessing = m_dlgMain.MsgProc(hWnd, uMsg, wParam, lParam);
	return false;
}