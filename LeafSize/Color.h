#pragma once

struct Color
{
	int r;
	int g;
	int b;
	int a;
	Color(int nr = 0xFF, int ng = 0xFF, int nb = 0xFF, int na = 0xFF)
	{
		r = nr;
		g = ng;
		b = nb;
		a = na;
	}
};

