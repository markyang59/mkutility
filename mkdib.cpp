#include "mkdib.h"

DIB* dib_create(int32_t w,int32_t h)
{	
	DIB* c = new DIB;
	if (c != NULL)
	{
		BITMAPINFO bmp_info;
		bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmp_info.bmiHeader.biWidth         = w;
		bmp_info.bmiHeader.biHeight        = -h;
		bmp_info.bmiHeader.biPlanes        = 1;
		bmp_info.bmiHeader.biBitCount      = 32;
		bmp_info.bmiHeader.biCompression   = BI_RGB;
		bmp_info.bmiHeader.biSizeImage     = 0;
		bmp_info.bmiHeader.biXPelsPerMeter = 0;
		bmp_info.bmiHeader.biYPelsPerMeter = 0;
		bmp_info.bmiHeader.biClrUsed       = 0;
		bmp_info.bmiHeader.biClrImportant  = 0;
		c->dc       = ::CreateCompatibleDC(0);
		c->p        = 0;
		c->bmp      = ::CreateDIBSection(c->dc, &bmp_info, DIB_RGB_COLORS, &(c->p), 0, 0);
		if(c->bmp != NULL) ::SelectObject(c->dc, c->bmp);
		c->w        = w;
		c->h        = h;
		c->flipy    = false;
		c->fill     = false;
		c->pts      = 0;
		c->framenum = 0;	
		c->var      = 0;
	}
	return c;
}

bool dib_delete(PBYTE p)
{	
	bool ret = false;
	if(p != NULL)
	{
		DIB* c = (DIB*) p;
		DeleteObject(c->bmp);
		DeleteObject(c->dc);
		c->bmp      = NULL;
		c->dc       = NULL;
		c->p        = NULL;
		c->w        = 0;
		c->h        = 0;
		c->flipy    = false;	
		c->fill     = false;
		c->pts      = 0;
		c->framenum = 0;
		c->var      = 0;
		delete c;
		ret = true;
	}
	return ret;
}

// Make full dib copy
DIB* dib_duplicate (DIB* s)
{
	DIB* d  = NULL;
	if ( s != NULL)
	{
		d = new DIB;
		BITMAPINFO bmp_info;
		bmp_info.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
		bmp_info.bmiHeader.biWidth         =  (s->w);
		bmp_info.bmiHeader.biHeight        = -(s->h);
		bmp_info.bmiHeader.biPlanes        = 1;
		bmp_info.bmiHeader.biBitCount      = 32;
		bmp_info.bmiHeader.biCompression   = BI_RGB;
		bmp_info.bmiHeader.biSizeImage     = 0;
		bmp_info.bmiHeader.biXPelsPerMeter = 0;
		bmp_info.bmiHeader.biYPelsPerMeter = 0;
		bmp_info.bmiHeader.biClrUsed       = 0;
		bmp_info.bmiHeader.biClrImportant  = 0;
		d->dc    = ::CreateCompatibleDC(0);
		d->p     = 0;
		d->bmp   = ::CreateDIBSection(d->dc, &bmp_info, DIB_RGB_COLORS, &(d->p), 0, 0);
		if (d->bmp != NULL) ::SelectObject(d->dc, d->bmp);
		d->w     = s->w;
		d->h     = s->h;
		d->flipy = false;
		d->fill  = false;
		d->pts   = s->pts;
		d->framenum = s->framenum;
		d->var   = s->var;

		// Copy Contents
		::BitBlt(d->dc,0,0,d->w,d->h,s->dc,0,0,SRCCOPY);
	}
	return d;
}

void dib_replace (DIB* d,DIB* s)
{
	if (d != NULL && s != NULL)
	{
		d->w = min(d->w, s->w);
		d->h = min(d->h, s->h);
		::BitBlt(d->dc, 0, 0, d->w, d->h, s->dc, 0, 0, SRCCOPY);
	}
}
