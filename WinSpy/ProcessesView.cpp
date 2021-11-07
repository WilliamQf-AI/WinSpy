#include "pch.h"
#include "resource.h"
#include "ProcessesView.h"
#include "ImageIconCache.h"

LRESULT CProcessesView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	m_Splitter.SetSplitterExtendedStyle(SPLIT_FLATBAR | SPLIT_PROPORTIONAL);
	m_hWndClient = m_Splitter.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_Tree.Create(m_Splitter, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE, IDC_TREE);
	m_WindowsView.Create(m_Splitter, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	m_Tree.SetExtendedStyle(TVS_EX_DOUBLEBUFFER, TVS_EX_DOUBLEBUFFER);

	m_Tree.SetImageList(WindowHelper::GetImageList(), TVSIL_NORMAL);

	m_Splitter.SetSplitterPanes(m_Tree, m_WindowsView);
	UpdateLayout();
	m_Splitter.SetSplitterPosPct(35);

	InitTree();

	return 0;
}

void CProcessesView::OnFinalMessage(HWND) {
	delete this;
}

LRESULT CProcessesView::OnTimer(UINT /*uMsg*/, WPARAM id, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	if (id == 3) {
		KillTimer(3);
		m_Selected = m_Tree.GetSelectedItem();
		auto data = static_cast<ItemType>(m_Selected.GetData());
		switch (data) {
			case ItemType::Process:
				m_WindowsView.UpdateListByProcess(m_Processes[m_Selected]);
				break;

			case ItemType::Thread:
				m_WindowsView.UpdateListByThread(m_Threads[m_Selected]);
				break;

			default:
				auto hWnd = (HWND)m_Selected.GetData();
				m_SelectedHwnd = hWnd;
				if (!::IsWindow(hWnd))	// window is probably destroyed
					m_Selected.Delete();
				else {
					m_WindowsView.UpdateList(hWnd);
				}
				break;
		}

	}
	return 0;
}

void CProcessesView::InitTree() {
	m_Tree.LockWindowUpdate();
	m_Tree.DeleteAllItems();
	m_Processes.clear();
	m_WindowMap.clear();
	m_Threads.clear();
	m_TotalWindows = m_TotalVisibleWindows = m_TopLevelWindows = 0;

	CString text;
	for (auto& pi : ProcessHelper::EnumProcessesAndThreads(EnumProcessesOptions::UIThreadsOnly | EnumProcessesOptions::SkipProcessesWithNoUI)) {
		auto icon = ImageIconCache::Get().GetIcon(pi.FullPath);
		text.Format(L"%s (%u)", (PCWSTR)pi.ProcessName, pi.ProcessId);
		auto node = m_Tree.InsertItem(text, icon, icon, TVI_ROOT, TVI_SORT);
		node.SetData((DWORD_PTR)ItemType::Process);
		m_Processes.insert({ node, pi });

		for (auto tid : pi.Threads) {
			text.Format(L"Thread %u (0x%X)", tid, tid);
			auto tnode = m_Tree.InsertItem(text, icon, icon, node, TVI_LAST);
			if(!WindowHelper::ThreadHasWindows(tid))
				tnode.SetState(TVIS_CUT, TVIS_CUT);
			tnode.SetData((DWORD_PTR)ItemType::Thread);
			m_Threads.insert({ tnode, tid });
			struct LocalData {
				CProcessesView* pThis;
				HTREEITEM hParent;
			};
			LocalData data = { this, tnode };
			::EnumThreadWindows(tid, [](auto hWnd, auto param) {
				auto data = (LocalData*)param;
				data->pThis->AddNode(hWnd, data->hParent);
				return TRUE;
				}, reinterpret_cast<LPARAM>(&data));
		}
	}
	m_Tree.LockWindowUpdate(FALSE);
}

