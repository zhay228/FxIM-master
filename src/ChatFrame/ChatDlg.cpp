#include "StdAfx.h"
#include "Utils/UICrack.h"
#include "Utils/BLOWFISH.H"
#include "MainFrame/MainDlg.h"
#include "MainFrame/SkinDlg.h"
#include "MainFrame/ShareMng.h"
#include "ChatFrame/ChatDlg.h"
#include "ChatFrame/FaceSelDlg.h"
#include "ChatFrame/RichEditUtil.h"
#include "ControlEx/UIMenu.h"

const int	kRightPartLargeWidth = 270;
const int	kRightPartNormalWidth = 150;
const TCHAR kButtonCloseFileTrans[] = _T("btn_close_filetrans");
const TCHAR kButtonCloseMsgLog[] = _T("btn_close_msglog");

CChatDlg::CChatDlg(Host* pHost)
 : m_pHost(pHost)
 , m_pShareInfo(NULL)
 , m_pShareInfoImage(NULL)
 , m_pBtnSysRestore(NULL)
 , m_pBtnSysMax(NULL)
 , m_pOptionFont(NULL)
 , m_pOptionEmotion(NULL)
 , m_pBtnImage(NULL)
 , m_pLayoutFontbar(NULL)
 , m_pRichEditInput(NULL)
 , m_pRichEditView(NULL)
 , m_pComboFontName(NULL)
 , m_pComboFontSize(NULL)
 , m_pOptionBold(NULL)
 , m_pOptionItalic(NULL)
 , m_pOptionUnderline(NULL)
 , m_pLabelNickName(NULL)
 , m_pLabelDescription(NULL)
 , m_pLabelAvatar(NULL)
 , m_pBtnFileShare(NULL)
 , m_pVLayoutRightPanel(NULL)
 , m_pHLayoutOption(NULL)
 , m_pTLayoutPanel(NULL)
 , m_pVLayoutPersonal(NULL)
 , m_pFileTrans(NULL)
{
	m_szShare = new char[MAX_UDPBUF / 2];
	if (m_szShare) { memset(m_szShare, 0, MAX_UDPBUF / 2);}

	m_strTipFontString = _T("/a[\"9,0,0,0,#EB9647,����\"]");
	m_strWarningIcon.Format(_T("/c[\"%s\\misc\\image\\warning_icon.png\"]"), m_PaintManager.GetInstancePath());
}

CChatDlg::~CChatDlg()
{
	if (m_szShare)
		delete [] m_szShare;
}

LPCTSTR CChatDlg::GetWindowClassName() const
{
	return kWindowClassName;
}

void CChatDlg::OnFinalMessage(HWND hWnd)
{
	RemoveObserver();
	CMainDlg::m_mapChatDlg.erase(m_pHost);
	WindowImplBase::OnFinalMessage(hWnd);	
	delete this;
}

CDuiString CChatDlg::GetSkinFile()
{
	return _T("xmls\\dlg_chat.xml");
}

CDuiString CChatDlg::GetSkinFolder()
{
	return  _T("skin");
}

UILIB_RESOURCETYPE CChatDlg::GetResourceType() const
{
	return UILIB_FILE;
}

CControlUI*	CChatDlg::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, _T("FileTrans")) == 0)
	{
		return new CFileTransUI();
	}
	return NULL;
}


BOOL CChatDlg::Receive(SkinChangedParam param)
{
	return SetControlBackGround(m_PaintManager.FindControl(_T("bg")),
		param.bgimage, param.bkcolor);
}

void CChatDlg::InitWindow()
{
	Config*	pCfg = Singleton<Config>::getInstance();

	CSkinDlg::AddReceiver(this);
	SetControlBackGround(m_PaintManager.FindControl(_T("bg")), pCfg->szBgImage, pCfg->dwBgColor);

	if (m_pHost->hIconAvatar)	{
		::SendMessage(m_hWnd, WM_SETICON, TRUE, (LPARAM)m_pHost->hIconAvatar);
		::SendMessage(m_hWnd, WM_SETICON, FALSE, (LPARAM)m_pHost->hIconAvatar);
	}
	SetWindowText(m_hWnd, m_pHost->NickNameEx());
	m_pBtnSysRestore = static_cast<CButtonUI*>(m_PaintManager.FindControl(kButtonSysRestoreName));
	m_pBtnSysMax = static_cast<CButtonUI*>(m_PaintManager.FindControl(kButtonSysMaxName));
	m_pOptionFont = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("option_font")));
	m_pOptionEmotion = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("option_emotion")));
	m_pBtnImage = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_image")));
	m_pLayoutFontbar = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("layout_fontbar")));
	m_pRichEditInput = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("richedit_input")));
	m_pRichEditView = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("richedit_view")));

	// ���ڵ�������OLE�ؼ����ڽ���ͼƬ���滻ͼƬ����ʱ�ᵼ�±������������ʱ���η�ͼƬ�Ĺ���
	m_pBtnImage->SetEnabled(false);

	// ���岼�ֿؼ�
	m_pComboFontName = static_cast<CComboBoxUI*>(m_PaintManager.FindControl(_T("combo_font_name")));
	m_pComboFontSize = static_cast<CComboBoxUI*>(m_PaintManager.FindControl(_T("combo_font_size")));
	m_pOptionBold = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("option_bold")));
	m_pOptionItalic = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("option_italic")));
	m_pOptionUnderline = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("option_underline")));

	m_pBtnFileShare = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_fileshare")));

	m_pVLayoutRightPanel = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("vlayout_rightpanel")));
	m_pHLayoutOption = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("hlayout_option")));
	m_pTLayoutPanel = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tlayout_panel")));
	m_pVLayoutPersonal = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("vlayout_personal")));

	AddPanel(_T("������Ϣ"), m_pVLayoutPersonal);
	
	m_pFileTrans = static_cast<CFileTransUI*>(m_PaintManager.FindControl(_T("filetrans")));

	UpdatePersonalInfo();

	// ���ó�ʼֵ
	if (m_pLayoutFontbar)
	{	
		m_pLayoutFontbar->SetVisible(true);				// �Ƚ�Layout��Ϊ�ɼ�״̬�������ComboBox���ò��ܳɹ�����SelectText�����ڲ�˵����
		if (m_pComboFontName) m_pComboFontName->SelectText(pCfg->cfLite.szFaceName);
		TCHAR  szFontSize[12] = {0};
		wsprintf(szFontSize, _T("%d"), pCfg->cfLite.nSize);
		if (m_pComboFontSize) m_pComboFontSize->SelectText(szFontSize);
		if (m_pOptionBold) m_pOptionBold->Selected(pCfg->cfLite.bBold);
		if (m_pOptionItalic) m_pOptionItalic->Selected(pCfg->cfLite.bItalic);
		if (m_pOptionUnderline) m_pOptionUnderline->Selected(pCfg->cfLite.bUnderLine);
		CharFormatLite cfLite;
		if (m_pRichEditView) _RichEdit_SetFont(m_pRichEditView, cfLite);
		if (m_pRichEditInput) _RichEdit_SetFont(m_pRichEditInput, pCfg->cfLite);
		m_pLayoutFontbar->SetVisible(false);
	}

	// RichEdit.
	IRichEditOleCallback2* pRichEditOleCallback2 = NULL;
	HRESULT hr = ::CoCreateInstance(CLSID_ImageOle, NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IRichEditOleCallback2), (void**)&pRichEditOleCallback2);
	if (SUCCEEDED(hr))
	{
		pRichEditOleCallback2->SetNotifyHwnd(m_hWnd);
		ITextServices * pTextServices = m_pRichEditView->GetTextServices();
		pRichEditOleCallback2->SetTextServices(pTextServices);
		pTextServices->Release();
		m_pRichEditView->SetOleCallback(pRichEditOleCallback2);
		pRichEditOleCallback2->Release();
	}

	pRichEditOleCallback2 = NULL;
	hr = ::CoCreateInstance(CLSID_ImageOle, NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IRichEditOleCallback2), (void**)&pRichEditOleCallback2);
	if (SUCCEEDED(hr))
	{
		pRichEditOleCallback2->SetNotifyHwnd(m_hWnd);
		ITextServices * pTextServices = m_pRichEditInput->GetTextServices();
		pRichEditOleCallback2->SetTextServices(pTextServices);
		pTextServices->Release();
		m_pRichEditInput->SetOleCallback(pRichEditOleCallback2);
		pRichEditOleCallback2->Release();
	}

	IDropTarget *pdt = m_pRichEditInput->GetTxDropTarget();
	hr = ::RegisterDragDrop(m_hWnd, pdt);
	pdt->Release();
}

void CChatDlg::UpdatePersonalInfo()
{
	// �û���Ϣ
	m_pLabelAvatar = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("label_avatar")));
	m_pLabelNickName = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("label_nickname")));
	m_pLabelDescription = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("label_description")));

	in_addr addr;
	addr.S_un.S_addr = m_pHost->hostSub.addr;
	const char* szAddr = inet_ntoa(addr);

	m_pLabelAvatar->SetBkImage(m_pHost->avatar);
	m_pLabelNickName->SetText(m_pHost->NickNameEx());	
	m_pLabelDescription->SetText(szAddr);

	CLabelUI* pLabelPersonalName = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("label_personal_nickname")));
	CLabelUI* pLabelPersonalWorkgroup = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("label_personal_workgroup")));
	CLabelUI* pLabelPersonalAddr = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("label_personal_addr")));
	ASSERT(pLabelPersonalName);
	ASSERT(pLabelPersonalWorkgroup);
	ASSERT(pLabelPersonalAddr);

	pLabelPersonalName->SetText(m_pHost->NickNameEx());
	pLabelPersonalWorkgroup->SetText(m_pHost->groupName);
	pLabelPersonalAddr->SetText(szAddr);
}

