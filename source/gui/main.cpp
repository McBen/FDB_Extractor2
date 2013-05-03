#include "stdafx.h"
#include "main.h"
#include "MainWindow.h"

bool MyApp::OnInit()
{
	wxInitAllImageHandlers();

	MainWindow *frame = new MainWindow();
	frame->Show(true);
	SetTopWindow(frame);
	return true;
};

IMPLEMENT_APP(MyApp)

