/* DDA Linear scan conversion algorithm */
/* DDA(Digital Differential Analyzer) 直线扫描转换算法 */

void DDALine(int x1, int y1, int x2, int y2)
{
    double dx, dy, e, x, y;
    dx = x2 - x1;
    dy = y2 - y1;
    e = (fabs(dx) > fabs(dy)) ? fabs(dx) : fabs(dy);
    dx /= e;
    dy /= e;
    x = x1;
    y = y1;
    for(int i = 0; i <= e; i++)
    {
	SetPixel((int)(x + 0.5), (int)(y + 0.5), color);
	x += dx;
	y += dy;
    }
}
