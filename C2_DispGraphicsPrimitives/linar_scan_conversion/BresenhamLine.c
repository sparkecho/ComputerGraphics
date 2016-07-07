/* Bresenham 画线算法 */

void BresenhamLine(int x1, int y1, int x2, int y2)
{
    int x, y, dx, dy, p;
    x = x1;
    y = y1;
    dx = x2 - x1;
    dy = y2 - y1;
    p = 2*dy - dx;
    for( ; x <= x2; x++ )
    {
	SetPixel(x, y, color);
	if(p >= 0)
	{
	    y++;
	    p += 2 * (dy - dx);
	}
	else
	{
	    p += 2 * dy;
	}
    }
}