void CChatDlg::Notify(TNotifyUI& msg)
{
	CDuiString strSenderName = msg.pSender->GetName();
	if (_tcscmp(msg.sType, DUI_MSGTYPE_CLICK) == 0)
	{
		if (_tcscmp(strSenderName, kButtonSysCloseName) == 0 
			|| _tcscmp(strSenderName, kButtonCloseName) == 0)
		{
			Close();
		}
		else if (_tcscmp(strSenderName, kButtonSysMinName) == 0)
		{
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
//			static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("layout_sys")))->Remove(msg.pSender);
		}
		else if (_tcscmp(strSenderName, kButtonSysMaxName) == 0)
		{
			if (m_pBtnSysMax) m_pBtnSysMax->SetVisible(false);
			if (m_pBtnSysRestore) m_pBtnSysRestore->SetVisible(true);
			SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		}
		else if (_tcscmp(strSenderName, kButtonSysRestoreName) == 0)
		{
			if (m_pBtnSysMax) m_pBtnSysMax->SetVisible(true);
			if (m_pBtnSysRestore) m_pBtnSysRestore->SetVisible(false);
			SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
		}
		else if (_tcscmp(strSenderName, _T("option_font")) == 0)
		{
			if (m_pLayoutFontbar != NULL && m_pOptionFont != NULL) {
				m_pLayoutFontbar->SetVisible(!m_pOptionFont->IsSelected());				
			}
		}
		else if (_tcscmp(strSenderName, _T("option_emotion")) == 0)
		{
			OnBtnEmotion(msg);
		}
		else if (_tcscmp(strSenderName, _T("btn_image")) == 0)
		{
			OnBtnImage(msg);
		}
		else if (_tcscmp(strSenderName, _T("btn_color")) == 0)
		{
			OnBtnColor(msg);
		}
		else if (_tcscmp(strSenderName, _T("option_bold")) == 0)
		{
			OnBtnBold(msg);
		}
		else if (_tcscmp(strSenderName, _T("option_italic")) == 0)
		{
			OnBtnItalic(msg);
		}
		else if (_tcscmp(strSenderName, _T("option_underline")) == 0)
		{
			OnBtnUnderLine(msg);
		}
		else if (_tcscmp(strSenderName, _T("btn_file")) == 0)
		{
			OnBtnFile(msg);
		}
		else if (_tcscmp(strSenderName, _T("btn_file_right")) == 0)
		{
			OnBtnFileRight(msg);
		}
		else if (_tcscmp(strSenderName, _T("btn_send_left")) == 0)
		{
			OnBtnSendMsg();
		}
		else if (_tcscmp(strSenderName, _T("btn_send_right")) == 0)
		{
			OnBtnSendMsgRight(msg);
		}
		else if (_tcscmp(strSenderName, kButtonCloseFileTrans) == 0)
		{
			CloseFileTrans(msg);
		}
		else if (_tcsstr(strSenderName, _T("option_panel_")) != NULL)
		{
			CControlUI*	pControl = reinterpret_cast<CControlUI*>(msg.pSender->GetTag());
			if (pControl) m_pTLayoutPanel->SelectItem(pControl);
		}		
	}
	else if (msg.sType == _T("itemselect"))
	{
		if (_tcscmp(strSenderName, _T("combo_font_name")) == 0)
		{
			OnItemSelectComboFontName(msg);
		}
		else if (_tcscmp(strSenderName, _T("combo_font_size")) == 0) 
		{
			OnItemSelectComboFontSize(msg);
		}
	}
}

LRESULT CChatDlg::OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (EN_PASTE == ((LPNMHDR)lParam)->code)
	{
		// ImageOle.dll���͵�֪ͨ��Ϣ��ճ��ͼƬ��ImageOle.dll�ѽ�ͼƬ������ϵͳ����ʱĿ¼����Md5������
		ITextServices * pTextServices = m_pRichEditInput->GetTextServices();
		if ((UINT)pTextServices == ((LPNMHDR)lParam)->idFrom)
		{
			// ImageOle.dll��Unicode���룬((NMRICHEDITOLECALLBACK *)lParam)->lpszText��Unicode�ַ��� [12/15/2014 ybt] 
			LPCWSTR pszTextW = (WCHAR*)((NMRICHEDITOLECALLBACK *)lParam)->lpszText;

#if (defined(UNICODE) || defined(_UNICODE))
			AddMsgToRichEditInput(pszTextW);
#else
			LPCSTR pszTextA = UnicodeToAnsi(pszTextW);
			if (pszTextA != NULL)
			{
				AddMsgToRichEditInput(pszTextA);
				delete pszTextA;
			}
#endif			
		}
		if (pTextServices != NULL)
			pTextServices->Release();
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CChatDlg::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (uMsg)
	{
	case WM_NOTIFY:
		OnNotify(uMsg, wParam, lParam, bHandled);
		break;
	case WM_RBUTTONUP:
		OnRButtonUp(uMsg, wParam, lParam, bHandled);
		break;
	case FACE_CTRL_SEL:
		OnFaceCtrlSel(uMsg, wParam, lParam, bHandled);
		break;
	case WM_MENUCLICK:
		OnMenuClick(uMsg, wParam, lParam, bHandled);
		break;
	case WM_SENDOBJEVENT:
		OnSendObjEvent(uMsg, wParam, lParam, bHandled);
		break;
	case WM_RECVOBJEVENT:
		OnRecvObjEvent(uMsg, wParam, lParam, bHandled);
		break;
	}
	return 0;
}

LRESULT CChatDlg::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BOOL bZoomed = ::IsZoomed(m_hWnd);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if (::IsZoomed(m_hWnd) != bZoomed)
	{
		bool bFlag = bZoomed ? true : false;
		if (m_pBtnSysMax) m_pBtnSysMax->SetVisible(bFlag);
		if (m_pBtnSysRestore) m_pBtnSysRestore->SetVisible(!bFlag);
	}

	return 0;
}

LRESULT CChatDlg::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CChatDlg::OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	CRichEditUI*	pRichEdit = static_cast<CRichEditUI*>(m_PaintManager.FindControl(pt));
	if (pRichEdit == NULL) 
	{
		bHandled = FALSE;
		return 0;
	}

	ITextServices * pTextServices = pRichEdit->GetTextServices();
	IImageOle * pImageOle = NULL;	
	CMenuWnd*	pMenu = new CMenuWnd(m_hWnd);
	BOOL bRet = RichEdit_GetImageOle(pTextServices, pt, &pImageOle);

	::ClientToScreen(m_hWnd, &pt);
	if (pMenu == NULL)  return 0;
	if (m_pRichEditInput == pRichEdit)
	{
		STRINGorID	xml(_T("xmls\\menu_richedit_input.xml"));	
		pMenu->Init(NULL, xml, _T("xml"), pt);
		
	}
	else if (m_pRichEditView == pRichEdit) 
	{
		STRINGorID	xml(_T("xmls\\menu_richedit_view.xml"));

		pMenu->Init(NULL, xml, _T("xml"), pt);
		if (m_strRichEditViewZoom.IsEmpty())
			pMenu->CheckMenuItem(_T("menu_zoom_100"), true);
		else 
			pMenu->CheckMenuItem(m_strRichEditViewZoom, true);
	}
	pMenu->EnableMenuItem(_T("menu_saveas"), bRet ? true : false);
	bHandled = FALSE;
	return 0;
}

LRESULT CChatDlg::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Config*	pCfg = Singleton<Config>::getInstance();
	bHandled = FALSE;
	if (m_pRichEditInput != NULL && m_pRichEditInput->IsFocused())
	{
		if (GetKeyState(VK_CONTROL)<0) //lParam & VK_CONTROL
		{
			if (wParam == 'V')
			{
				//m_pRichEditInput->Paste();
				m_pRichEditInput->PasteSpecial(CF_TEXT);
				bHandled = TRUE;
			}
			else if (wParam == VK_RETURN && !pCfg->bWantReturnSend)
			{
				OnBtnSendMsg();
				bHandled = TRUE;
			}
		}
		else if (wParam == VK_RETURN && pCfg->bWantReturnSend) {
			OnBtnSendMsg();
			bHandled = TRUE;
		}
	}	
	return 0;
}

LRESULT CChatDlg::OnFaceCtrlSel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CFaceSelDlg* pFaceDlg = Singleton<CFaceSelDlg>::getInstance();

	int nFaceId = pFaceDlg->GetSelFaceId();
	int nFaceIndex = pFaceDlg->GetSelFaceIndex();
	tstring strFileName = pFaceDlg->GetSelFaceFileName();
	if (!strFileName.empty())
	{
		_RichEdit_InsertFace(m_pRichEditInput, strFileName.c_str(), nFaceId, nFaceIndex);
		m_pRichEditInput->SetFocus();
	}
	if (m_pOptionEmotion) m_pOptionEmotion->Selected(false);	
	bHandled = true;
	return 0;
}

LRESULT CChatDlg::OnMenuClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Config*	pCfg = Singleton<Config>::getInstance();
	TCHAR* szMenuItem = (TCHAR*)wParam;
	if (_tcsicmp(szMenuItem, _T("menu_send_wantreturn")) == 0)
	{
		pCfg->bWantReturnSend = TRUE;
	}
	else if (_tcsicmp(szMenuItem, _T("menu_send_ctrl_wantreturn")) == 0)
	{
		pCfg->bWantReturnSend = FALSE;
	}
	else if (_tcsicmp(szMenuItem, _T("menu_send_file")) == 0) 
	{
		OnBrowserFile();
	}
	else if (_tcsicmp(szMenuItem, _T("menu_send_folder")) == 0) 
	{
		OnBrowserFolder();
	}
	else if (_tcsicmp(szMenuItem, _T("menu_copy")) == 0) 
	{
		if (m_pRichEditView->IsFocused()) m_pRichEditView->Copy();
		else if (m_pRichEditInput->IsFocused()) m_pRichEditInput->Copy();
	}
	else if (_tcsicmp(szMenuItem, _T("menu_cut")) == 0) 
	{
		if (m_pRichEditInput->IsFocused()) m_pRichEditInput->Cut();
	}
	else if (_tcsicmp(szMenuItem, _T("menu_selall")) == 0) 
	{
		if (m_pRichEditView->IsFocused()) m_pRichEditView->SetSelAll();
	}
	else if (_tcsicmp(szMenuItem, _T("menu_saveas")) == 0) 
	{
		//if (m_pRichEditView->IsFocused()) m_pRichEditView->Copy();
		//else if (m_pRichEditInput->IsFocused()) m_pRichEditInput->Copy();
	}
	else if (_tcsicmp(szMenuItem, _T("menu_paste")) == 0) 
	{
		m_pRichEditInput->PasteSpecial(CF_TEXT);
	}
	else if (_tcsicmp(szMenuItem, _T("menu_zoom_400")) == 0) 
	{
		m_pRichEditView->SetZoom(16, 4);
		m_strRichEditViewZoom = szMenuItem;
	}
	else if (_tcsicmp(szMenuItem, _T("menu_zoom_400")) == 0) 
	{
		m_pRichEditView->SetZoom(16, 4);
		m_strRichEditViewZoom = szMenuItem;
	}
	else if (_tcsicmp(szMenuItem, _T("menu_zoom_200")) == 0) 
	{
		m_pRichEditView->SetZoom(8, 4);
		m_strRichEditViewZoom = szMenuItem;
	}
	else if (_tcsicmp(szMenuItem, _T("menu_zoom_150")) == 0) 
	{
		m_pRichEditView->SetZoom(6, 4);
		m_strRichEditViewZoom = szMenuItem;
	}
	else if (_tcsicmp(szMenuItem, _T("menu_zoom_125")) == 0) 
	{
		m_pRichEditView->SetZoom(5, 4);
		m_strRichEditViewZoom = szMenuItem;
	}
	else if (_tcsicmp(szMenuItem, _T("menu_zoom_100")) == 0) 
	{
		m_pRichEditView->SetZoom(0, 0);
		m_strRichEditViewZoom = szMenuItem;
	}
	else if (_tcsicmp(szMenuItem, _T("menu_zoom_75")) == 0) 
	{
		m_pRichEditView->SetZoom(3, 4);
		m_strRichEditViewZoom = szMenuItem;
	}
	else if (_tcsicmp(szMenuItem, _T("menu_zoom_50")) == 0) 
	{
		m_pRichEditView->SetZoom(1, 2);
		m_strRichEditViewZoom = szMenuItem;
	}
	bHandled = TRUE;
	return 0;
}

