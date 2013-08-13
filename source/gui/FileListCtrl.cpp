#include "stdafx.h"
#include "FileListCtrl.h"
#include "../../resources/images/wxresource.h"

using namespace std;

FileListCtrl::FileListCtrl(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, 
					long style, const wxValidator &validator, const wxString &name) : 
		wxListCtrl(parent, id, pos, size, style, validator, name)
{
	InsertColumn(0,wxT("Name"),wxLIST_FORMAT_LEFT,220);
	InsertColumn(1,wxT("Size"), wxLIST_FORMAT_RIGHT,60);
	InsertColumn(2,wxT("Packed"), wxLIST_FORMAT_RIGHT,60);
	InsertColumn(3,wxT("Mode"), wxLIST_FORMAT_CENTER,40);
	InsertColumn(4,wxT("Time"),70);
	InsertColumn(5,wxT("Comment"));

	wxBitmap images = wxResourceGetBitmap(wxRES_filetype_icons);
	file_icons.Create(16,16);
	file_icons.Add(images);
	SetImageList(&file_icons,wxIMAGE_LIST_NORMAL);
	SetImageList(&file_icons,wxIMAGE_LIST_SMALL);
}


FileListCtrl::~FileListCtrl()
{
}

