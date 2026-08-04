#include <View.h>
#include "ClockView.h"

class DigitalClock
{
public:
	enum : uint8 {
		SEG_A = 1 << 0,
		SEG_B = 1 << 1,
		SEG_C = 1 << 2,
		SEG_D = 1 << 3,
		SEG_E = 1 << 4,
		SEG_F = 1 << 5,
		SEG_G = 1 << 6,
	};

		DigitalClock(float width,
			float height,
			float thickness);

	void	SetColors(rgb_color on, rgb_color off);

	void	DrawMask(BView* view,
			BPoint where,
			uint8 mask) const;

	void	DrawDigit(BView* view,
			BPoint where,
			int digit) const;

	void	DrawLetter(BView* view,
			BPoint where,
			char letter) const;

	void	DrawColon(BView* view,
			BPoint where,
			bool on = true) const;

private:
	struct Polygon {
		BPoint p[6];
	};

	Polygon MakeHorizontal(float x, float y, float length, float t);
	Polygon MakeVertical(float x, float y, float length, float t);

	Polygon fSegments[7];

	float fWidth;
	float fHeight;
	float fThickness;

	rgb_color fOnColor;
	rgb_color fOffColor;
};

class DigitalClockView : public ClockView
{
public:
			DigitalClockView(const char* name, BRect frame);
	virtual void    Draw(BRect updateRect);
};