// Ŀǰ�ļ����䶼�Ǿ���������
LRESULT CChatDlg::OnTcpEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(HIWORD(lParam)) {
		DUI__Trace(_T("CChatDlg::OnTcpEvent error %d"), HIWORD(lParam));
		return -1;
	}

	switch(LOWORD(lParam)) 
	{
	case FD_CONNECT:
		/*TRACE(_T("=> CChatDlg OnTcpEvent: FD_CONNECT, so StartRecvFile\n"));
		StartRecvFile();*/
		break;
	case FD_CLOSE:
		DUI__Trace(_T("CChatDlg OnTcpEvent: FD_CLOSE, so EndRecvFile\n"));
		//EndRecvFile(TRUE); 
		//::closesocket((SOCKET)wParam);
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CChatDlg::OnSendObjEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SendFileObj*		sendObj = (SendFileObj*)wParam;
	CSendFileElementUI*	pElement = NULL;
	
	if (sendObj == NULL) {
		return -1;
	}
	pElement = reinterpret_cast<CSendFileElementUI*>(sendObj->tag);
	switch (lParam)
	{
	case CShareMng::TRANS_INIT:
		if (pElement != NULL) pElement->StartSendFile();
		break;
	case CShareMng::TRANS_BUSY:
		if (pElement != NULL) pElement->SetTransState();
		break;
	case CShareMng::TRANS_DONE:
		DUI__Trace(_T("CChatDlg::OnSendObjEvent TRANS_DONE status : %d"), sendObj->status);
		if (sendObj->fileInfo->exattr != IM_FILE_CHATIMAGE)
		{
			CDuiString strText;

			strText.Format(_T("%s%s�ļ�\"%s\"����%s.(%s)"), (LPCTSTR)m_strTipFontString,
				(LPCTSTR)m_strWarningIcon, sendObj->fileInfo->Fname(), sendObj->status == FS_COMPLETE ? _T("�ɹ�") : _T("ʧ��"),
				FormatTime(time(NULL), _T("%H:%M:%S")));
			AddTipMsgToRichEditView(strText);
		}
		if (sendObj->status == FS_COMPLETE) 
		{
		}
		// Error occurs or user cancel
		else {

		}
		//if (pElement != NULL) pElement->EndSendFile();
		break;
	}
	bHandled = TRUE;
	return 0;
}

LRESULT CChatDlg::OnRecvObjEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RecvFileObj*		recvObj = (RecvFileObj*)wParam;
	CRecvFileElementUI*	pElement = NULL;

	if (recvObj == NULL) {
		return -1;
	}
	pElement = reinterpret_cast<CRecvFileElementUI*>(recvObj->tag);
	switch (lParam)
	{
	case CShareMng::TRANS_INIT:
		if (pElement != NULL) pElement->StartRecvFile();
		break;
	case CShareMng::TRANS_BUSY:
		if (pElement != NULL) pElement->SetTransState();
		break;
	case CShareMng::TRANS_DONE:
		DUI__Trace(_T("CChatDlg::OnRecvObjEvent TRANS_DONE status : %d"), recvObj->status);
		if (recvObj->fileInfo->exattr != IM_FILE_CHATIMAGE)
		{
			CDuiString strText;
			strText.Format(_T("%s%s�ļ�\"%s\"����%s.(%s)"), (LPCTSTR)m_strTipFontString,
				(LPCTSTR)m_strWarningIcon, recvObj->fileInfo->Fname(), recvObj->status == FS_COMPLETE ? _T("�ɹ�") : _T("ʧ��"),
				FormatTime(time(NULL), _T("%H:%M:%S")));
			AddTipMsgToRichEditView(strText);
		}

		{
			BOOL bImageExists = recvObj->status == FS_COMPLETE || recvObj->status == FS_ALREADYEXISTS;
			if (recvObj->fileInfo->exattr == IM_FILE_CHATIMAGE)
			{
				// ����RichEdit�е�IImageOle.
				IImageOle*	pImageOle = m_mapImageOle[recvObj->fileInfo->Fname()];
				char		szBuf[MAX_PATH] = {0};
				if (pImageOle != NULL) 
				{
					wsprintf(szBuf, _T("%s\\%s"), recvObj->saveDir, recvObj->fileInfo->Fname());
					if (!::PathFileExists(szBuf)) {
						wsprintf(szBuf, _T("%s\\misc\\image\\errorBmp.gif"), m_PaintManager.GetInstancePath());
					}
					LPCWSTR pszBufW = AnsiToUnicode(szBuf);
					DUI__Trace(_T("ImageOle Update file : %s"), szBuf);
					if (pszBufW != NULL)
					{
						pImageOle->LoadFromFile((BSTR)pszBufW);
						delete pszBufW;
					}
					pImageOle->OnViewChange();
				}
			}
		}
		//if (pElement != NULL) pElement->EndRecvFile();
		//pShareMng->EndRecvFile(recvObj);
		break;
	}
	bHandled = TRUE;
	return 0;
}


LRESULT CChatDlg::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (wParam)
	{
	case IM_FLASHWINDOW_TIMER:
		{
			static	int nCount = 0;
			if (nCount++ > 5 || m_hWnd == ::GetLastActivePopup(::GetForegroundWindow()))
			{
				KillTimer(m_hWnd, IM_FLASHWINDOW_TIMER);
				nCount = 0;
			}
			else 
			{
				FlashWindow(m_hWnd, TRUE);
			}
		}
		bHandled = TRUE;
		break;
	}	
	return 0;
}

void CChatDlg::_RichEdit_ReplaceSel(CRichEditUI * pRichEdit, LPCTSTR lpszNewText, CharFormatLite& cfLite, int nStartIndent)
{
	ITextServices * pTextServices = pRichEdit->GetTextServices();

#if (defined(UNICODE) || defined(_UNICODE))
	RichEdit_ReplaceSel(pTextServices, lpszNewText, 
		cfLite.szFaceName, cfLite.nSize, 
		cfLite.crText, cfLite.bBold, cfLite.bItalic, 
		cfLite.bUnderLine, FALSE, nStartIndent);
#else
	LPCWSTR pszNewTextW = AnsiToUnicode(lpszNewText);
	LPCWSTR pszFaceNameW = AnsiToUnicode(cfLite.szFaceName);
	if (pszNewTextW != NULL && pszFaceNameW != NULL)
	{
		RichEdit_ReplaceSel(pTextServices, pszNewTextW, 
			pszFaceNameW, cfLite.nSize, 
			cfLite.crText, cfLite.bBold, cfLite.bItalic, 
			cfLite.bUnderLine, FALSE, nStartIndent);
		delete pszNewTextW;
		delete pszFaceNameW;
	}
#endif
	
	pTextServices->Release();
}

BOOL CChatDlg::_RichEdit_InsertFace(CRichEditUI * pRichEdit, LPCTSTR lpszFileName, int nFaceId, 
									int nFaceIndex, IImageOle** ppImageOle)
{
	BOOL bRet = FALSE;

	if (NULL == pRichEdit || NULL == lpszFileName || NULL == *lpszFileName)
		return FALSE;

	ITextServices * pTextServices = pRichEdit->GetTextServices();
	ITextHost * pTextHost = pRichEdit->GetTextHost();

	if (pTextServices != NULL && pTextHost != NULL)
	{
		if (pRichEdit == m_pRichEditView)
			RichEdit_SetStartIndent(pTextServices, 300);
#if (defined(UNICODE) || defined(_UNICODE))
		bRet = RichEdit_InsertFace(pTextServices, pTextHost, 
			lpszFileName, nFaceId, nFaceIndex, RGB(255,255,255), TRUE, 40, ppImageOle);
#else
		LPCWSTR pszFileNameW = AnsiToUnicode(lpszFileName);
		if (pszFileNameW != NULL)
		{
			bRet = RichEdit_InsertFace(pTextServices, pTextHost, 
				pszFileNameW, nFaceId, nFaceIndex, RGB(255,255,255), TRUE, 40, ppImageOle);
			delete pszFileNameW;
		}
#endif	
	}

	if (pTextServices != NULL)
		pTextServices->Release();
	if (pTextHost != NULL)
		pTextHost->Release();

	return bRet;
}

