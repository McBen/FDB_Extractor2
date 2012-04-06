#include "stdafx.h"
#include "main.h"
#include "mainwindow.h"

bool MyApp::OnInit()
{
	wxInitAllImageHandlers();

	MainWindow *frame = new MainWindow();
	frame->Show(true);
	SetTopWindow(frame);
	return true;
};

IMPLEMENT_APP(MyApp)

