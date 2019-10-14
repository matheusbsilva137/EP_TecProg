#include <stdlib.h>
#include <string.h>
#include "xwc.h"

static Display *display = NULL;
static Colormap cmap;

void WCor(WINDOW *w, Color c)
{
  XSetForeground(display, w->gc, c);
}

void WPlot(WINDOW *w, int x, int y, Color c)
{
  XSetForeground(display, w->gc, c);
  XDrawPoint(display, w->ptr.window, w->gc, x, y);
  XFlush(display);
}

void WLine(WINDOW *w, int x1, int y1, int x2, int y2, Color c)
{
  XSetForeground(display, w->gc, c);
  XDrawLine(display, w->ptr.window, w->gc, x1, y1, x2, y2);
  XFlush(display);
}

void WRect(WINDOW *w, int x, int y, int wd, int h, Color c)
{
  XSetForeground(display, w->gc, c);
  XDrawRectangle(display, w->ptr.window, w->gc, x, y, wd, h);
  XFlush(display);
}
  
void WFillRect(WINDOW *w, int x, int y, int wd, int h, Color c)
{
  XSetForeground(display, w->gc, c);
  XFillRectangle(display, w->ptr.window, w->gc, x, y, wd, h);
  XFlush(display);
}
  

void WArc(WINDOW *w, int x, int y, int a1, int a2, int wd, int h, Color c)
{
  XSetForeground(display, w->gc, c);
  XDrawArc(display, w->ptr.window, w->gc, x, y, wd, h, a1, a2);
  XFlush(display);
}

void WFillArc(WINDOW *w, int x, int y, int a1, int a2, int wd, int h, Color c)
{
  XSetForeground(display, w->gc, c);
  XFillArc(display, w->ptr.window, w->gc, x, y, wd, h, a1, a2);
  XFlush(display);
}


void WClear(WINDOW *w)
{
   XClearWindow(display, w->ptr.window);
   XFlush(display);
}

void WFlush()
{
   XFlush(display);
}

void WPrint(WINDOW *w, int x, int y, char *msg)
{
  XDrawString(display, w->ptr.window, w->gc, x, y, msg, strlen(msg));
  XFlush(display);
}

PIC NewPic(WINDOW *w, int wd, int h)
{
  PIC p;
  XGCValues gcv;

  gcv.function = GXcopy;
  gcv.plane_mask = 0xFFFFFFFF;
  gcv.subwindow_mode = ClipByChildren;
  gcv.clip_x_origin = gcv.clip_y_origin = 0;
  gcv.clip_mask = None;
  gcv.foreground = 1;
  gcv.background = 0;
  gcv.graphics_exposures = False;

  p = (PIC)malloc(sizeof(PIC));
  p->ptr.p = XCreatePixmap(display, w->ptr.window, wd, h,
						   DefaultDepth(display, DefaultScreen(display)));
  p->gc = XCreateGC(display, p->ptr.p, GCForeground |GCBackground| GCPlaneMask | 
		     GCSubwindowMode | GCClipXOrigin |  GCClipYOrigin |
		     GCClipMask | GCFunction | GCGraphicsExposures, &gcv);

  return p;
}

void FreePic(PIC pic)
{
   XFreePixmap(display, pic->ptr.p);
}

/* new */
MASK NewMask(WINDOW *w, int wd, int h)
{
  MASK nm;
  XGCValues gcv;

  gcv.function = GXcopy;
  gcv.plane_mask = 0xFFFFFFFF;
  gcv.subwindow_mode = ClipByChildren;
  gcv.clip_x_origin = gcv.clip_y_origin = 0;
  gcv.clip_mask = None;
  gcv.foreground = 1;
  gcv.background = 0;
  gcv.graphics_exposures = False;

  nm = (MASK) malloc(sizeof(MASK));
  nm->ptr.m = XCreatePixmap(display,w->ptr.window,wd,h,1);
  nm->gc = XCreateGC(display, nm->ptr.m, GCForeground |GCBackground| GCPlaneMask | 
		     GCSubwindowMode | GCClipXOrigin |  GCClipYOrigin |
		     GCClipMask | GCFunction | GCGraphicsExposures, &gcv);
  XSetForeground(display, nm->gc,0);
  XFillRectangle(display, nm->ptr.m, nm->gc, 0, 0, wd, h);
  XSetForeground(display, nm->gc,1);
  return nm;
}

void SetMask(PIC p, MASK mask)
{
  XSetClipMask(display, p->gc, mask->ptr.m);
}

void UnSetMask(PIC p)
{
  XSetClipMask(display, p->gc, None);
}

/* acaba aqui */


void PutPic(PIC pic1, PIC pic2, int x0, int y0, int wd, int h, int x, int y)
{
  XSetClipOrigin(display, pic1->gc, x, y);
  XCopyArea(display, pic2->ptr.p, pic1->ptr.window, pic1->gc, x0, y0, wd, h, x, y);
  XFlush(display);
}

Color WNamedColor(char *name)
{
  XColor cor1, cor2;

  if (XAllocNamedColor(display,cmap,name,&cor1,&cor2))
	return cor1.pixel;
  else return -1;
}

void WShow(WINDOW *w)
{
  XMapWindow(display, w->ptr.window);
  XFlush(display);
}

void WHide(WINDOW *w)
{
  XUnmapWindow(display, w->ptr.window);
  XFlush(display);
}