BOOL CChatDlg::HandleSysFaceId(CRichEditUI * pRichEdit, LPCTSTR& p, tstring& strText, CharFormatLite& cfLite, int nStartIndent)
{
	int nFaceId = GetBetweenInt(p+2, _T("[\""), _T("\"]"), -1);
	CFaceSelDlg*	pFaceSelDlg = Singleton<CFaceSelDlg>::getInstance();
	CFaceInfo*		lpFaceInfo = pFaceSelDlg->GetFaceList().GetFaceInfoById(nFaceId);
	if (lpFaceInfo != NULL)
	{
		if (!strText.empty())
		{
			_RichEdit_ReplaceSel(pRichEdit, strText.c_str(), cfLite, nStartIndent); 
			strText = _T("");
		}

		_RichEdit_InsertFace(pRichEdit, lpFaceInfo->m_strFileName.c_str(), 
			lpFaceInfo->m_nId, lpFaceInfo->m_nIndex);

		p = _tcsstr(p+2, _T("\"]"));
		p++;
		return TRUE;
	}
	return FALSE;
}

BOOL CChatDlg::HandleSysFaceIndex(CRichEditUI * pRichEdit, LPCTSTR& p, tstring& strText, CharFormatLite& cfLite, int nStartIndent)
{
	int nFaceIndex = GetBetweenInt(p+2, _T("[\""), _T("\"]"), -1);
	CFaceSelDlg*	pFaceSelDlg = Singleton<CFaceSelDlg>::getInstance();
	CFaceInfo*		lpFaceInfo = pFaceSelDlg->GetFaceList().GetFaceInfoById(nFaceIndex);
	if (lpFaceInfo != NULL)
	{
		if (!strText.empty())
		{
			_RichEdit_ReplaceSel(pRichEdit, strText.c_str(), cfLite, nStartIndent); 
			strText = _T("");
		}

		_RichEdit_InsertFace(pRichEdit, lpFaceInfo->m_strFileName.c_str(), 
			lpFaceInfo->m_nId, lpFaceInfo->m_nIndex);

		p = _tcsstr(p+2, _T("\"]"));
		p++;
		return TRUE;
	}
	return FALSE;
}

BOOL CChatDlg::HandleCustomPic(CRichEditUI * pRichEdit, LPCTSTR& p, tstring& strText, CharFormatLite& cfLite, int nStartIndent)
{
	TCHAR		szBuf[MAX_PATH] = {0};
	IImageOle*	pImageOle = NULL;
	tstring		strFileName = GetBetweenString(p+2, _T("[\""), _T("\"]"));
	tstring		strFilePath;
	Config*		pCfg = Singleton<Config>::getInstance();

	if (!strFileName.empty())
	{
		if (!strText.empty())
		{
			_RichEdit_ReplaceSel(pRichEdit, strText.c_str(), cfLite, nStartIndent); 
			strText = _T("");
		}
		
		
		// ���ֻ���ļ���
		if (strFileName.find('\\') == -1 && strFileName.find('/') == -1 )
		{
			wsprintf(szBuf, _T("%s\\%s"), pCfg->szChatImageSaveDir, strFileName.data());
			strFilePath = szBuf;
		}
		else {
			strFilePath = strFileName;
		}

		// ����ļ�������
		if (GetFileAttributes(strFilePath.c_str()) == 0xFFFFFFFF) 
		{
			wsprintf(szBuf, _T("%s\\misc\\image\\sendingBmp.gif"), m_PaintManager.GetInstancePath());
			_RichEdit_InsertFace(pRichEdit, szBuf, -1, -1, &pImageOle);
			if (pImageOle != NULL) {
				m_mapImageOle[strFileName] = pImageOle;
			}
		}
		else {
			_RichEdit_InsertFace(pRichEdit, strFilePath.c_str(), -1, -1, &pImageOle);
		}		

		p = _tcsstr(p+2, _T("\"]"));
		p++;
		return TRUE;
	}
	return FALSE;
}

// ʾ��"/a["12,0,1,0,#FF89F008,����"]"
tstring CChatDlg::EncodeFontString(CharFormatLite& cfLite)
{
	TCHAR	szBuf[MAX_PATH];
	tstring	strFontString;

	wsprintf(szBuf, _T("/a[\"%d,%d,%d,%d,#%X,%s\"]"), cfLite.nSize, cfLite.bBold, 
		cfLite.bItalic, cfLite.bUnderLine, cfLite.crText, cfLite.szFaceName);
	strFontString = szBuf;
	return strFontString;
}

BOOL CChatDlg::HandleCharFormatLite(LPCTSTR& p, CharFormatLite& cfLite)
{
	tstring strInfo = GetBetweenString(p+2, _T("[\""), _T("\"]"));
	if (!strInfo.empty()) 
	{
		// ע�� : ���cfLite.szFaceName��ǰ�棬�����ַ������ᱻ������cfLite.szFaceName�У�cfLite�е�
		// �������û�и�ֵ�ˡ� [11/29/2014 ybt]
		int len = _stscanf(strInfo.data(), _T("%d,%d,%d,%d,#%X,%s"), &cfLite.nSize, 
			&cfLite.bBold, &cfLite.bItalic, &cfLite.bUnderLine, &cfLite.crText, cfLite.szFaceName);

		p = _tcsstr(p+2, _T("\"]"));
		p += 2;
		return len > 0;
	}
	return FALSE;
}

// "/a["������Ϣ"]/f["ϵͳ����id"]/s["ϵͳ����index"]/c["�Զ���ͼƬ·��"]"
void CChatDlg::AddMsg(CRichEditUI * pRichEdit, LPCTSTR lpText, int nStartIndent)
{
	if (NULL == pRichEdit || NULL == lpText || NULL == *lpText)
		return;

	LPCTSTR			p = lpText;
	tstring			strText;
	CharFormatLite	cfLite;

	// �������������Ϣ
	if (_tcsncmp(p, _T("/a[\""), 4) == 0)
	{
		HandleCharFormatLite(p, cfLite);
	}

	for (; *p != _T('\0'); p++)
	{
		if (*p == _T('/'))
		{
			if (*(p+1) == _T('/'))
			{
				strText += *p;
				p++;
				continue;
			}
			else if (*(p+1) == _T('f'))
			{
				if (HandleSysFaceId(pRichEdit, p, strText, cfLite, nStartIndent))
					continue;
			}
			else if (*(p+1) == _T('s'))
			{
				if (HandleSysFaceIndex(pRichEdit, p, strText, cfLite, nStartIndent))
					continue;
			}
			else if (*(p+1) == _T('c'))
			{
				if (HandleCustomPic(pRichEdit, p, strText, cfLite, nStartIndent))
					continue;
			}
		}
		strText += *p;
	}

	if (!strText.empty())
		_RichEdit_ReplaceSel(pRichEdit, strText.c_str(), cfLite, nStartIndent);
}

void CChatDlg::AddMsgToRichEditInput(LPCTSTR lpText)
{
	AddMsg(m_pRichEditInput, lpText, 0);
	m_pRichEditInput->EndDown();
}

void CChatDlg::AddMsgToRichEditView(LPCTSTR lpText, LPCTSTR lpName, time_t msgTime, COLORREF crName)
{
	if (NULL == lpText || NULL == *lpText)
		return;

	m_pRichEditView->SetAutoURLDetect(true);

	tstring strTime;
	strTime = FormatTime(msgTime, _T("%H:%M:%S"));

	ITextServices * pTextServices = m_pRichEditView->GetTextServices();

	TCHAR cText[MAX_PATH] = {0};
	wsprintf(cText, _T("%s %s\r\n"), lpName, strTime.c_str());

	RichEdit_SetSel(pTextServices, -1, -1);
#if (defined(UNICODE) || defined(_UNICODE))
	RichEdit_ReplaceSel(pTextServices, cText, 
		L"����", 9, crName, FALSE, FALSE, FALSE, FALSE, 0);
#else
	LPCWSTR pszTextW = AnsiToUnicode(cText);
	if (pszTextW != NULL)
	{
		RichEdit_ReplaceSel(pTextServices, pszTextW, 
			L"����", 9, crName, FALSE, FALSE, FALSE, FALSE, 0);
	}
#endif
	
	//m_pRichEditView->SetAutoURLDetect(true);
	RichEdit_SetSel(pTextServices, -1, -1);
	AddMsg(m_pRichEditView, lpText, 300);

	RichEdit_SetSel(pTextServices, -1, -1);
	RichEdit_ReplaceSel(pTextServices, L"\r\n");
	RichEdit_SetStartIndent(pTextServices, 0);
	m_pRichEditView->EndDown();

	pTextServices->Release();
}

void CChatDlg::AddTipMsgToRichEditView(LPCTSTR lpText)
{
	if (NULL == lpText || NULL == *lpText)
		return;

	ITextServices * pTextServices = m_pRichEditView->GetTextServices();

	RichEdit_SetSel(pTextServices, -1, -1);
	AddMsg(m_pRichEditView, lpText, 0);
	
	RichEdit_SetSel(pTextServices, -1, -1);
	RichEdit_ReplaceSel(pTextServices, L"\r\n");
	RichEdit_SetStartIndent(pTextServices, 0);
	m_pRichEditView->EndDown();

	pTextServices->Release();
}

void CChatDlg::OnBtnEmotion(TNotifyUI& msg)
{
	CFaceSelDlg* pFaceDlg = Singleton<CFaceSelDlg>::getInstance();

	if (!IsWindow(pFaceDlg->GetHWND())) {
		pFaceDlg->Create(NULL, NULL, WS_CHILD|WS_POPUP, WS_EX_TOOLWINDOW);
	}
	pFaceDlg->SetNotifyWnd(m_hWnd);

	RECT		rcBtn;
	SIZE		cWnd = {432, 236};
	rcBtn = msg.pSender->GetPos();
	::ClientToScreen(m_hWnd, (LPPOINT)&rcBtn);
	POINT		pt = {rcBtn.left - cWnd.cx / 2, rcBtn.top - cWnd.cy};
	if (pt.x < 0) { pt.x = 0; }
	::SetWindowPos((HWND)*pFaceDlg, NULL, pt.x, pt.y, cWnd.cx, cWnd.cy, NULL);
	::ShowWindow((HWND)*pFaceDlg, SW_SHOW);
}

