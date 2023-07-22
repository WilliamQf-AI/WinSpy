#pragma once

#include "ViewBase.h"
#include "WindowsListView.h"
#include "TreeViewManager.h"
#include "ProcessHelper.h"

class CProcessesView : 
	public CViewBase<CProcessesView>,
	public CTreeViewManager<CProcessesView> {
public:
	CProcessesView(IMainFrame* frame) : CViewBase(frame), m_WindowsView(frame) {}

	void OnActivate(bool active);

protected:
	enum { IDC_TREE = 123 };

	BEGIN_MSG_MAP(CProcessesView)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		NOTIFY_CODE_HANDLER(TVN_ITEMEXPANDING, OnNodeExpanding)
		//NOTIFY_CODE_HANDLER(TVN_DELETEITEM, OnNodeDeleted)
		COMMAND_ID_HANDLER(ID_VIEW_HIDDENWINDOWS, OnToggleHiddenWindows)
		COMMAND_ID_HANDLER(ID_VIEW_EMPTYTITLEWINDOWS, OnToggleEmptyTitleWindows)
		NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnNodeSelected)
		COMMAND_ID_HANDLER(ID_WINDOW_SHOW, OnWindowShow)
		COMMAND_ID_HANDLER(ID_WINDOW_HIDE, OnWindowHide)
		COMMAND_ID_HANDLER(ID_WINDOW_BRINGTOFRONT, OnWindowBringToFront)
		COMMAND_ID_HANDLER(ID_WINDOW_MINIMIZE, OnWindowMinimize)
		COMMAND_ID_HANDLER(ID_WINDOW_MAXIMIZE, OnWindowMaximize)
		COMMAND_ID_HANDLER(ID_STATE_FLASH, OnWindowFlash)
		COMMAND_ID_HANDLER(ID_WINDOW_RESTORE, OnWindowRestore)
		COMMAND_ID_HANDLER(ID_WINDOW_PROPERTIES, OnWindowProperties)
		COMMAND_ID_HANDLER(ID_PROCESS_PROPERTIES, OnProcessProperties)
		CHAIN_MSG_MAP(CTreeViewManager<CProcessesView>)
		CHAIN_MSG_MAP(CViewBase<CProcessesView>)
		if (m_WindowsView) {
			CHAIN_MSG_MAP_ALT_MEMBER(m_WindowsView, 1)
		}
	END_MSG_MAP()

	LRESULT OnTreeNodeRightClick(HTREEITEM hItem, CPoint const& pt);
	LRESULT OnTreeNodeDoubleClick(HTREEITEM hItem, CPoint const& pt);

private:
	enum class ItemType {
		Process,
		Thread,
		Window
	};

	void InitTree();
	void UpdateUI();
	CTreeItem AddNode(HWND hWnd, HTREEITEM hParent);
	void AddChildWindows(HTREEITEM hParent);
	BOOL AddChildNode(HWND hChild);
	void ChangeSelection(HTREEITEM hItem);

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNodeExpanding(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnNodeDeleted(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnNodeSelected(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnWindowShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowHide(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowMinimize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowMaximize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowRestore(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnToggleHiddenWindows(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnToggleEmptyTitleWindows(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnToggleChildWindows(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowFlash(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowBringToFront(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowProperties(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnProcessProperties(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	CWindowsListView m_WindowsView;
	CSplitterWindow m_Splitter;
	CTreeViewCtrlEx m_Tree;

	std::unordered_map<HTREEITEM, ProcessInfo> m_Processes;
	std::unordered_map<HTREEITEM, DWORD> m_Threads;
	std::unordered_map<HWND, HTREEITEM> m_WindowMap;
	CTreeItem m_hCurrentNode;
	CTreeItem m_Selected;
	CWindow m_SelectedHwnd;
	int m_TotalWindows, m_TotalVisibleWindows, m_TopLevelWindows;
	bool m_ShowHiddenWindows : 1 { true };
	bool m_ShowNoTitleWindows : 1 { true };
	bool m_ShowChildWindows : 1 { true };
};
