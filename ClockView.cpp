#include <DateTime.h>

#include <iostream>

#include "Message.h"
#include "ClockView.h"

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
ClockView::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
		case kTickMessage:
		{
			count++;
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
