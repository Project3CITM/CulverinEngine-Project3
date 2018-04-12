
#include "Color.h"

Color Red = Color(1.0f, 0.0f, 0.0f);
Color Green = Color(0.0f, 1.0f, 0.0f);
Color Blue = Color(0.0f, 0.0f, 1.0f);
Color Black = Color(0.0f, 0.0f, 0.0f);
Color White = Color(1.0f, 1.0f, 1.0f);
Color Yellow = Color(1.0f, 1.0f, 0.0f);
Color Purpule = Color(1.0f, 0.0f, 1.0f);
Color ElectricBlue = Color(0.2f, 0.6f, 1.0f);
Color ElectricRed = Color(1.0f, 0.2f, 0.2f);
Color Brown = Color(0.4, 0.2, 0);

bool Color::operator==(Color test)
{
	return(this->r == test.r && this->g == test.g && this->b == test.b);
}
