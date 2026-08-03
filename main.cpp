#include <Application.h>
#include <Window.h>
#include <GroupView.h>
#include <MenuBar.h>
#include <String.h>
#include <StringView.h>
#include <View.h>
#include <AboutWindow.h>
#include <AppKit.h>
#include <Looper.h>

#include <GroupLayout.h>
#include <LayoutBuilder.h>

#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>

#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <Message.h>
#include <Path.h>

#include <DateTime.h>
#include <TimeZone.h>

#include <iostream>

#include "DigitalClock.h"

const char* kSignature = "application/x-vnd.manakin-clock";
const BRect kDefaultMainWindowRect = BRect(150, 150, 0, 0);
const char* kSettingsFileName = "Manakin clock settings";

const uint32 kSettingsMessage = 'Pref';
const uint32 kTickMessage = 'tick';

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

class ClockView : public BView
{
public:
			ClockView(const char* name, BRect frame);
	virtual void	MessageReceived(BMessage* msg);
	virtual void	AttachedToWindow ();
	virtual void	Draw(BRect updateRect);
	status_t	SetTimeZone(const char* name);
private:
	uint32		count;
	BTimeZone*	timeZone;
};

class TickLooper : public BLooper {
public:
			TickLooper(const char *name);
	void		StartTimer(BMessenger replyTo);
private:
	BMessageRunner* fRunner;
};

TickLooper::TickLooper(const char *name)
	:
	BLooper(name)
{
}

void
TickLooper::StartTimer(BMessenger replyTo)
{
	BMessage msg(kTickMessage);
	fRunner = new BMessageRunner(replyTo,
					&msg,
					1000000); // 1 s
}

ClockView::ClockView(const char *name, BRect frame)
	:
	BView(frame, name, B_FOLLOW_NONE,
		B_WILL_DRAW | B_FRAME_EVENTS | B_DRAW_ON_CHILDREN)
{
	count = 0;
}

void
ClockView::AttachedToWindow ()
{
	TickLooper* worker = new TickLooper(Name());
	worker->Run();
	worker->StartTimer(BMessenger(this));
}

void
ClockView::Draw(BRect updateRect)
{
	int header_h = 25;
	BRect box = Bounds();
	box.bottom = box.top+header_h;
	SetHighColor(25, 25, 25);
	FillRect(box);

	int hour = 0;
	int minute = 0;
	if (timeZone != NULL) {
		int offset_in_minute = std::floor(timeZone->OffsetFromGMT()/60);
		BTime gmt = BTime::CurrentTime(B_GMT_TIME);
		int gmt_in_minute = gmt.Hour()*60+gmt.Minute();
		int gmt_offseted = gmt_in_minute+offset_in_minute;

		if (gmt_offseted < 0) {
			gmt_offseted += 24*60;
		}

		int hour_offseted = std::floor(gmt_offseted / 60);
		if (hour_offseted > 24) {
			hour_offseted -= 24;
		}
		int minute_offseted = gmt_offseted % 60;

		hour = hour_offseted;
		minute = minute_offseted;
	} else {
		BTime now = BTime::CurrentTime(B_LOCAL_TIME);
		hour = now.Hour();
		minute = now.Minute();
	}

	box = Bounds();
	box.top = header_h;
	SetHighColor(220,220,220);
	FillRect(box);

	uint32 height = (Bounds().Height()-header_h)*0.8;
	uint32 width = height/2;
	uint32 space = height/8;
	DigitalClock disp(width, height, height/10);
	float x = space;
	disp.DrawDigit(this, BPoint(x,header_h+space), std::floor(hour/10));
	x += width+space;
	disp.DrawDigit(this, BPoint(x,header_h+space), hour%10);
	x += width+space*2;
	disp.DrawColon(this, BPoint(x,header_h+space), true);
	x += space*2;

	disp.DrawDigit(this, BPoint(x,header_h+space), std::floor(minute/10));
	x += width+space;
	disp.DrawDigit(this, BPoint(x,header_h+space), minute%10);
	x += width+space*3;
	disp.DrawDigit(this, BPoint(x,header_h+space), count%10);
}

void
ClockView::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
		case kTickMessage:
		{
			count++;
			std::cout << "Tick to " << Name() << " " << count << std::endl;
			Invalidate();
			break;
		}
		default:
			BView::MessageReceived(msg);
	}
}

status_t
ClockView::SetTimeZone(const char* name)
{
	timeZone = new BTimeZone(name);
	status_t status = timeZone->InitCheck();
	if (status != B_OK) {
		timeZone = NULL;
	}
	return status;
}

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
	ClockView *clock1 = new ClockView("clock1", BRect(0, 0, 300, 100));
	ClockView *clock2 = new ClockView("clock2", BRect(0, 0, 300, 100));
	clock2->SetTimeZone("Asia/Tokyo");

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.Add(_PrepareMenuBar())
		.AddGroup(B_VERTICAL)
			.Add(clock1)
			.Add(clock2)
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
