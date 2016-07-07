/* 中点画线法 */


void MidpointLine(int x0, int y0, int x1, int y1)
{
    int a, b, delta1, delta2, d, x, y;
    a = y0 - y1;
    b = x1 - x0;
    d = 2*a + b;
    delta1 = 2 * a;
    delta2 = 2 * (a + b);
    x = x0;
    y = y0;
    SetPixel(x, y, color);
    while(x < x1)
    {
	if(d < 0)
	{
	    x++;
	    y++;
	    d += delta2;
	}
	else
	{
	    x++;
	    d += delta1;
	}
	SetPixel(x, y, color);
    }
}
