#include <DateTime.h>

#include "DigitalClock.h"

static const uint8 kDigits[10] =
{
	DigitalClock::SEG_A |
	DigitalClock::SEG_B |
	DigitalClock::SEG_C |
	DigitalClock::SEG_D |
	DigitalClock::SEG_E |
	DigitalClock::SEG_F,

	DigitalClock::SEG_B |
	DigitalClock::SEG_C,

	DigitalClock::SEG_A |
	DigitalClock::SEG_B |
	DigitalClock::SEG_D |
	DigitalClock::SEG_E |
	DigitalClock::SEG_G,

	DigitalClock::SEG_A |
	DigitalClock::SEG_B |
	DigitalClock::SEG_C |
	DigitalClock::SEG_D |
	DigitalClock::SEG_G,

	DigitalClock::SEG_B |
	DigitalClock::SEG_C |
	DigitalClock::SEG_F |
	DigitalClock::SEG_G,

	DigitalClock::SEG_A |
	DigitalClock::SEG_C |
	DigitalClock::SEG_D |
	DigitalClock::SEG_F |
	DigitalClock::SEG_G,

	DigitalClock::SEG_A |
	DigitalClock::SEG_C |
	DigitalClock::SEG_D |
	DigitalClock::SEG_E |
	DigitalClock::SEG_F |
	DigitalClock::SEG_G,

	DigitalClock::SEG_A |
	DigitalClock::SEG_B |
	DigitalClock::SEG_C,

	DigitalClock::SEG_A |
	DigitalClock::SEG_B |
	DigitalClock::SEG_C |
	DigitalClock::SEG_D |
	DigitalClock::SEG_E |
	DigitalClock::SEG_F |
	DigitalClock::SEG_G,

	DigitalClock::SEG_A |
	DigitalClock::SEG_B |
	DigitalClock::SEG_C |
	DigitalClock::SEG_D |
	DigitalClock::SEG_F |
	DigitalClock::SEG_G
};

static constexpr uint8 LETTER_A =
	DigitalClock::SEG_A |
	DigitalClock::SEG_B |
	DigitalClock::SEG_C |
	DigitalClock::SEG_E |
	DigitalClock::SEG_F |
	DigitalClock::SEG_G;

static constexpr uint8 LETTER_P =
	DigitalClock::SEG_A |
	DigitalClock::SEG_B |
	DigitalClock::SEG_E |
	DigitalClock::SEG_F |
	DigitalClock::SEG_G;

static constexpr uint8 LETTER_M =
	DigitalClock::SEG_A |
	DigitalClock::SEG_B |
	DigitalClock::SEG_C |
	DigitalClock::SEG_E |
	DigitalClock::SEG_F;


DigitalClock::Polygon
DigitalClock::MakeHorizontal(float x,float y,float len,float t)
{
	Polygon poly;

	float b = t * 0.45f;

	poly.p[0]={x+b,y};
	poly.p[1]={x+len-b,y};
	poly.p[2]={x+len,y+t/2};
	poly.p[3]={x+len-b,y+t};
	poly.p[4]={x+b,y+t};
	poly.p[5]={x,y+t/2};

	return poly;
}


DigitalClock::Polygon
DigitalClock::MakeVertical(float x,float y,float len,float t)
{
	Polygon poly;

	float b=t*0.45f;

	poly.p[0]={x,y+b};
	poly.p[1]={x+t/2,y};
	poly.p[2]={x+t,y+b};
	poly.p[3]={x+t,y+len-b};
	poly.p[4]={x+t/2,y+len};
	poly.p[5]={x,y+len-b};

	return poly;
}


DigitalClock:: DigitalClock(float width, float height, float t)
{
	fWidth=width;
	fHeight=height;
	fThickness=t;

	fOnColor={55,55,255,255};
	fOffColor={220,220,220,255};

	float h = width - 2*t;
	float v = (height - 3*t)/2;

	fSegments[0]=MakeHorizontal(t,0,h,t);
	fSegments[1]=MakeVertical(width-t,t,v,t);
	fSegments[2]=MakeVertical(width-t,2*t+v,v,t);
	fSegments[3]=MakeHorizontal(t,height-t,h,t);
	fSegments[4]=MakeVertical(0,2*t+v,v,t);
	fSegments[5]=MakeVertical(0,t,v,t);
	fSegments[6]=MakeHorizontal(t,t+v,h,t);
}


void
DigitalClock::SetColors(rgb_color on, rgb_color off)
{
	fOnColor=on;
	fOffColor=off;
}


void
DigitalClock::DrawMask(BView* view,
                              BPoint where,
                              uint8 mask) const
{
	view->PushState();
	view->TranslateBy(where.x, where.y);

	for (int i=0;i<7;i++)
	{
		view->SetHighColor(
			(mask&(1<<i))
			? fOnColor
			: fOffColor);

		view->FillPolygon(fSegments[i].p,6);
	}

	view->PopState();
}


void
DigitalClock::DrawDigit(BView* view,
                               BPoint where,
                               int digit) const
{
	if (digit<0 || digit>9)
		return;

	DrawMask(view,where,kDigits[digit]);
}


void
DigitalClock::DrawLetter(BView* view,
                                BPoint where,
                                char c) const
{
	switch(c)
	{
		case 'A':
		case 'a':
			DrawMask(view,where,LETTER_A);
			break;

		case 'P':
		case 'p':
			DrawMask(view,where,LETTER_P);
			break;

		case 'M':
		case 'm':
			DrawMask(view,where,LETTER_M);
			break;

		default:
			DrawMask(view,where,0);
    }
}


void
DigitalClock::DrawColon(BView* view,
                               BPoint where,
                               bool on) const
{
	view->PushState();
	view->TranslateBy(where.x,where.y);

	view->SetHighColor(on ? fOnColor : fOffColor);

	float r=fThickness*0.55f;

	view->FillEllipse(BPoint(0,fHeight*0.30f),r,r);
	view->FillEllipse(BPoint(0,fHeight*0.70f),r,r);

	view->PopState();
}

DigitalClockView::DigitalClockView(const char *name, BRect frame)
        :
        ClockView(name, frame)
{
}

void
DigitalClockView::Draw(BRect updateRect)
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
