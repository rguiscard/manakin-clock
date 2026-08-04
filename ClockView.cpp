#include <DateTime.h>
#include <TimeZone.h>
#include <Looper.h>
#include <MessageRunner.h>

#include <iostream>

#include "Message.h"
#include "ClockView.h"
#include "DigitalClock.h"

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

	MovePenTo(5, 20);
	SetHighColor(200, 200, 200);
	if (timeZone == NULL) {
		DrawString("Local");
	} else {
		DrawString(timeZone->Name());
	}

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
