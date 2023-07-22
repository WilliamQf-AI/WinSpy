// View.h : interface of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TreeViewManager.h"
#include "ViewBase.h"
#include "WindowsListView.h"

class CWindowsView : 
	public CViewBase<CWindowsView>,
	public CTreeViewManager<CWindowsView> {
public:
	CWindowsView(IMainFrame* frame) : CViewBase(frame), m_WindowsView(frame) {}

	BOOL PreTranslateMessage(MSG* pMsg);

	void OnActivate(bool activate);
	LRESULT OnTreeNodeDoubleClick(HTREEITEM hItem, CPoint const& pt);

protected:
	enum { IDC_TREE = 123 };

	BEGIN_MSG_MAP(CWindowsView)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		NOTIFY_CODE_HANDLER(TVN_ITEMEXPANDING, OnNodeExpanding)
		NOTIFY_CODE_HANDLER(TVN_DELETEITEM, OnNodeDeleted)
		NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnNodeSelected)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		COMMAND_ID_HANDLER(ID_VIEW_HIDDENWINDOWS, OnToggleHiddenWindows)
		COMMAND_ID_HANDLER(ID_VIEW_EMPTYTITLEWINDOWS, OnToggleEmptyTitleWindows)
		COMMAND_ID_HANDLER(ID_WINDOW_SHOW, OnWindowShow)
		COMMAND_ID_HANDLER(ID_WINDOW_HIDE, OnWindowHide)
		COMMAND_ID_HANDLER(ID_WINDOW_BRINGTOFRONT, OnWindowBringToFront)
		COMMAND_ID_HANDLER(ID_WINDOW_MINIMIZE, OnWindowMinimize)
		COMMAND_ID_HANDLER(ID_WINDOW_MAXIMIZE, OnWindowMaximize)
		COMMAND_ID_HANDLER(ID_WINDOW_HIGHLIGHT, OnWindowHighlight)
		COMMAND_ID_HANDLER(ID_STATE_FLASH, OnWindowFlash)
		COMMAND_ID_HANDLER(ID_WINDOW_RESTORE, OnWindowRestore)
		COMMAND_ID_HANDLER(ID_STATE_CLOSE, OnWindowClose)
		COMMAND_ID_HANDLER(ID_WINDOW_MESSAGES, OnCaptureMessages)
		COMMAND_ID_HANDLER(ID_WINDOW_PROPERTIES, OnWindowProperties)
		COMMAND_ID_HANDLER(ID_VIEW_REFRESH, OnRefreshAll)
		CHAIN_MSG_MAP(CTreeViewManager<CWindowsView>)
		CHAIN_MSG_MAP(CViewBase<CWindowsView>)
		if (m_WindowsView) {
			CHAIN_MSG_MAP_ALT_MEMBER(m_WindowsView, 1)
		}
	END_MSG_MAP()

	LRESULT OnTreeNodeRightClick(HTREEITEM hItem, CPoint const& pt);

private:
	void UpdateUI();
	void Refresh();
	void InitTree();
	void AddChildWindows(HTREEITEM hParent);
	CTreeItem AddNode(HWND hWnd, HTREEITEM hParent);
	BOOL AddChildNode(HWND hChild);
	void NodeSelected();
	CTreeItem AddMessageOnlyWindows();

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNodeExpanding(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnNodeDeleted(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnNodeSelected(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnRefreshAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowHide(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowMinimize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowMaximize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowRestore(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnToggleHiddenWindows(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnToggleEmptyTitleWindows(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnToggleChildWindows(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowFlash(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowBringToFront(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowProperties(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowHighlight(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCaptureMessages(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	enum { MessageOnlyWindowsNode = 1 };

	CSplitterWindow m_Splitter;
	CWindowsListView m_WindowsView;
	//CTreeViewFrame m_TreeFrame;
	CTreeViewCtrlEx m_Tree;
	CTreeItem m_hCurrentNode;
	CTreeItem m_DesktopNode;
	CTreeItem m_Selected;
	CTreeItem m_MsgOnlyNode;
	CWindow m_SelectedHwnd;
	std::unordered_map<HWND, HTREEITEM> m_WindowMap;
	DWORD m_TotalWindows, m_TotalVisibleWindows, m_TopLevelWindows;

	bool m_ShowHiddenWindows : 1 { false };
	bool m_ShowNoTitleWindows : 1 { true };
	bool m_ShowChildWindows : 1 { true };
	bool m_Deleting{ false };
};
