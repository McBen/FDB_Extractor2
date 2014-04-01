#include <wx/treectrl.h>

class FDB_Collection;

class DirectoryListCtrl : public wxTreeCtrl
{
	public:
		DirectoryListCtrl(wxWindow *parent, wxWindowID id=wxID_ANY,
					const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxTR_DEFAULT_STYLE,
					const wxValidator &validator=wxDefaultValidator, const wxString &name=wxTreeCtrlNameStr);
		~DirectoryListCtrl();

		void SetDataSource(FDB_Collection* fdb_pack);

		void Reset();

		wxString GetFullName(wxTreeItemId node);
		wxString GetCurrentDir();

	protected:
		void OnTreeItemExpanding( wxTreeEvent& event );

	private:
		void FillDirTree(const wxTreeItemId& node);

	private:

		FDB_Collection*	p_fdb_pack;
};

