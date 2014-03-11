#include "Export_DLG.h"
#include "main.h"

Export_DLG::Export_DLG( wxWindow* parent ) : BASE_DLG::Export_DLG( parent )
{
	pos=0;
	IsCanceled = false;
	next_update_tick = 0;
}

void Export_DLG::m_sdbSizer1OnCancelButtonClick( wxCommandEvent& event )
{
	IsCanceled = true;
}

void Export_DLG::SetMax(int max)
{
	pos=0;
	m_gauge1->SetRange(max);
}


void Export_DLG::Step(const wxString& fname)
{
	++pos;

	if (next_update_tick < wxGetLocalTimeMillis().GetLo())
	{
		m_gauge1->SetValue(pos);
		m_textCtrl1->SetLabel(fname);
		next_update_tick = wxGetLocalTimeMillis().GetLo()+100;

		wxGetApp().Yield();
	}
}

