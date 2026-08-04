#include <View.h>
#include <TimeZone.h>
#include <Looper.h>
#include <MessageRunner.h>

class TickLooper : public BLooper {
public:
                        TickLooper(const char *name);
        void            StartTimer(BMessenger replyTo);
private:
        BMessageRunner* fRunner;
};

class ClockView : public BView
{
public:
			ClockView(const char* name, BRect frame);
	virtual void	MessageReceived(BMessage* msg);
	virtual void	AttachedToWindow ();
	status_t	SetTimeZone(const char* name);
protected:
	BTimeZone*	timeZone;
private:
};