void CChatDlg::OnBtnImage(TNotifyUI& msg)
{
	TCHAR szFile[MAX_BUF] = {0};
	TCHAR szFilter[] = {
		_T("Image Files\0*.bmp;*.dib;*.rle;*.jpg;*.jpeg;*.jpe;*.jfif;*.gif;*.emf;*.wmf;*.tif;*.tiff;*.png;\0")
		_T("*.BMP;*.DIB;*.RLE\0*.bmp;*.dib;*.rle\0")
		_T("*.JPG;*.JPEG;*.JPE;JFIF\0*.jpg;*.jpeg;*.jpe;*.jfif\0")
		_T("*.GIF\0*.gif\0")
		_T("*.EMF\0*.emf\0")
		_T("*.WMF\0*.wmf\0")
		_T("*.TIF;*.TIFF\0*.tif;*.tiff\0")
		_T("*.PNG\0*.png\0")
	};

	OPENFILENAME ofn;      
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.Flags			  = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST
								| OFN_ALLOWMULTISELECT | OFN_EXPLORER;
	ofn.lStructSize       = sizeof(ofn);
	ofn.hwndOwner         = m_hWnd;
	ofn.lpstrFile         = szFile;
	ofn.nMaxFile          = sizeof(szFile);
	ofn.lpstrFilter       = szFilter;
	ofn.nFilterIndex      = 1;
	ofn.lpstrFileTitle    = NULL;
	ofn.nMaxFileTitle     = 0;
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrDefExt		  = NULL;

	if (GetOpenFileName(&ofn) == FALSE)
	{
		return ;
	}

	TCHAR	szPath[MAX_PATH] = {0};
	LPTSTR	psz = ofn.lpstrFile + ofn.nFileOffset;

	*(psz -1) = _T('\0');	
	while (*psz != 0) 
	{
		wsprintf(szPath, _T("%s\\%s"), ofn.lpstrFile, psz); 	
		psz += _tcslen(psz) + 1;

		_RichEdit_InsertFace(m_pRichEditInput, szPath, -1, -1);
	}	
}

void CChatDlg::OnItemSelectComboFontName(TNotifyUI& msg)
{
	Config*	pCfg = Singleton<Config>::getInstance();
	ASSERT(pCfg);
	CDuiString strFontName = m_pComboFontName->GetText();
	if (_tcscmp(strFontName, pCfg->cfLite.szFaceName) != 0) {
		_tcsncpy(pCfg->cfLite.szFaceName, strFontName, LF_FACESIZE);
		_RichEdit_SetFont(m_pRichEditInput, pCfg->cfLite);
	}
}

void CChatDlg::OnItemSelectComboFontSize(TNotifyUI& msg)
{
	Config*	pCfg = Singleton<Config>::getInstance();
	ASSERT(pCfg);
	CDuiString	strFontSize = m_pComboFontSize->GetText();
	int			nFontSize = _ttoi(strFontSize);
	if (nFontSize != pCfg->cfLite.nSize) {
		pCfg->cfLite.nSize = nFontSize;
		_RichEdit_SetFont(m_pRichEditInput, pCfg->cfLite);
	}
}

void CChatDlg::OnBtnBold(TNotifyUI& msg)
{
	Config*	pCfg = Singleton<Config>::getInstance();
	ASSERT(pCfg);

	// ���������ַ�������Ȼ���������촰����ͬʱ��������ť�������ˡ�
	//pCfg->cfLite.bBold = !pCfg->cfLite.bBold;				
	COptionUI* pOptionBtn = static_cast<COptionUI*>(msg.pSender);
	// �ȴ�����Ϣ��UI�ٱ��״̬�����������ȡ��
	pCfg->cfLite.bBold = !pOptionBtn->IsSelected();
	_RichEdit_SetFont(m_pRichEditInput, pCfg->cfLite);
}

void CChatDlg::OnBtnItalic(TNotifyUI& msg)
{
	Config*	pCfg = Singleton<Config>::getInstance();
	ASSERT(pCfg);
	COptionUI* pOptionBtn = static_cast<COptionUI*>(msg.pSender);
	pCfg->cfLite.bItalic = !pOptionBtn->IsSelected();
	_RichEdit_SetFont(m_pRichEditInput, pCfg->cfLite);
}

void CChatDlg::OnBtnUnderLine(TNotifyUI& msg)
{
	Config*	pCfg = Singleton<Config>::getInstance();
	ASSERT(pCfg);
	COptionUI* pOptionBtn = static_cast<COptionUI*>(msg.pSender);
	pCfg->cfLite.bUnderLine = !pOptionBtn->IsSelected();
	_RichEdit_SetFont(m_pRichEditInput, pCfg->cfLite);
}

void CChatDlg::OnBtnColor(TNotifyUI& msg)
{
	static COLORREF rgbCustomColors[16] =
	{
		RGB(255, 255, 255), RGB(255, 255, 255), 
		RGB(255, 255, 255), RGB(255, 255, 255), 
		RGB(255, 255, 255), RGB(255, 255, 255), 
		RGB(255, 255, 255), RGB(255, 255, 255), 
		RGB(255, 255, 255), RGB(255, 255, 255), 
		RGB(255, 255, 255), RGB(255, 255, 255), 
		RGB(255, 255, 255), RGB(255, 255, 255), 
		RGB(255, 255, 255), RGB(255, 255, 255), 
	};

	CHOOSECOLOR cc = {0};
	cc.lStructSize = sizeof(cc);
	cc.lpCustColors = rgbCustomColors;
	cc.hwndOwner = m_hWnd;
	cc.Flags = CC_RGBINIT;

	if (!::ChooseColor(&cc))
	{
		return ;
	}

	COLORREF	clrText = cc.rgbResult;
	Config*		pCfg = Singleton<Config>::getInstance();

	ASSERT(pCfg);
	if (pCfg->cfLite.crText != clrText)
	{
		pCfg->cfLite.crText = clrText;
		_RichEdit_SetFont(m_pRichEditInput, pCfg->cfLite);
	}
}

void CChatDlg::OnBtnFile(TNotifyUI& msg)
{
	OnBrowserFile();
}

void CChatDlg::OnBtnFileRight(TNotifyUI& msg)
{
	CMenuWnd*	pMenu = new CMenuWnd(m_hWnd);
	RECT		rcBtn = msg.pSender->GetPos();
	POINT		pt = {rcBtn.left, rcBtn.bottom};

	::ClientToScreen(m_hWnd, &pt);
	if (pMenu != NULL)
	{
		STRINGorID	xml(_T("xmls\\menu_send_file.xml"));
		pMenu->Init(NULL, xml, _T("xml"), pt);
	}
}


void CChatDlg::OnBtnScreenShots(TNotifyUI& msg)
{
	
}

void CChatDlg::OnBrowserFile()
{
	CMainDlg* pMainDlg = Singleton<CMainDlg>::getInstance();
	if (pMainDlg->BrowserFile(m_hWnd, &m_pShareInfo))
	{
		SetButtonFileShareText(true);
	}
}

void CChatDlg::OnBrowserFolder()
{
	CMainDlg* pMainDlg = Singleton<CMainDlg>::getInstance();
	if (pMainDlg->BrowserFolder(m_hWnd, &m_pShareInfo))
	{
		SetButtonFileShareText(true);
	}
}

void CChatDlg::SetButtonFileShareText(bool bVisible)
{
	//��Ҫ���͵��ļ�/�ļ�����ʾ����ť��
	if (m_pBtnFileShare == NULL) {
		return ;
	}
	if (bVisible)
	{
		char buf[MAX_BUF / 2];
		memset(buf, 0, MAX_BUF / 2);
		MakeShareStr(buf, m_pShareInfo, MAX_BUF / 2);
		m_pBtnFileShare->SetText(buf);
	}	
	m_pBtnFileShare->SetVisible(bVisible);
}

BOOL CChatDlg::MakeShareStr(char *dest, ShareInfo *shareInfo, int maxLen)
{
	char buf[MAX_PATH] = {0};
	int offset = 0;

	for (int i = 0; i < shareInfo->fileCnt; i++)
	{
		ForcePathToFname(buf, shareInfo->fileInfo[i]->fname);
		offset += wsprintf(dest + offset, _T("<%s> "), buf);

		if (offset >= maxLen)
		{
			return FALSE;		
		}
	}
	return TRUE;
}

void CChatDlg::AdjustWindowSize(int nRightPartWidth)
{
	int	nWidth = m_pVLayoutRightPanel->GetFixedWidth();
	if (nWidth != nRightPartWidth)
	{
		RECT	rcWnd;
		GetWindowRect(m_hWnd, &rcWnd);
		rcWnd.right += (nRightPartWidth - nWidth);
		MoveWindow(m_hWnd, rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, 
			rcWnd.bottom - rcWnd.top, FALSE);
		m_pVLayoutRightPanel->SetFixedWidth(nRightPartWidth);
	}	
}

BOOL CChatDlg::AddSendFileTrans(ULONG nPacketNo)
{
	CShareMng*	pShareMng = Singleton<CShareMng>::getInstance();
	if (m_pTLayoutPanel == NULL) {
		return FALSE;
	}
	if (m_pFileTrans == NULL) 
	{
		CreateFileTrans();
	}

	SendFileObj*	sendObj = NULL;
	for (sendObj = pShareMng->TopSendObj(); sendObj; sendObj = pShareMng->NextSendObj(sendObj))
	{
		if (sendObj->packetNo == nPacketNo)
		{
			m_pFileTrans->AddSendFileObj(sendObj);
		}
	}
	return TRUE;
}

// addr port �Է��ĵ�ַ�Ͷ˿ڣ������ֽ���
BOOL CChatDlg::AddRecvFileTrans(ULONG nPacketNo)
{
	CShareMng*	pShareMng = Singleton<CShareMng>::getInstance();
	if (m_pTLayoutPanel == NULL) {
		return FALSE;
	}
	if (m_pFileTrans == NULL) 
	{
		CreateFileTrans();
	}
	RecvFileObj* recvObj = NULL;
	for (recvObj = pShareMng->TopRecvObj(); recvObj; recvObj = pShareMng->NextRecvObj(recvObj))
	{
		// recvObj->packetNo��pShareInfo->packetNo��ͬ�������ͨ��packetNo�ҵ�ShareInfo
		if (recvObj->packetNo == nPacketNo)
		{
			m_pFileTrans->AddRecvFileObj(recvObj);
		}
	}
	return TRUE;
}

