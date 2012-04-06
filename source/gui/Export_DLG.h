#ifndef __FDB_Extractor2Export_DLG__
#define __FDB_Extractor2Export_DLG__

#include "gui_def.h"

class Export_DLG : public BASE_DLG::Export_DLG
{
	protected:
		void m_sdbSizer1OnCancelButtonClick( wxCommandEvent& event );

	public:
		Export_DLG( wxWindow* parent );

		void SetMax(int max);
		void Step(const char* fname);

		bool IsCanceled;

	private:
		int pos;
		wxDateTime cur_time;
		unsigned next_update_tick;
};

#endif
