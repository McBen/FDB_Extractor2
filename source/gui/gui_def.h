///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __GUI_DEF_H__
#define __GUI_DEF_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
class DirectoryListCtrl;
class FileListCtrl;

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/toolbar.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/treectrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/splitter.h>
#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/gauge.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

namespace BASE_DLG
{
	///////////////////////////////////////////////////////////////////////////////
	/// Class MainWindow
	///////////////////////////////////////////////////////////////////////////////
	class MainWindow : public wxFrame 
	{
		private:
		
		protected:
			enum
			{
				wxCMD_LOAD_FDB = 6000,
				wxCMD_ADD_FDB,
				FDBex_ExtractFolder,
				FDBex_ExtractFiles,
			};
			
			wxToolBar* m_toolBar1;
			wxSplitterWindow* m_splitter1;
			wxPanel* m_panel5;
			DirectoryListCtrl* directory_ctrl;
			wxPanel* m_panel6;
			FileListCtrl* file_ctrl;
			
			// Virtual event handlers, overide them in your derived class
			virtual void OnLoadFDB( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnAddFDB( wxCommandEvent& event ) { event.Skip(); }
			virtual void m_extract_folderOnUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
			virtual void m_extract_fileOnUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
			virtual void directory_ctrlOnTreeItemMenu( wxTreeEvent& event ) { event.Skip(); }
			virtual void directory_ctrlOnTreeSelChanged( wxTreeEvent& event ) { event.Skip(); }
			virtual void file_ctrlOnLeftDClick( wxMouseEvent& event ) { event.Skip(); }
			
		
		public:
			
			MainWindow( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("FDB Extractor2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 864,601 ), long style = wxDEFAULT_FRAME_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER|wxSYSTEM_MENU|wxTAB_TRAVERSAL );
			
			~MainWindow();
			
			void m_splitter1OnIdle( wxIdleEvent& )
			{
				m_splitter1->SetSashPosition( 220 );
				m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( MainWindow::m_splitter1OnIdle ), NULL, this );
			}
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class Export_DLG
	///////////////////////////////////////////////////////////////////////////////
	class Export_DLG : public wxDialog 
	{
		private:
		
		protected:
			wxTextCtrl* m_textCtrl1;
			wxGauge* m_gauge1;
			wxStdDialogButtonSizer* m_sdbSizer1;
			wxButton* m_sdbSizer1Cancel;
			
			// Virtual event handlers, overide them in your derived class
			virtual void m_sdbSizer1OnCancelButtonClick( wxCommandEvent& event ) { event.Skip(); }
			
		
		public:
			
			Export_DLG( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Extracting..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 406,164 ), long style = wxDEFAULT_DIALOG_STYLE ); 
			~Export_DLG();
		
	};
	
} // namespace BASE_DLG

#endif //__GUI_DEF_H__