void CChatDlg::_RichEdit_SetFont(CRichEditUI* pRichEdit, CharFormatLite& cfLite)
{
	if (pRichEdit == NULL) { return ; }
	ITextServices * pTextServices = pRichEdit->GetTextServices();
#if (defined(UNICODE) || defined(_UNICODE))
	RichEdit_SetDefFont(pTextServices, cfLite.szFaceName, cfLite.nSize, cfLite.crText,
		cfLite.bBold, cfLite.bItalic, cfLite.bUnderLine, FALSE);
#else
	LPCWSTR pszFaceNameW = AnsiToUnicode(cfLite.szFaceName);
	if (pszFaceNameW != NULL)
	{
		RichEdit_SetDefFont(pTextServices, pszFaceNameW, cfLite.nSize, cfLite.crText,
			cfLite.bBold, cfLite.bItalic, cfLite.bUnderLine, FALSE);
		delete pszFaceNameW;
	}
#endif

	pTextServices->Release();
}


void CChatDlg::OnBtnSendMsg()
{
	Config*			pCfg = Singleton<Config>::getInstance();
	CMainDlg*		pMainDlg = Singleton<CMainDlg>::getInstance();
	ITextServices*	pTextServices = m_pRichEditInput->GetTextServices();

	// ���m_pHostҲ�Ǳ�����û�������Ϣ����OLE��Ϣ��������Ϣ��
	wstring	strTextW;
	RichEdit_GetText(pTextServices, strTextW, m_pHost->hostStatus & IM_FXIMOPT);

	pTextServices->Release();

	if (strTextW.size() == 0 && m_pShareInfo == NULL)
		return;

	string strTextA;

	if (strTextW.size() > 0 && m_pHost->hostStatus & IM_FXIMOPT) 
	{
		tstring strFontString = EncodeFontString(pCfg->cfLite);
#if (defined(UNICODE) || defined(_UNICODE))
		LPCSTR pszFontStringA = UnicodeToAnsi(strFontString.data());
		if (pszFontStringA != NULL)
		{
			strTextA = pszFontStringA;
			delete pszFontStringA;
		}
#else
		strTextA = strFontString;
#endif
	}

	LPCTSTR pszTextA = UnicodeToAnsi(strTextW.data());
	if (pszTextA == NULL)
	{
		return ;
	}
	strTextA += pszTextA;
	delete pszTextA;
	DisposeLocalMsg(strTextA);
	
	if (strTextA.length() >= MAX_UDPBUF) 
	{
		::MessageBox(m_hWnd, _T("��Ϣ����"), APPNAME_STR, MB_OK);
		return ;
	}
	strncpy(m_szMsgBuf, strTextA.c_str(), strTextA.length() + 1);

	m_pRichEditInput->SetText(_T(""));
	m_pRichEditInput->SetFocus();

	if (strTextA.size() > 0)
	{
#if (defined(UNICODE) || defined(_UNICODE))
		LPCWSTR pszMsgW = AnsiToUnicode(strTextA.c_str());
		LPCWSTR pszNickNameW = AnsiToUnicode(pMainDlg->GetNickNameEx());
		if (pszMsgW != NULL && pszNickNameW != NULL)
		{
			AddMsgToRichEditView(pszMsgW, A2W(), time(NULL), RGB(0, 128, 64));
			delete pszMsgW;
			delete pszNickNameW;
		}		
#else
		AddMsgToRichEditView(strTextA.c_str(), pMainDlg->GetNickNameEx(), time(NULL), RGB(0, 128, 64));
#endif
	}

	ULONG	nLocalStatus;
	m_sendEntry.host = m_pHost;
	m_sendEntry.command = IM_SENDMSG | (pCfg->bSendCheckOpt ? IM_SENDCHECKOPT : 0);
	m_sendEntry.command |= ((m_pShareInfo && m_pShareInfo->fileCnt > 0) ? IM_FILEATTACHOPT : 0);
	
	nLocalStatus = (pCfg->pubKey.key || pCfg->smallPubKey.key) ? IM_ENCRYPTOPT : 0;	
	m_sendEntry.status = (nLocalStatus & m_pHost->hostStatus) ? m_pHost->pubKey.key == NULL?
		ST_GETCRYPT : ST_MAKECRYPTMSG: ST_MAKEMSG;
	m_sendEntry.command |= (m_sendEntry.status == ST_MAKEMSG) ? 0 : IM_ENCRYPTOPT;

	if (m_sendEntry.command & IM_FILEATTACHOPT)
	{
		EncodeShareMsg(m_szShare, m_pShareInfo, MAX_UDPBUF / 2);
	}
	SendMsgSub();
}

void CChatDlg::OnBtnSendMsgRight(TNotifyUI& msg)
{
	CMenuWnd*	pMenu = new CMenuWnd(m_hWnd);
	Config*		pCfg = Singleton<Config>::getInstance();
	RECT		rcBtn = msg.pSender->GetPos();
	POINT		pt = {rcBtn.left, rcBtn.bottom};
	::ClientToScreen(m_hWnd, &pt);
	if (pMenu != NULL)
	{
		STRINGorID	xml(_T("xmls\\menu_send_right.xml"));

		pMenu->Init(NULL, xml, _T("xml"), pt);
		if (pCfg->bWantReturnSend)
			pMenu->CheckMenuItem(_T("menu_send_wantreturn"), true);
		else
			pMenu->CheckMenuItem(_T("menu_send_ctrl_wantreturn"), true);
	}
}

void CChatDlg::SendMsgSub()
{
	switch (m_sendEntry.status)
	{
	case ST_GETCRYPT:
		SendGetPubKeyMsg();
		break;
	case ST_MAKECRYPTMSG:
		m_sendEntry.command |= IM_ENCRYPTOPT;
		EncryptMsg(m_sendEntry.msg, m_sendEntry.host, m_szMsgBuf);
		m_sendEntry.status = ST_SENDMSG;
		break;
	case ST_MAKEMSG:
		m_sendEntry.command &= ~IM_ENCRYPTOPT;
		strncpy(m_sendEntry.msg, m_szMsgBuf, MAX_UDPBUF);
		m_sendEntry.status = ST_SENDMSG;		
		break;
	}
	if (m_sendEntry.status == ST_SENDMSG) 
	{
		CMsgMng*	pMsgMng = Singleton<CMsgMng>::getInstance();
		CShareMng*	pShareMng = Singleton<CShareMng>::getInstance();
		// ���ﲻ����pMsgMng->Send(&m_pHost->hostSub, m_sendEntry.command, m_sendEntry.msg, m_szShare);
		// ���ַ�ʽ������Ϣ����Ϊ�޷���֤PacketNo��һ����
		char msgBuf[MAX_UDPBUF] = {0};
		int  len;
		
		// Ҫ��֤ShareInfo�е�PacketNo����Ϣ�е�PacketNoһ�£��Է������ļ�ʱ��������Ϣ��PacketNo���ͷ�����Ҫ������ļ�(��)
		len = pMsgMng->MakeMsg(msgBuf, m_pShareInfo ? m_pShareInfo->packetNo : pMsgMng->MakePacketNo(), 
			m_sendEntry.command, m_sendEntry.msg, m_szShare);
		pMsgMng->UdpSend(&m_pHost->hostSub, msgBuf, len);

		if (m_pShareInfo != NULL && m_sendEntry.command & IM_FILEATTACHOPT)
		{
			//AddSendFileObjToShareWnd();
			pShareMng->AddHostShare(m_pShareInfo, &m_sendEntry, 1);
			pShareMng->AddSendFileObjs(m_pShareInfo, m_hWnd);
			AddSendFileTrans(m_pShareInfo->packetNo);

			SetButtonFileShareText(false);
		}

		// ������Ϣ�е�ͼƬ
		if (m_pShareInfoImage != NULL && m_pHost->hostStatus & IM_FXIMOPT)
		{
			char szImages[MAX_UDPBUF / 2];

			EncodeShareMsg(szImages, m_pShareInfoImage, MAX_UDPBUF / 2);
			len = pMsgMng->MakeMsg(msgBuf, m_pShareInfoImage->packetNo, 
				IM_SENDMSGIMAGE | IM_FILEATTACHOPT, NULL, szImages);
			pMsgMng->UdpSend(m_pHost->hostSub.addr, m_pHost->hostSub.portNo, msgBuf, len);

			pShareMng->AddHostShare(m_pShareInfoImage, &m_sendEntry, 1);
			pShareMng->AddSendFileObjs(m_pShareInfoImage, m_hWnd);
		}

		memset(m_szMsgBuf, 0, MAX_UDPBUF);
		memset(m_szShare, 0, MAX_UDPBUF / 2);
		m_pShareInfo = NULL;
		m_pShareInfoImage = NULL;
		m_sendEntry.status = ST_DONE;
	}
}

void CChatDlg::DisposeLocalMsg(tstring& strText)
{
	LPCTSTR		psz = strText.data();
	tstring		strNewText = strText;
	tstring		strCustomPic;
	TCHAR		szMd5[MAX_MD5] = {0};
	TCHAR		szNewFileName[MAX_NAMEBUF] = {0};
	TCHAR		szNewFilePath[MAX_PATH] = {0};
	Config*		pCfg = Singleton<Config>::getInstance();
	CShareMng*	pShareMng = Singleton<CShareMng>::getInstance();
	CMsgMng*	pMsgMng = Singleton<CMsgMng>::getInstance();
	
	while ((psz = _tcsstr(psz, _T("/c"))) != NULL)
	{
		strCustomPic = GetBetweenString(psz + 2, _T("[\""), _T("\"]"));
		if (!strCustomPic.empty())
		{
			if (m_pShareInfoImage == NULL) {
				m_pShareInfoImage = pShareMng->CreateShare(pMsgMng->MakePacketNo());
			}
			MD5File(szMd5, strCustomPic.data());
			wsprintf(szNewFileName, _T("%s%s"), szMd5, ::PathFindExtension(strCustomPic.data()));
			wsprintf(szNewFilePath, _T("%s\\%s"), pCfg->szChatImageSaveDir, szNewFileName);
			CopyFile(strCustomPic.data(), szNewFilePath, TRUE);
			
			Replace(strNewText, strCustomPic.data(), szNewFileName);
			if (m_pShareInfoImage) {
				pShareMng->AddFileShare(m_pShareInfoImage, strCustomPic.data(), IM_FILE_CHATIMAGE);
			}
		}
		psz += 2;
	}
	// ��RichEdit��ȡ���ַ������з�Ϊ"\r"���ɸ�Ļ��з�Ϊ"\r\n"���ڱ�������"\r\n"Ϊ�������з� [12/13/2014 ybt]
	ReplaceA(strNewText, _T("\r"), _T("\n"));
	strText = strNewText;	
}


