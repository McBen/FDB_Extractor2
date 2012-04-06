#include <wx/listctrl.h>
#include <wx/imaglist.h>

class FDB_Collection;

class FileListCtrl : public wxListCtrl
{
	public:
		FileListCtrl(wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
					long style=wxLC_ICON, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxListCtrlNameStr);
		~FileListCtrl();

	private:
		wxImageList file_icons;


};

