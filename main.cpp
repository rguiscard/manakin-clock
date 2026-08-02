#include <Application.h>
#include <Window.h>
#include <GroupView.h>
#include <MenuBar.h>
#include <String.h>
#include <StringView.h>
#include <View.h>
#include <AboutWindow.h>
#include <AppKit.h>

#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>

const char* kSignature = "application/x-vnd.manakin-clock";
const BRect kDefaultMainWindowRect = BRect(150, 150, 0, 0);

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
	BGroupLayout* root = new BGroupLayout(B_VERTICAL);
	root->SetSpacing(0);
	SetLayout(root);

	AddChild(_PrepareMenuBar());

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
	return true;
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