CTreeItem CProcessesView::AddNode(HWND hWnd, HTREEITEM hParent) {
	CString text, name;
	CWindow win(hWnd);
	m_TotalWindows++;
	if (win.IsWindowVisible())
		m_TotalVisibleWindows++;

	m_TopLevelWindows++;

	if (!m_ShowHiddenWindows && !win.IsWindowVisible())
		return nullptr;
	win.GetWindowText(name);
	if (!m_ShowNoTitleWindows && name.IsEmpty())
		return nullptr;

	if (name.GetLength() > 64)
		name = name.Left(64) + L"...";
	if (!name.IsEmpty())
		name = L"[" + name + L"]";
	WCHAR className[64] = { 0 };
	::GetClassName(hWnd, className, _countof(className));
	text.Format(L"0x%zX (%s) %s", (DWORD_PTR)hWnd, className, (PCWSTR)name);

	HICON hIcon{ nullptr };
	int image = 0;
	if ((win.GetStyle() & WS_CHILD) == 0) {
		auto& icons = WindowHelper::GetIconMap();
		if (auto it = icons.find(hWnd); it == icons.end()) {
			hIcon = WindowHelper::GetWindowOrProcessIcon(hWnd);
			if (hIcon) {
				icons.insert({ hWnd, image = WindowHelper::GetImageList().AddIcon(hIcon) });
			}
		}
		else {
			image = it->second;
		}
	}

	auto node = m_Tree.InsertItem(text, image, image, hParent, TVI_LAST);
	node.SetData((DWORD_PTR)hWnd);
	m_WindowMap.insert({ hWnd, node });

	if (!win.IsWindowVisible())
		node.SetState(TVIS_CUT, TVIS_CUT);

	if (win.GetWindow(GW_CHILD)) {
		// add a "plus" button
		node.AddTail(L"*", 0);
	}
	return node;
}

LRESULT CProcessesView::OnNodeExpanding(int, LPNMHDR hdr, BOOL&) {
	auto tv = (NMTREEVIEW*)hdr;
	if (tv->action == TVE_EXPAND) {
		auto hItem = tv->itemNew.hItem;

		auto child = m_Tree.GetChildItem(hItem);
		if (child.GetData() == 0) {
			child.Delete();
			AddChildWindows(hItem);
		}
	}
	return 0;
}

void CProcessesView::AddChildWindows(HTREEITEM hParent) {
	auto hWnd = (HWND)m_Tree.GetItemData(hParent);
	ATLASSERT(hWnd);

	m_hCurrentNode = hParent;
	::EnumChildWindows(hWnd, [](auto hChild, auto p) -> BOOL {
		auto pThis = (CProcessesView*)p;
		return pThis->AddChildNode(hChild);
		}, reinterpret_cast<LPARAM>(this));
}

BOOL CProcessesView::AddChildNode(HWND hChild) {
	if (::GetAncestor(hChild, GA_PARENT) == (HWND)m_Tree.GetItemData(m_hCurrentNode)) {
		AddNode(hChild, m_hCurrentNode);
	}
	return TRUE;
}

LRESULT CProcessesView::OnTreeNodeRightClick(int, LPNMHDR, BOOL&) {
	ATLASSERT(m_Selected);
	if (!m_Selected)
		return 0;

	CMenu menu;
	menu.LoadMenu(IDR_CONTEXT);
	CPoint pt;
	::GetCursorPos(&pt);

	return GetFrame()->ShowContextMenu(menu.GetSubMenu(0), pt);
}

LRESULT CProcessesView::OnWindowShow(WORD, WORD, HWND, BOOL&) {
	ATLASSERT(m_Selected);
	m_SelectedHwnd.ShowWindow(SW_SHOW);

	return 0;
}

LRESULT CProcessesView::OnWindowHide(WORD, WORD, HWND, BOOL&) {
	ATLASSERT(m_Selected);
	m_SelectedHwnd.ShowWindow(SW_HIDE);
	return 0;
}

LRESULT CProcessesView::OnWindowMinimize(WORD, WORD, HWND, BOOL&) {
	ATLASSERT(m_Selected);
	m_SelectedHwnd.ShowWindow(SW_MINIMIZE);
	return 0;
}

LRESULT CProcessesView::OnWindowMaximize(WORD, WORD, HWND, BOOL&) {
	ATLASSERT(m_Selected);
	m_SelectedHwnd.ShowWindow(SW_MAXIMIZE);

	return 0;
}

LRESULT CProcessesView::OnWindowRestore(WORD, WORD, HWND, BOOL&) {
	ATLASSERT(m_Selected);
	m_SelectedHwnd.ShowWindow(SW_RESTORE);
	return 0;
}

LRESULT CProcessesView::OnWindowFlash(WORD, WORD, HWND, BOOL&) {
	ATLASSERT(m_Selected);
	WindowHelper::Flash((HWND)m_Selected.GetData());

	return 0;
}

LRESULT CProcessesView::OnWindowBringToFront(WORD, WORD, HWND, BOOL&) {
	ATLASSERT(m_Selected);
	m_SelectedHwnd.BringWindowToTop();
	return 0;
}

LRESULT CProcessesView::OnNodeSelected(int, LPNMHDR, BOOL&) {
	// short delay before update in case the user moves quickly through the tree
	SetTimer(3, 250, nullptr);
	return 0;
}