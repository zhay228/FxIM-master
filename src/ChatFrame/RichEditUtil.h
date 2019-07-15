#pragma once

#include <string>
#include "Utils/Utils.h"
#include "ChatFrame/IImageOle.h"

#if defined(UNICODE) || defined(_UNICODE)
	typedef std::wstring tstring;
#else
	typedef std::string tstring;
#endif

IRichEditOle* RichEdit_GetOleInterface(HWND hWnd);
int RichEdit_GetWindowTextLength(HWND hWnd);
int RichEdit_GetWindowText(HWND hWnd, LPTSTR lpszStringBuf, int nMaxCount);
int RichEdit_GetWindowText(HWND hWnd, tstring& strText);
int RichEdit_GetTextRange(HWND hWnd, CHARRANGE * lpchrg, tstring& strText);
DWORD RichEdit_GetDefaultCharFormat(HWND hWnd, CHARFORMAT& cf);
BOOL RichEdit_SetDefaultCharFormat(HWND hWnd, CHARFORMAT& cf);
DWORD RichEdit_GetSelectionCharFormat(HWND hWnd, CHARFORMAT& cf);
BOOL RichEdit_SetSelectionCharFormat(HWND hWnd, CHARFORMAT& cf);
void RichEdit_ReplaceSel(HWND hWnd, LPCTSTR lpszNewText, BOOL bCanUndo = FALSE);
void RichEdit_GetSel(HWND hWnd, LONG& nStartChar, LONG& nEndChar);
int RichEdit_SetSel(HWND hWnd, LONG nStartChar, LONG nEndChar);

void RichEdit_SetDefFont(HWND hWnd, LPCTSTR lpFontName, int nFontSize,	// ����Ĭ������
						 COLORREF clrText, BOOL bBold, BOOL bItalic, 
						 BOOL bUnderLine, BOOL bIsLink);
void RichEdit_SetDefFont(HWND hWnd, LPCTSTR lpFontName, int nFontSize);	// ����Ĭ���������ƺʹ�С
void RichEdit_SetDefTextColor(HWND hWnd, COLORREF clrText);				// ����Ĭ��������ɫ
void RichEdit_SetDefLinkText(HWND hWnd, BOOL bEnable);					// ����Ĭ�ϳ�����
void RichEdit_SetFont(HWND hWnd, LPCTSTR lpFontName, int nFontSize,		// ��������
					  COLORREF clrText, BOOL bBold, BOOL bItalic, 
					  BOOL bUnderLine, BOOL bIsLink);	
void RichEdit_SetFont(HWND hWnd, LPCTSTR lpFontName, int nFontSize);	// �����������ƺʹ�С
void RichEdit_SetTextColor(HWND hWnd, COLORREF clrText);				// ����������ɫ
void RichEdit_SetLinkText(HWND hWnd, BOOL bEnable);						// ���ó�����
BOOL RichEdit_SetStartIndent(HWND hWnd, int nSize);						// ����������(��λ:�)
BOOL RichEdit_InsertFace(HWND hWnd, LPCTSTR lpszFileName, int nFaceId,	// �������ͼƬ
						 int nFaceIndex, COLORREF clrBg, BOOL bAutoScale, int nReservedWidth);
void RichEdit_GetText(HWND hWnd, tstring& strText);						// ��ȡ�ı�
void RichEdit_ReplaceSel(HWND hWnd, LPCTSTR lpszNewText, LPCTSTR lpFontName,	// �滻ѡ���ı�
						 int nFontSize,	COLORREF clrText, BOOL bBold, BOOL bItalic, 
						 BOOL bUnderLine, BOOL bIsLink, int nStartIndent, BOOL bCanUndo = FALSE);
BOOL RichEdit_GetImageOle(HWND hWnd, POINT pt, IImageOle** pImageOle);


IRichEditOle* RichEdit_GetOleInterface(ITextServices * pTextServices);
int RichEdit_GetWindowTextLength(ITextServices * pTextServices);
int RichEdit_GetWindowText(ITextServices * pTextServices, LPWSTR lpszStringBuf, int nMaxCount);
int RichEdit_GetWindowText(ITextServices * pTextServices, wstring& strText, BOOL bIncludeOleInfo);
int RichEdit_GetTextRange(ITextServices * pTextServices, CHARRANGE * lpchrg, wstring& strText);
DWORD RichEdit_GetDefaultCharFormat(ITextServices * pTextServices, CHARFORMATW& cf);
BOOL RichEdit_SetDefaultCharFormat(ITextServices * pTextServices, CHARFORMATW& cf);
DWORD RichEdit_GetSelectionCharFormat(ITextServices * pTextServices, CHARFORMATW& cf);
BOOL RichEdit_SetSelectionCharFormat(ITextServices * pTextServices, CHARFORMATW& cf);
void RichEdit_ReplaceSel(ITextServices * pTextServices, LPCWSTR lpszNewText, BOOL bCanUndo = FALSE);
void RichEdit_GetSel(ITextServices * pTextServices, LONG& nStartChar, LONG& nEndChar);
int RichEdit_SetSel(ITextServices * pTextServices, LONG nStartChar, LONG nEndChar);

void RichEdit_SetDefFont(ITextServices * pTextServices, LPCWSTR lpFontName,		// ����Ĭ������
						 int nFontSize,	COLORREF clrText, BOOL bBold, 
						 BOOL bItalic, BOOL bUnderLine, BOOL bIsLink);
void RichEdit_SetFont(ITextServices * pTextServices, LPCWSTR lpFontName, int nFontSize,	// ��������
					  COLORREF clrText, BOOL bBold, BOOL bItalic, BOOL bUnderLine, BOOL bIsLink);
BOOL RichEdit_SetStartIndent(ITextServices * pTextServices, int nSize);			// ����������(��λ:�)
BOOL RichEdit_InsertFace(ITextServices *pTextServices, ITextHost *pTextHost,	// �������ͼƬ
						 LPCWSTR lpszFileName, int nFaceId,	int nFaceIndex, 
						 COLORREF clrBg, BOOL bAutoScale, int nReservedWidth, IImageOle** ppImageOle = NULL);
void RichEdit_GetText(ITextServices * pTextServices, wstring& strText,			// ��ȡ�ı�
						BOOL bIncludeOleInfo);			
void RichEdit_ReplaceSel(ITextServices * pTextServices, LPCWSTR lpszNewText,	// �滻ѡ���ı�
						 LPCWSTR lpFontName, int nFontSize,	COLORREF clrText, 
						 BOOL bBold, BOOL bItalic, BOOL bUnderLine, BOOL bIsLink, 
						 int nStartIndent, BOOL bCanUndo = FALSE);
BOOL RichEdit_GetImageOle(ITextServices * pTextServices, POINT pt, IImageOle** pImageOle);