BOOL CChatDlg::EncryptMsg(char *dest, Host *host, const char *src)
{
	ASSERT(dest && host && src);

	Config*		pCfg = Singleton<Config>::getInstance();
	HCRYPTKEY	hExKey = 0;
	HCRYPTKEY	hKey = 0;	
	BYTE		skey[MAX_BUF] = {0};
	BYTE		data[MAX_UDPBUF] = {0};
	int			len = 0;
	int			capa = host->pubKey.capa & (pCfg->pubKey.capa | pCfg->smallPubKey.capa);
	HCRYPTPROV	target_csp = (capa & IM_RSA_1024) ? pCfg->hCsp : pCfg->hSmallCsp;	
	DWORD 		msgLen;

	if ((capa & IM_RSA_1024) && (capa & IM_BLOWFISH_128))		//RSA/BlowFish
	{
		capa = IM_RSA_1024 | IM_BLOWFISH_128;
	}
	else if ((capa & IM_RSA_512) && (capa & IM_RC2_40))			//RSA/RC2
	{
		capa = IM_RSA_512 | IM_RC2_40;
	}else{
		return	FALSE;
	}

	host->pubKey.KeyBlob(data, sizeof(data), &len);		//����Կ��Ϣд�뵽data��

	if (!CryptImportKey(target_csp, data, len, 0, 0, &hExKey))
	{
		return ShowErrorInfo(GetLastError(), _T("pCryptImportKey")), FALSE;
	}

	if (capa & IM_BLOWFISH_128)		//ʹ��Blowfish
	{
		if (!CryptGenRandom(target_csp, len = 128 / 8, data))
		{
			return ShowErrorInfo(GetLastError(), _T("pCryptGenRandom")), FALSE;
		}	

		CBlowFish	blowFish(data, len);

		if (!CryptEncrypt(hExKey, 0, TRUE, 0, data, (DWORD *)&len, MAX_BUF))
		{
			return ShowErrorInfo(GetLastError(), _T("pCryptEncrypt")), FALSE;
		}
		bin2hexstr_bigendian(data, len, (char*)skey);				//��dataת����ʮ�������ַ�

		//TRACE(_T("CIMChatDlg::EncryptMsg�У�\ndata = %s \nskey = %s\n"), (char*)(data + 20), skey);

		CMsgMng::LocalNewLineToUnix((char*)data, src, MAX_CRYPTLEN);		
		msgLen = blowFish.Encrypt(data, data, strlen(src) + 1);		//�ӵ�������������sizeof�����������ҵ���Ϣֻ�ܷ��������ֽ�
	}
	else						//RC2
	{
		if (!CryptGenKey(target_csp, CALG_RC2, CRYPT_EXPORTABLE, &hKey))
		{
			return	ShowErrorInfo(GetLastError(), _T("pCryptGenKey")), FALSE;
		}

		CryptExportKey(hKey, hExKey, SIMPLEBLOB, 0, NULL, (DWORD *)&len);
		if (!CryptExportKey(hKey, hExKey, SIMPLEBLOB, 0, data, (DWORD *)&len))
		{
			return ShowErrorInfo(GetLastError(), _T("pCryptExportKey")), FALSE;
		}

		len -= SKEY_HEADER_SIZE;
		bin2hexstr_bigendian(data + SKEY_HEADER_SIZE, len, (char *)skey);

		if (capa & IM_RC2_40)
		{
			CMsgMng::LocalNewLineToUnix((char*)data, src, MAX_CRYPTLEN);
		}else{
			strncpy((char *)data, src, MAX_CRYPTLEN);	// beta1-4 only
		}

		msgLen = strlen((char *)data) + 1;

		if (!CryptEncrypt(hKey, 0, TRUE, 0, data, &msgLen, MAX_UDPBUF))
		{
			return ShowErrorInfo(GetLastError(), _T("CryptEncrypt RC2")), FALSE;
		}

		CryptDestroyKey(hKey);
	}
	CryptDestroyKey(hExKey);

	wsprintf(dest, "%X:%s:", capa, skey);
	bin2hexstr(data, (int)msgLen, dest + strlen(dest));	
	
	return TRUE;
}

BOOL CChatDlg::DecryptMsg(MsgBuf *msg)
{
	Config*		pCfg = Singleton<Config>::getInstance();
	HCRYPTKEY	hKey=0, hExKey=0;      //typedef unsigned long HCRYPTKEY
	char		*capa_hex, *skey_hex, *msg_hex, *p;
	BYTE		skey[MAX_BUF];
	int			len, msgLen;
	int			cryptCapa;
	HCRYPTPROV	target_csp;

	if ((capa_hex = separate_token(msg->msgBuf, ':', &p)) == NULL)
		return	FALSE;
	cryptCapa = strtoul(capa_hex, 0, 16);
	target_csp = (cryptCapa & IM_RSA_1024) && pCfg->pubKey.Key() ? pCfg->hCsp : pCfg->hSmallCsp;
	hExKey = target_csp == pCfg->hCsp ? pCfg->hPrivKey : pCfg->hSmallPrivKey;

	if ((skey_hex = separate_token(NULL, ':', &p)) == NULL)
		return	FALSE;
	if ((msg_hex = separate_token(NULL, 0, &p)) == NULL)
		return	FALSE;

	if (cryptCapa & IM_BLOWFISH_128) {	// blowfish
		hexstr2bin_bigendian(skey_hex, skey, sizeof(skey), &len);
		//
		if (!CryptDecrypt(hExKey, 0, TRUE, 0, (BYTE *)skey, (DWORD *)&len))
			return	wsprintf(msg->msgBuf, "CryptDecrypt Err(%X)", GetLastError()), FALSE;

		CBlowFish	bl(skey, len);
		hexstr2bin(msg_hex, (BYTE *)msg->msgBuf, sizeof(msg->msgBuf), &msgLen);
		bl.Decrypt((BYTE *)msg->msgBuf, (BYTE *)msg->msgBuf, msgLen);
	}
	else {	// RC2
		// Skey Blob 
		skey[0] = SIMPLEBLOB;
		skey[1] = CUR_BLOB_VERSION;
		*(WORD *)(skey + 2) = 0;
		*(ALG_ID *)(skey + 4) = CALG_RC2;
		*(ALG_ID *)(skey + 8) = CALG_RSA_KEYX;
		hexstr2bin_bigendian(skey_hex, skey + SKEY_HEADER_SIZE, sizeof(skey) - SKEY_HEADER_SIZE, &len);

		//import
		if (!CryptImportKey(target_csp, skey, len + SKEY_HEADER_SIZE, hExKey, 0, &hKey))
			return	wsprintf(msg->msgBuf, "CryptImportKey Err(%X)", GetLastError()), FALSE;

		//Decrypt
		hexstr2bin(msg_hex, (BYTE *)msg->msgBuf, sizeof(msg->msgBuf), &msgLen);
		if (!CryptDecrypt(hKey, 0, TRUE, 0, (BYTE *)msg->msgBuf, (DWORD *)&msgLen))
			return	wsprintf(msg->msgBuf, "CryptDecrypt Err(%X)", GetLastError()), FALSE;
		CryptDestroyKey(hKey);
	}
	// UNIX 
	if (cryptCapa & (IM_BLOWFISH_128|IM_RC2_40))
		CMsgMng::UnixNewLineToLocal(msg->msgBuf, msg->msgBuf, sizeof(msg->msgBuf));

	return	TRUE;
}

void CChatDlg::SendGetPubKeyMsg()
{
	char		spec_str[12] = {0};
	int			spec = IM_RSA_512 | IM_RC2_40;
	Config*		pCfg = Singleton<Config>::getInstance();
	CMsgMng*	pMsgMng = Singleton<CMsgMng>::getInstance();

	if (pCfg->pubKey.key)
	{
		spec |= IM_RSA_1024 | IM_BLOWFISH_128;
	}
	wsprintf(spec_str, _T("%x"), spec);
	pMsgMng->Send(&m_pHost->hostSub, IM_GETPUBKEY, spec_str);	
}

void CChatDlg::SendPubKeyNotify(HostSub *hostSub, BYTE *pubkey, int len, int e, int capa)
{
	if (m_sendEntry.status == ST_GETCRYPT 
		&& m_sendEntry.host->hostSub.addr == hostSub->addr
		&& m_sendEntry.host->hostSub.portNo == hostSub->portNo)
	{
		m_sendEntry.host->pubKey.Set(pubkey, len, e, capa);
		m_sendEntry.status = ST_MAKECRYPTMSG;

		SendMsgSub();
	}
}

void CChatDlg::AddMsgBuf(MsgBuf *msg)
{
	// [12/10/2014 ybt] ������ͷɸ���ļ�(��)���ջ��Ʋ������£�
	// �ɸ�Ļ��ƣ��ɸ���˳����գ�����ĳ�ν��յ���ShareInfo��M��FileInfo(�����û�������)����ֻ��Ӧһ��RecvFileObj��
	// ��RecvFileObj��ָ���FileInfo������ɺ���ָ����һ�����������գ�����M��.
	// ������Ļ��ƣ�������ǲ��н��գ�һ��FileInfo��Ӧһ��RecvFileObj��(����ÿ��FileInfo����Ҫ�����촰���Ҳ�չʾ���ȴ��û�����)
	// ��2010�������ҵ��Ƶ�ʱ�򣬱�����Ľ����ļ����ƾ���������ֻ�ǵ�ʱû�н��˻���д��ע�͡�����������д�����ʱ��������������ơ�
	// �����ļ�(��)���ͻ������ߴ��������Ƶģ�����һ��FileInfo��Ӧһ��SendFileObj��
	if (msg->command & IM_FILEATTACHOPT)
	{
		ShareInfo*	shareInfo = DecodeShareMsg(msg->msgBuf + msg->exOffset);
		Config*		pCfg = Singleton<Config>::getInstance();
		CShareMng*	pShareMng = Singleton<CShareMng>::getInstance();
		CMainDlg*	pMainDlg = Singleton<CMainDlg>::getInstance();
		if (shareInfo != NULL)
		{
			// DecodeShareMsgû�и�ShareInfo��packetNo��ֵ
			shareInfo->packetNo = msg->packetNo;			
			pShareMng->AddRecvFileObjs(shareInfo, pMainDlg->GetHWND(), m_hWnd, 
				msg->hostSub.addr, msg->hostSub.portNo);

			if (GET_MODE(msg->command) == IM_SENDMSG)
			{
				// ��ӵ������Ҳ����ȴ��û�����
				AddRecvFileTrans(shareInfo->packetNo);
			}
			else {
				// ���������ͼƬ���Զ�����
				pShareMng->SaveRecvFileObjs(shareInfo->packetNo, pCfg->szChatImageSaveDir);
			}

			// ��SendFileObj��һ����RecvFileObj��fileInfo�����ָ��ShareInfo�е�ĳ��fileInfo����������������һ���ռ�
			// �ڽ�����ɺ󣬻�����ɾ���� �����������ͷ�ShareInfo
			FreeDecodeShareMsg(shareInfo);
		}
	}

	if (msg->command & IM_ENCRYPTOPT)
	{
		DecryptMsg(msg);	
	}

	if (_tcslen(msg->msgBuf) > 0) 
	{
#if (defined(UNICODE) || defined(_UNICODE))
		LPCWSTR pszMsgBufW = AnsiToUnicode(msg->msgBuf);
		LPCWSTR pszNickNameExW = AnsiToUnicode(m_pHost->NickNameEx());
		if (pszMsgBufW != NULL && pszNickNameExW != NULL)
		{
			AddMsgToRichEditView(pszMsgBufW, pszNickNameExW, 
				msg->recvTime, RGB(0, 0, 255));
			delete pszMsgBufW;
			delete pszNickNameExW;
		}		
#else
		AddMsgToRichEditView(msg->msgBuf, m_pHost->NickNameEx(), 
			msg->recvTime, RGB(0, 0, 255));
#endif
	}
}

