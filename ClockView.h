#include <View.h>
#include <TimeZone.h>

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
	virtual void	Draw(BRect updateRect);
	status_t	SetTimeZone(const char* name);
private:
	uint32		count;
	BTimeZone*	timeZone;
};
