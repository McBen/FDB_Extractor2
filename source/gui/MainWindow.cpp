#include "stdafx.h"
#include "MainWindow.h"
#include "DirectoryListCtrl.h"
#include "FileListCtrl.h"
#include "resource.h"
#include "../../resources/images/wxresource.h"
#include "../VersionNo.h"
#include <wx/clipbrd.h>

using namespace std;

enum {
	FDBex_ExtractFocusFile = 8000,
	FDBex_ExtractFiles,
	FDBex_ExtractFolder,
	FDBex_CopyPath,
};


BEGIN_EVENT_TABLE(MainWindow, BASE_DLG::MainWindow)
  EVT_MENU    (FDBex_ExtractFocusFile,   MainWindow::OnExtractFocusFile)
  EVT_MENU    (FDBex_ExtractFiles,   MainWindow::OnExtractFiles)
  EVT_MENU    (FDBex_ExtractFolder, MainWindow::OnExtractFolder)
  EVT_MENU    (FDBex_CopyPath, MainWindow::OnCopyPath)
END_EVENT_TABLE()


MainWindow::MainWindow() : BASE_DLG::MainWindow(NULL)
{
	// not supported by wxFormBuilder so by hand:
	m_toolBar1->SetToolNormalBitmap(wxCMD_LOAD_FDB, wxResourceGetBitmap(wxRES_folder));
	m_toolBar1->SetToolNormalBitmap(FDBex_ExtractFiles, wxResourceGetBitmap(wxRES_cog_go));
	m_toolBar1->SetToolNormalBitmap(FDBex_ExtractFolder, wxResourceGetBitmap(wxRES_ex_folder));


	#if wxCHECK_VERSION(2, 9, 0)
		m_toolBar1->AddStretchableSpace();
	#else
		m_toolBar1->AddSeparator();
	#endif

	m_toolBar1->AddTool( 7000, _("Wiki"), wxArtProvider::GetBitmap( wxART_QUESTION, wxART_BUTTON ), wxNullBitmap, wxITEM_NORMAL, _("Open FDBex2 Webpage"), wxEmptyString, NULL ); 
	m_toolBar1->Realize(); 


#ifdef WIN32
	SetIcon(wxICON(IDI_ICON1)); // TODO: BUG: Idk why this isn't working
#endif
	
	directory_ctrl->SetDataSource(&fdb_pack);
	
	if (fdb_pack.OpenDefault())
	{
	    
	    #if wxCHECK_VERSION(2, 9, 0)
		wxString path=GetROMInstallDir();
	    #else
		wxString path = wxString(GetROMInstallDir().c_str(), wxConvUTF8);
	    #endif
	    ShowBasePath(path+wxT("fdb"));
	}

	RebuildView();

	file_ctrl->Connect(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(MainWindow::file_ctrlOnContextMenu), NULL, this);
	this->Connect( 7000, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainWindow::OnOpenWebPage ) );
};

MainWindow::~MainWindow()
{
	file_ctrl->Disconnect(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(MainWindow::file_ctrlOnContextMenu), NULL, this);
	DeleteTempFiles();
}

void MainWindow::ShowBasePath(const wxString& path)
{
	this->SetTitle(_("FDB Extractor " STRFILEVER " - ")+ path);
}

void MainWindow::RebuildView()
{
	file_ctrl->DeleteAllItems();

	directory_ctrl->Reset();
}

