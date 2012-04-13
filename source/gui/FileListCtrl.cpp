#include "stdafx.h"
#include "FileListCtrl.h"
#include "../../resources/images/wxresource.h"

using namespace std;

FileListCtrl::FileListCtrl(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, 
					long style, const wxValidator &validator, const wxString &name) : 
		wxListCtrl(parent, id, pos, size, style, validator, name)
{
	AppendColumn("Name",wxLIST_FORMAT_LEFT,220);
	AppendColumn("Size", wxLIST_FORMAT_RIGHT,60);
	AppendColumn("Packed", wxLIST_FORMAT_RIGHT,60);
	AppendColumn("Mode", wxLIST_FORMAT_CENTER,40);
	AppendColumn("Time",70);
	AppendColumn("Comment");

	wxBitmap images = wxResourceGetBitmap(wxRES_filetype_icons);
	file_icons.Create(16,16);
	file_icons.Add(images);
	SetImageList(&file_icons,wxIMAGE_LIST_NORMAL);
	SetImageList(&file_icons,wxIMAGE_LIST_SMALL);
}


FileListCtrl::~FileListCtrl()
{
}

