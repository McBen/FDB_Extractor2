///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  8 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "DirectoryListCtrl.h"
#include "FileListCtrl.h"

#include "gui_def.h"

///////////////////////////////////////////////////////////////////////////
using namespace BASE_DLG;

MainWindow::MainWindow( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_toolBar1 = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL );
	m_toolBar1->SetToolBitmapSize( wxSize( 16,16 ) );
	m_toolBar1->SetToolSeparation( 10 );
	m_toolBar1->AddTool( wxCMD_LOAD_FDB, _("Load FDBs"), wxArtProvider::GetBitmap( wxART_FOLDER, wxART_TOOLBAR ), wxNullBitmap, wxITEM_NORMAL, _("Open FDB Files"), wxEmptyString, NULL ); 
	
	m_toolBar1->AddSeparator(); 
	
	m_toolBar1->AddTool( FDBex_ExtractFolder, _("Extract Folder"), wxArtProvider::GetBitmap( wxART_GO_FORWARD, wxART_TOOLBAR ), wxNullBitmap, wxITEM_NORMAL, _("Extract current Folder"), wxEmptyString, NULL ); 
	
	m_toolBar1->AddTool( FDBex_ExtractFiles, _("Extract Files"), wxArtProvider::GetBitmap( wxART_EXECUTABLE_FILE, wxART_TOOLBAR ), wxNullBitmap, wxITEM_NORMAL, _("Extract selected File(s)"), wxEmptyString, NULL ); 
	
	m_toolBar1->Realize(); 
	
	bSizer1->Add( m_toolBar1, 0, wxEXPAND, 5 );
	
	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_3DBORDER|wxSP_3DSASH|wxSP_LIVE_UPDATE );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( MainWindow::m_splitter1OnIdle ), NULL, this );
	m_splitter1->SetMinimumPaneSize( 50 );
	
	m_panel5 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	directory_ctrl = new DirectoryListCtrl( m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT );
	bSizer4->Add( directory_ctrl, 1, wxALL|wxEXPAND, 5 );
	
	
	m_panel5->SetSizer( bSizer4 );
	m_panel5->Layout();
	bSizer4->Fit( m_panel5 );
	m_panel6 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	file_ctrl = new FileListCtrl( m_panel6, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	bSizer5->Add( file_ctrl, 1, wxALL|wxEXPAND, 5 );
	
	
	m_panel6->SetSizer( bSizer5 );
	m_panel6->Layout();
	bSizer5->Fit( m_panel6 );
	m_splitter1->SplitVertically( m_panel5, m_panel6, 220 );
	bSizer1->Add( m_splitter1, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxCMD_LOAD_FDB, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainWindow::OnLoadFDB ) );
	this->Connect( FDBex_ExtractFolder, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( MainWindow::m_extract_folderOnUpdateUI ) );
	this->Connect( FDBex_ExtractFiles, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( MainWindow::m_extract_fileOnUpdateUI ) );
	directory_ctrl->Connect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( MainWindow::directory_ctrlOnTreeItemMenu ), NULL, this );
	directory_ctrl->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( MainWindow::directory_ctrlOnTreeSelChanged ), NULL, this );
	file_ctrl->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( MainWindow::file_ctrlOnLeftDClick ), NULL, this );
}

MainWindow::~MainWindow()
{
	// Disconnect Events
	this->Disconnect( wxCMD_LOAD_FDB, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainWindow::OnLoadFDB ) );
	this->Disconnect( FDBex_ExtractFolder, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( MainWindow::m_extract_folderOnUpdateUI ) );
	this->Disconnect( FDBex_ExtractFiles, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( MainWindow::m_extract_fileOnUpdateUI ) );
	directory_ctrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( MainWindow::directory_ctrlOnTreeItemMenu ), NULL, this );
	directory_ctrl->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( MainWindow::directory_ctrlOnTreeSelChanged ), NULL, this );
	file_ctrl->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( MainWindow::file_ctrlOnLeftDClick ), NULL, this );
	
}

Export_DLG::Export_DLG( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_textCtrl1 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTRE|wxTE_READONLY|wxNO_BORDER );
	m_textCtrl1->SetMaxLength( 0 ); 
	m_textCtrl1->Enable( false );
	
	bSizer4->Add( m_textCtrl1, 0, wxALIGN_CENTER|wxALL|wxEXPAND, 5 );
	
	m_gauge1 = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxSize( -1,20 ), wxGA_HORIZONTAL );
	bSizer4->Add( m_gauge1, 0, wxALL|wxEXPAND, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	
	bSizer4->Add( m_sdbSizer1, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer4 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_sdbSizer1Cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Export_DLG::m_sdbSizer1OnCancelButtonClick ), NULL, this );
}

Export_DLG::~Export_DLG()
{
	// Disconnect Events
	m_sdbSizer1Cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Export_DLG::m_sdbSizer1OnCancelButtonClick ), NULL, this );
	
}