void WDestroy(WINDOW *w)
{
  XDestroyWindow(display, w->ptr.window);
}

WINDOW* InitGraph(int WIDTH, int HEIGHT, char *nome)
{
  WINDOW *nwin;
  XGCValues gcv;
  XSetWindowAttributes atts;
  Window window;
  GC gc;
  
  nwin = (WINDOW *) malloc(sizeof(WINDOW));
  if (!nwin) return NULL;

  if (!display) {
	display = XOpenDisplay("");
	if (!display) return NULL;
  }

  cmap = DefaultColormap(display, DefaultScreen(display));
  window = XCreateSimpleWindow(display, DefaultRootWindow(display), 
			       50, 50, WIDTH, HEIGHT,	0, 8, 0);
  atts.backing_store = Always ;
  atts.backing_planes = 0xFFFFFFF;
  XChangeWindowAttributes(display, window, CWBackingStore|CWBackingPlanes,
			  &atts);

  XStoreName(display, window, nome);

  
  gcv.function = GXcopy;
  gcv.plane_mask = 0xFFFFFFFF;
  gcv.subwindow_mode = ClipByChildren;
  gcv.clip_x_origin = gcv.clip_y_origin = 0;
  gcv.clip_mask = None;
  gcv.foreground = 1;
  gcv.background = 0;
  gcv.graphics_exposures = False;

  gc = XCreateGC(display, window, GCForeground |GCBackground| GCPlaneMask | 
		 GCSubwindowMode | GCClipXOrigin |  GCClipYOrigin |
		 GCClipMask | GCFunction | GCGraphicsExposures, &gcv);

  nwin->ptr.window = window;
  nwin->gc = gc;


  XClearWindow(display, window);
  XMapWindow(display, window);
  /* Espere a janela ser mapeada antes de retornar */
  XSelectInput(display, window, StructureNotifyMask);
  for(;;) {
	XEvent e;
	XNextEvent(display, &e);
	if (e.type == MapNotify)
	  break;
  }          
  return nwin;
}

void CloseGraph()
{
  XCloseDisplay(display);
}

Display *GetDisplay()
{
  return display;
}

#ifndef NOXPM
/* Arquivos  - Xpm */
PIC ReadPic(WINDOW *w, char *fname, MASK m)
{
  PIC p;
  Pixmap pm;
  XGCValues gcv;

  if (XpmReadFileToPixmap(display, w->ptr.p, fname, &pm, 
						  m ? &(m->ptr.m) : NULL, NULL))
	 return NULL;

  gcv.function = GXcopy;
  gcv.plane_mask = 0xFFFFFFFF;
  gcv.subwindow_mode = ClipByChildren;
  gcv.clip_x_origin = gcv.clip_y_origin = 0;
  gcv.clip_mask = None;
  gcv.foreground = 1;
  gcv.background = 0;
  gcv.graphics_exposures = False;
  
  p = (PIC)malloc(sizeof(PIC));
  p->ptr.p = pm;

  p->gc = XCreateGC(display, p->ptr.p, GCForeground |GCBackground| GCPlaneMask | 
		     GCSubwindowMode | GCClipXOrigin |  GCClipYOrigin |
		     GCClipMask | GCFunction | GCGraphicsExposures, &gcv);
  return p;
}

int WritePic(PIC p, char *fname, MASK m)
{
  return XpmWriteFileFromPixmap(display, fname, p->ptr.p,
								m ? m->ptr.m : 0, NULL);
}

PIC MountPic(WINDOW *w, char **data, MASK m)
{
  PIC p;
  Pixmap pm;
  XGCValues gcv;

  if (XpmCreatePixmapFromData(display, w->ptr.p, data,&pm, 
							  m? & (m->ptr.m) : 0, NULL))
	 return NULL;

  gcv.function = GXcopy;
  gcv.plane_mask = 0xFFFFFFFF;
  gcv.subwindow_mode = ClipByChildren;
  gcv.clip_x_origin = gcv.clip_y_origin = 0;
  gcv.clip_mask = None;
  gcv.foreground = 1;
  gcv.background = 0;
  gcv.graphics_exposures = False;
  
  p = (PIC)malloc(sizeof(PIC));
  p->ptr.p =  pm;
  p->gc = XCreateGC(display, p->ptr.p, GCForeground |GCBackground| GCPlaneMask | 
		     GCSubwindowMode | GCClipXOrigin |  GCClipYOrigin |
		     GCClipMask | GCFunction | GCGraphicsExposures, &gcv);

  return p;
}

#endif

void InitKBD(WINDOW *w)
{
  /*  XSelectInput (display, w->ptr.window, KeyPressMask|KeyReleaseMask);*/
  XSelectInput (display, w->ptr.window, KeyPressMask);
}

KeySym key;

int WCheckKBD(WINDOW *w)
{
  int r;
  XEvent xev;

  r = XCheckWindowEvent(display,w->ptr.window, KeyPressMask|KeyReleaseMask, &xev);
  if (r) XPutBackEvent(display, &xev);
  return r;
}

KeyCode WGetKey(WINDOW *w)
{
  XEvent xev;

  XWindowEvent(display,w->ptr.window, KeyPressMask|KeyReleaseMask, &xev);
  key = XkbKeycodeToKeysym(display, xev.xkey.keycode,
						   0, xev.xkey.state & ShiftMask ? 1 : 0);
  return xev.xkey.keycode;
}

KeySym WLastKeySym()
{
  return key;
}
