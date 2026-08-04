#include <Application.h>
#include <Window.h>
#include <GroupView.h>
#include <MenuBar.h>
#include <String.h>
#include <StringView.h>
#include <View.h>
#include <AboutWindow.h>
#include <AppKit.h>

#include <GroupLayout.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>

#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>

#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <Message.h>
#include <Path.h>

#include <iostream>

#include "Message.h"
#include "DigitalClock.h"

const char* kSignature = "application/x-vnd.manakin-clock";
const BRect kDefaultMainWindowRect = BRect(150, 150, 0, 0);
const char* kSettingsFileName = "Manakin clock settings";

class App : public BApplication
{
private:
	BWindow* window;

public:
	App(void);
};

class MainWindow : public BWindow
{
public:
			MainWindow(void);
	virtual void	MessageReceived(BMessage* msg);

	virtual bool	QuitRequested();
	virtual void	AboutRequested();

private:
	status_t	_LoadSettings(BMessage& m);
	status_t	_SaveSettings();
	BMenuBar*	_PrepareMenuBar(void);

	BRect		fMainWindowRect;
};

BMenuBar*
MainWindow::_PrepareMenuBar(void)
{
	BMenuBar* menubar = new BMenuBar("menu");
	BMenu* menu = new BMenu("File");
	menu->AddItem(new BMenuItem("About", new BMessage(B_ABOUT_REQUESTED)));
	menu->AddItem(new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'Q'));
	menubar->AddItem(menu);

	return menubar;
}


MainWindow::MainWindow()
	:
	BWindow(BRect(150, 150, 0, 0), "Clock",
		B_TITLED_WINDOW,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_ASYNCHRONOUS_CONTROLS
			| B_QUIT_ON_WINDOW_CLOSE | B_AUTO_UPDATE_SIZE_LIMITS)
{
	DigitalClockView *clock1 = new DigitalClockView("clock1", BRect(0, 0, 300, 100));
	DigitalClockView *clock2 = new DigitalClockView("clock2", BRect(0, 0, 300, 100));
	clock2->SetTimeZone("Asia/Tokyo");
	DigitalClockView *clock3 = new DigitalClockView("clock3", BRect(0, 0, 300, 100));
	clock3->SetTimeZone("America/New_York");

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.Add(_PrepareMenuBar())
		.AddGroup(B_VERTICAL)
			.Add(clock1)
			.Add(new BSeparatorView(B_HORIZONTAL))
			.Add(clock2)
			.Add(new BSeparatorView(B_HORIZONTAL))
			.Add(clock3)
			.End()
		.End();

	BMessage settings;
	_LoadSettings(settings);
	if (settings.FindRect("fMainWindowRect", &fMainWindowRect) != B_OK)
		fMainWindowRect = kDefaultMainWindowRect;
	MoveTo(fMainWindowRect.LeftTop());
}


void
MainWindow::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
		case B_ABOUT_REQUESTED:
			AboutRequested();
			break;
		default:
			BWindow::MessageReceived(msg);
	}
}

void
MainWindow::AboutRequested()
{
	BAboutWindow* about = new BAboutWindow("Clock", "application/x-vnd.manakin-clock");

	const char* kAuthors[] = {
		"Robert Guiscard",
		NULL
	};

	const char* kCopyright = "Robert Guiscard";

	about->AddDescription("An open source clock app");
	about->AddAuthors(kAuthors);
	about->AddCopyright(2026, kCopyright, NULL);
	about->Show();
}


bool
MainWindow::QuitRequested()
{
	_SaveSettings();
	return true;
}

status_t
MainWindow::_LoadSettings(BMessage& m)
{
        BPath p;
        BFile f;

        if (find_directory(B_USER_SETTINGS_DIRECTORY, &p) != B_OK)
                return B_ERROR;
        p.Append(kSettingsFileName);

        f.SetTo(p.Path(), B_READ_ONLY);
        if (f.InitCheck() != B_OK)
                return B_ERROR;

        if (m.Unflatten(&f) != B_OK)
                return B_ERROR;

        if (m.what != kSettingsMessage)
                return B_ERROR;

        return B_OK;
}

status_t
MainWindow::_SaveSettings()
{
        BPath p;
        BFile f;
        BMessage m(kSettingsMessage);

        m.AddRect("fMainWindowRect", Frame());

        app_info info;
        be_roster->GetAppInfo("application/x-vnd.manakin-clock", &info);
        m.AddRef("appLocation", &info.ref);

        if (find_directory(B_USER_SETTINGS_DIRECTORY, &p) != B_OK)
                return B_ERROR;
        p.Append(kSettingsFileName);

        f.SetTo(p.Path(), B_WRITE_ONLY | B_ERASE_FILE | B_CREATE_FILE);
        if (f.InitCheck() != B_OK)
                return B_ERROR;

        if (m.Flatten(&f) != B_OK)
                return B_ERROR;

        return B_OK;
}

App::App(void)
	:
	BApplication(kSignature)
{
	MainWindow* mw = new MainWindow();
	mw->Show();
}


int
main()
{
	App* mApp = new App();
	mApp->Run();
	delete mApp;
	return 0;
}
