#include "stdafx.h"
#include "DirectoryListCtrl.h"
#include "FDB_Collection.h"

using namespace std;

DirectoryListCtrl::DirectoryListCtrl(wxWindow *parent, wxWindowID id, 
					const wxPoint &pos, const wxSize &size, long style, const wxValidator &validator, const wxString &name) : 
		wxTreeCtrl(parent, id, pos, size, style, validator, name)
{
	p_fdb_pack = NULL;
	Connect( wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler( DirectoryListCtrl::OnTreeItemExpanding ), NULL, this );
}

DirectoryListCtrl::~DirectoryListCtrl()
{
	p_fdb_pack = NULL;
	Disconnect( wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler( DirectoryListCtrl::OnTreeItemExpanding ), NULL, this );
}

void DirectoryListCtrl::SetDataSource(FDB_Collection* _fdb_pack)
{
	p_fdb_pack = _fdb_pack;
}

void DirectoryListCtrl::Reset()
{
	DeleteAllItems();
	AddRoot(wxT(""));

	FillDirTree(GetRootItem());
}

void DirectoryListCtrl::OnTreeItemExpanding( wxTreeEvent& event )
{
	wxASSERT(p_fdb_pack);

	wxTreeItemIdValue v;
	wxTreeItemId n = GetFirstChild(event.GetItem(),v);
	if (n.IsOk() && GetItemText(n)==wxEmptyString)
	{
		FillDirTree(event.GetItem());
	}
}

void DirectoryListCtrl::FillDirTree(const wxTreeItemId& node)
{
	wxASSERT(p_fdb_pack);

	wxString cur_dir = GetFullName(node);

	vector<string> names;
	vector<bool> has_childs;

	p_fdb_pack->GetSubDirectories(cur_dir.mb_str(wxConvUTF8), names, has_childs);

	// clear all childs
	wxTreeItemId n;
	wxTreeItemIdValue v;

	while ( (n=GetFirstChild(node,v)).IsOk())
		Delete(n);

	// fill
	for (size_t i=0;i<names.size();++i)
	{
		wxTreeItemId n = InsertItem(node,0, wxString(names[i].c_str(), wxConvUTF8));
		if (has_childs[i])
			InsertItem(n,0,wxEmptyString);
	}

	SortChildren(node);
}

wxString DirectoryListCtrl::GetFullName(wxTreeItemId node)
{
wxString dir(wxT(""));
	while (node.IsOk() && node != GetRootItem())
	{
dir = GetItemText(node)+wxT("\\")+dir;
		node = GetItemParent(node);
	}
	return dir;
}

wxString DirectoryListCtrl::GetCurrentDir()
{
	return GetFullName(GetSelection());
}