void CChatDlg::ShowFlashWindow()
{
	if (m_hWnd != ::GetLastActivePopup(::GetForegroundWindow()))
	{
		FlashWindow(m_hWnd, TRUE);
		SetTimer(m_hWnd, IM_FLASHWINDOW_TIMER, 500, NULL);
	}
}

void CChatDlg::SetShareInfo(ShareInfo* pShareInfo)
{
	m_pShareInfo = pShareInfo;
	SetButtonFileShareText(true);
}

bool CChatDlg::AddPanel(LPCTSTR pszText, CControlUI* pControlPanel,
						bool bHasCloseBtn, LPCTSTR pszCloseBtnName)
{
	COptionUI*	pOption = new COptionUI;
	TCHAR		szName[MAX_PATH] = {0};

	if (m_pHLayoutOption == NULL || !m_pHLayoutOption->Add(pOption))
	{
		return false;
	}
	wsprintf(szName, _T("option_panel_%d"), time(NULL));
	pOption->SetName(szName);
	pOption->SetTag((UINT_PTR)pControlPanel);
	pOption->SetFloat(true);	
	pOption->SetText(pszText);
	pOption->SetNormalImage(_T("chatdlg\\option_tab_normal.png"));
	pOption->SetHotImage(_T("chatdlg\\option_tab_hover.png"));
	pOption->SetSelectedImage(_T("chatdlg\\option_tab_selected.png"));
	pOption->SetGroup(_T("group_panel"));

	if (m_pTLayoutPanel->FindSubControl(pControlPanel->GetName()) == NULL)
	{
		m_pTLayoutPanel->Add(pControlPanel);
	}	

	if (m_mapButtonClose.size() > 0)
	{
		pOption->Selected(true);
	}	
	m_pTLayoutPanel->SelectItem(pControlPanel);

	if (bHasCloseBtn)
	{
		CButtonUI*	pButtonClose = new CButtonUI;
		if (!m_pHLayoutOption->Add(pButtonClose))
		{
			return false;
		}
//		wsprintf(szName, _T("btn_panel_close_%d"), time(NULL));
		m_mapButtonClose[pOption] = pButtonClose;
		pButtonClose->SetName(pszCloseBtnName);
		pButtonClose->SetFloat(true);
		pButtonClose->SetTag((UINT_PTR)pOption);
		pButtonClose->SetNormalImage(_T("chatdlg\\close_normal.png"));
		pButtonClose->SetHotImage(_T("chatdlg\\close_hover.png"));
		pButtonClose->SetPushedImage(_T("chatdlg\\close_down.png"));
		pButtonClose->SetFixedWidth(15);
		pButtonClose->SetFixedHeight(15);
	}

	AdjustLayoutOption();
	return true;
}

void CChatDlg::AdjustLayoutOption()
{
	int			nIndex, nOptionIndex;
	int			nWidth;
	int			nOffset;
	int			nOptionCount = 0;
	bool		bOnlyOne;
	CDuiRect	rcOption;
	CDuiRect	rcButtonClose;
	COptionUI*	pOption;
	CControlUI*	pControl;
	CButtonUI*	pButtonClose;
	MapControl::iterator	iterOption;

	// Tab�ؼ����Ǿ���λ�ã����m_rcItem��ֵΪ�ա� [12/25/2014 ybt]
	// height="22" width="150" ������Ӧ����m_cxyFixed��ֵ
	for (nIndex = 0; nIndex < m_pHLayoutOption->GetCount(); nIndex++)
	{
		pControl = m_pHLayoutOption->GetItemAt(nIndex);
		if (pControl->GetInterface(DUI_CTR_OPTION) != NULL && pControl->IsVisible())
		{
			nOptionCount++;
		}
	}
	bOnlyOne = (nOptionCount == 1);
	nWidth = m_pHLayoutOption->GetFixedWidth() / nOptionCount;

	nOptionIndex = 0;
	for (nIndex = 0; nIndex < m_pHLayoutOption->GetCount(); nIndex++)
	{
		pControl = m_pHLayoutOption->GetItemAt(nIndex);
		if (pControl->GetInterface(DUI_CTR_OPTION) == NULL || !pControl->IsVisible())
		{
			continue;
		}		
		pOption = static_cast<COptionUI*>(pControl);
		pOption->SetMouseEnabled(!bOnlyOne);			// ���ֻ��һ����ǩ���ñ�ǩ���ܵ��

		rcOption.left = nWidth * nOptionIndex;
		rcOption.top = 0;	
		if (nIndex == nOptionCount - 1)
		{
			rcOption.right = m_pHLayoutOption->GetFixedWidth();
		}
		else 
		{
			rcOption.right = nWidth * (nOptionIndex + 1);
		}
		rcOption.bottom = m_pHLayoutOption->GetFixedHeight();

		SIZE szXY = {rcOption.left >= 0 ? rcOption.left : rcOption.right, 
			rcOption.top >= 0 ? rcOption.top : rcOption.bottom};
		pOption->SetFixedXY(szXY);
		pOption->SetFixedWidth(rcOption.GetWidth());
		pOption->SetFixedHeight(rcOption.GetHeight());
		//pOption->SetVisible(false);
		//pOption->SetPos(rcOption);

		pButtonClose = static_cast<CButtonUI*>(m_mapButtonClose[pOption]);
		if (pButtonClose != NULL)
		{
			nOffset = (rcOption.GetHeight() - pButtonClose->GetFixedHeight()) / 2;
			rcButtonClose.right = rcOption.right - nOffset;
			rcButtonClose.left = rcButtonClose.right - pButtonClose->GetFixedHeight();
			rcButtonClose.top = rcOption.top + nOffset;
			rcButtonClose.bottom = rcOption.bottom - nOffset;
			//pButtonClose->SetPos(rcButtonClose);
			SIZE szXYClose = {rcButtonClose.left >= 0 ? rcButtonClose.left : rcButtonClose.right, 
				rcButtonClose.top >= 0 ? rcButtonClose.top : rcButtonClose.bottom};
			pButtonClose->SetFixedXY(szXYClose);
			//pButtonClose->SetFixedWidth(rcButtonClose.GetWidth());
			//pButtonClose->SetFixedHeight(rcButtonClose.GetHeight());
		}

		nOptionIndex++;
	}
}

void CChatDlg::CreateFileTrans()
{
	AdjustWindowSize(kRightPartLargeWidth);
	m_pHLayoutOption->SetFixedWidth(kRightPartLargeWidth);

	m_pFileTrans = new CFileTransUI();
	AddPanel(_T("�ļ�����"), m_pFileTrans, true, kButtonCloseFileTrans);

	// ��ʹ��AddPanel��ӵ�TabLayout��֮ǰ��m_pFileTrans->m_pManager��ֵ��NULL��
	// ���ΪNULL��EnableScrollBar�Ĺ��������޷�ʹ��Default ScrollBar��Ϣ  [12/26/2014 ybt]
	m_pFileTrans->EnableScrollBar(true, false);
}


void CChatDlg::CloseFileTrans(TNotifyUI& msg)
{
	COptionUI*	pOptionFileTrans = reinterpret_cast<COptionUI*>(msg.pSender->GetTag());
	CButtonUI*	pButtonClose = static_cast<CButtonUI*>(msg.pSender);

	if (m_pFileTrans->GetShareCount() > 0)
	{
		if (IDOK != ::MessageBox(m_hWnd, _T("���ڴ����ļ�/�ļ��У��Ƿ�����ر�?"), APPNAME_STR, MB_OKCANCEL))
		{
			return ;
		}		
		m_pFileTrans->RemoveAllShareElement();
	}
	//m_pTLayoutPanel->Remove(m_pFileTrans);
	m_pTLayoutPanel->SelectItem(m_pVLayoutPersonal);
	

	// Duilib������Ҫ�Լ�ȥdelete�Ѿ�Remove�Ŀؼ�����Ϊ��Container::Remove()�������Ѿ�����Ӧ�ؼ���ӵ���ɾ���ؼ����С�
	// ����CPaintManagerUI::MessageHandler�е���ϢWM_APP + 1 ��ɾ����Щ�ؼ��� [12/26/2014 ybt] 
	if (m_pTLayoutPanel->Remove(m_pFileTrans))
	{
		m_pFileTrans = NULL;
	}
	
	if (pOptionFileTrans && pButtonClose)
	{
		m_mapButtonClose.erase(pOptionFileTrans);
		m_pHLayoutOption->Remove(pOptionFileTrans);
		m_pHLayoutOption->Remove(pButtonClose);
	}

	if (m_pTLayoutPanel->GetCount() == 1)
	{
		AdjustWindowSize(kRightPartNormalWidth);
	}
	AdjustLayoutOption();
}