void MainWindow::directory_ctrlOnTreeSelChanged( wxTreeEvent& event )
{
	wxBusyCursor wait;

	const wxChar* comp_modes[]=
	{
<<<<<<< HEAD
		_("none"),_("RLE"),_("LZO"),_("ZIP"),_("Redux"),_("!UNK!")
=======
		_("none"),_("RLE"),_("LZO"),_("ZIP"),_("Nvidia"),_("!UNK!")
>>>>>>> more linux stuff
	};

	wxString cur_dir = directory_ctrl->GetFullName(event.GetItem());

	vector<FDBPackage::file_info> infos;
	fdb_pack.GetFileInfos(cur_dir.mb_str(), infos);

	file_ctrl->Freeze();

	file_ctrl->DeleteAllItems();
	for (vector<FDBPackage::file_info>::iterator i=infos.begin();i!=infos.end();++i)
	{
		wxFileName fname( wxString(i->name, wxConvUTF8) );

		int icon = (int) GetFileTypeIcon(*i);

		long item = file_ctrl->InsertItem( file_ctrl->GetItemCount(), fname.GetFullName(), icon);
		file_ctrl->SetItem(item,1, FormatNumber(i->size_uncomp));
		file_ctrl->SetItem(item,2, FormatNumber(i->size_comp));

		int comp = i->compression;
		if (comp<0||comp>4) comp=5;
		file_ctrl->SetItem(item,3, comp_modes[comp]);
		
	  #ifdef WIN32
		// TODO: linux alternative required
		SYSTEMTIME SystemTime;
		FileTimeToSystemTime((FILETIME*)&i->mtime, &SystemTime);

		wxDateTime datetime(SystemTime);
		file_ctrl->SetItem(item,4, datetime.FormatDate());
	  #endif
	}

	file_ctrl->Thaw();
}

wxString MainWindow::FormatNumber(size_t num)
{
	wxString str =  wxString::Format(wxT("%i"),num);
	int l=str.Len()-3;
	while (l>0) { str.insert(l,wxT('.')); l-=3; };

	return str;
}

MainWindow::FT_ICONS MainWindow::GetFileTypeIcon(const FDBPackage::file_info& info)
{
	if (info.ftype == 2) return FT_PICTURE;

	wxFileName fname(wxString(info.name, wxConvUTF8) );
	wxString ext = fname.GetExt().Lower();
	if (ext==wxT("lua") || ext==wxT("ini")) return FT_SCRIPT;
	if (ext==wxT("mp3") || ext==wxT("wav")) return FT_MUSIC;
	if (ext==wxT("txt") || ext==wxT("toc")) return FT_TEXT;
	if (ext==wxT("db") ) return FT_TABLE;
	if (ext==wxT("ros")) return FT_MESH;
	if (ext==wxT("xml")) return FT_XML;
	//if (ext=="wav")  return FT_NODES;

	return FT_EMPTY;
}

void MainWindow::OnLoadFDB( wxCommandEvent& event )
{
	if (last_open_path.IsEmpty()) last_open_path = wxString(GetROMInstallDir().c_str(), wxConvUTF8);

	wxFileDialog dlg(this, _("Open FDB Files"), last_open_path, wxEmptyString,
			       _("FDB files (*.fdb)|*.fdb"), wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_MULTIPLE);
	
	if (dlg.ShowModal() == wxID_CANCEL) return;
	last_open_path = dlg.GetDirectory();
	ShowBasePath(last_open_path);

	fdb_pack.Close();

	wxArrayString filenames;
	dlg.GetPaths(filenames);

	for (size_t t=0;t<filenames.size();++t)
		fdb_pack.Open(filenames[t].mb_str());


	RebuildView();
}

void MainWindow::directory_ctrlOnTreeItemMenu( wxTreeEvent& event )
{
	wxPoint pt = event.GetPoint();

	wxMenu menu(wxEmptyString);
    menu.Append(FDBex_ExtractFolder, _("Extract folder to..."));

    PopupMenu(&menu, pt);
}

