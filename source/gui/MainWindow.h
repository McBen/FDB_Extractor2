#include "gui_def.h"
#include "FDB_Collection.h"


class MainWindow : public BASE_DLG::MainWindow
{
	private:
		enum FT_ICONS {
				FT_SCRIPT = 0,
				FT_PICTURE,
				FT_TABLE,
				FT_MUSIC,
				FT_MESH,
				FT_NODES = 5,
				FT_TEXT,
				FT_XML,
				FT_EMPTY=8,
		};

	public:
		MainWindow();
		~MainWindow();

		void directory_ctrlOnTreeSelChanged( wxTreeEvent& event );
		void directory_ctrlOnTreeItemMenu( wxTreeEvent& event );
		void OnLoadFDB( wxCommandEvent& event );
		void file_ctrlOnLeftDClick( wxMouseEvent& event );
		void file_ctrlOnContextMenu(wxContextMenuEvent& event);

		void OnExtractFocusFile(wxCommandEvent& WXUNUSED(event));
		void OnExtractFiles(wxCommandEvent& WXUNUSED(event));
		void OnExtractFolder(wxCommandEvent& WXUNUSED(event));
		void OnOpenWebPage(wxCommandEvent& WXUNUSED(event));
		
		void m_extract_folderOnUpdateUI( wxUpdateUIEvent& event );
		void m_extract_fileOnUpdateUI( wxUpdateUIEvent& event );

	private:
		void LoadFDBs(bool clear);
		void RebuildView();

		wxString FormatNumber(size_t num);
		FT_ICONS GetFileTypeIcon(const FDBPackage::file_info&);

		void	DeleteTempFiles();

	private:
		FDB_Collection	fdb_pack;

		wxArrayString files_to_delete;
		wxString last_export_path;

		DECLARE_EVENT_TABLE()
};