void MainWindow::file_ctrlOnContextMenu( wxContextMenuEvent& event )
{
	wxPoint point = event.GetPosition();

	if ( (point.x == -1) && (point.y == -1) )
    {
        wxSize size = GetSize();
        point.x = size.x / 2;
        point.y = size.y / 2;
    }
    else
    {
        point = ScreenToClient(point);
    }

	wxMenu menu(wxEmptyString);

	long item = file_ctrl->GetNextItem(-1,wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
	if (item!=-1) menu.Append(FDBex_ExtractFocusFile, wxString::Format(_("Extract '%s' to..."), file_ctrl->GetItemText(item)));
	if (file_ctrl->GetSelectedItemCount()>1)  menu.Append(FDBex_ExtractFiles, _("Extract files to..."));
    menu.Append(FDBex_ExtractFolder, _("Extract folder to..."));

	menu.AppendSeparator();
	menu.Append(FDBex_CopyPath, _("Copy pathname"));

    PopupMenu(&menu, point);
}

void MainWindow::file_ctrlOnLeftDClick( wxMouseEvent& event )
{
	int flags = wxLIST_HITTEST_ONITEM;
	long idx = file_ctrl->HitTest(event.GetPosition(), flags);
	if (idx==wxNOT_FOUND) return;
	
	wxString src = directory_ctrl->GetCurrentDir() + file_ctrl->GetItemText(idx);
	wxString dest =  wxFileName::GetTempDir() + wxT("\\")+ file_ctrl->GetItemText(idx);

	std::string res1_name = fdb_pack.ExtractFile(src.mb_str(),dest.mb_str());
	wxString res_name(res1_name.c_str(),wxConvUTF8);
	wxASSERT(!res_name.IsEmpty());
	if (res_name.IsEmpty()) return;

	files_to_delete.Add(res_name);

	#if wxCHECK_VERSION(2, 9, 0)
	    wxLaunchDefaultApplication(res_name);
	#else
	    #ifdef WIN32
	      wxShell(res_name);
	    #else
	      wxExecute(res_name);
	    #endif
	#endif	
}

void MainWindow::DeleteTempFiles()
{
	for (size_t t=0;t<files_to_delete.size();++t)
	{
		if (wxFileExists(files_to_delete[t]))
			wxRemoveFile(files_to_delete[t]);
	}

	files_to_delete.Clear();
}

void MainWindow::OnExtractFocusFile(wxCommandEvent& WXUNUSED(event))
{
	long item = file_ctrl->GetNextItem(-1,wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
	if (item==-1) return;

	wxString curname = directory_ctrl->GetCurrentDir()+file_ctrl->GetItemText(item);

	wxFileDialog dlg(this, _("Save file"),last_export_path,file_ctrl->GetItemText(item),wxEmptyString, wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	if (dlg.ShowModal()==wxID_CANCEL) return;
	last_export_path = dlg.GetDirectory();

	fdb_pack.ExtractFile(curname.mb_str(), dlg.GetPath().mb_str());
}

void MainWindow::OnCopyPath(wxCommandEvent& WXUNUSED(event))
{
	long item = file_ctrl->GetNextItem(-1,wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
	if (item==-1) return;

	wxString curname = directory_ctrl->GetCurrentDir()+file_ctrl->GetItemText(item);

	if (wxTheClipboard->Open())
	{
		wxTheClipboard->SetData( new wxTextDataObject(curname) );
		wxTheClipboard->Close();
	}
}


void MainWindow::OnExtractFiles(wxCommandEvent& WXUNUSED(event))
{
	wxArrayString cur_files;
	long item = -1;
    for ( ;; )
    {
        item = file_ctrl->GetNextItem(item,wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if ( item == -1 )
            break;

		cur_files.Add(file_ctrl->GetItemText(item));
    }

	wxDirDialog dlg(this, _("Export to..."), last_export_path, wxDD_DEFAULT_STYLE);

	if (dlg.ShowModal() == wxID_CANCEL) return;
	last_export_path = dlg.GetPath();

	fdb_pack.ExtractMultipleFiles(directory_ctrl->GetCurrentDir().mb_str(),dlg.GetPath().mb_str(), cur_files);

}

void MainWindow::OnExtractFolder(wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog dlg(this, _("Export to..."), last_export_path, wxDD_DEFAULT_STYLE);

	if (dlg.ShowModal() == wxID_CANCEL) return;
	last_export_path = dlg.GetPath();

	fdb_pack.ExtractMultipleFiles(directory_ctrl->GetCurrentDir().mb_str(),dlg.GetPath().mb_str(),wxArrayString());
}

void MainWindow::m_extract_folderOnUpdateUI( wxUpdateUIEvent& event )
{
	event.Enable( directory_ctrl->GetSelection().IsOk() );
}

void MainWindow::m_extract_fileOnUpdateUI( wxUpdateUIEvent& event )
{
	event.Enable( file_ctrl->GetSelectedItemCount()>0 );
}

void MainWindow::OnOpenWebPage(wxCommandEvent& WXUNUSED(event))
{
	wxLaunchDefaultBrowser(wxT("http://github.com/McBen/FDB_Extractor2/wiki"));
};
