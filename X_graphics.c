#include <stdio.h>
#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/Xatom.h>

char X_graphics_Id[]="$Id: X_graphics.c,v 1.1 1994/01/07 18:31:05 spb Exp $";

Atom wm_protocols;
Atom wm_delete_window;

#ifndef FALSE
#define TRUE 1
#define FALSE 0
#endif
  Display *dpy;
  int screen;
  int graph_width;
  int graph_height;
  Window parent, win, root;
  int use_root=FALSE;

  GC gc;
  Pixmap pix;
  XColor table[256];

/*{{{void finish_graphics()*/
void finish_graphics()
{
  int x,y,border,depth;
  
  XGetGeometry(dpy,win,&root,&x,&y,&graph_width,&graph_height,&border,&depth);
  XClearArea(dpy,win,0,0,graph_width,graph_height,FALSE);
  XCloseDisplay(dpy);
}
/*}}}*/

/*{{{void zap_events()*/
void zap_events()
{
  XEvent event;
  XExposeEvent *expose = (XExposeEvent *)&event;
  int exw, exh;
  if( ! use_root ) /* no events if on root window */
  {
    while( XPending(dpy) ){
      XNextEvent(dpy, &event);
      switch(event.type) {
        case ClientMessage:
          if (event.xclient.message_type == wm_protocols &&
            event.xclient.data.l[0] == wm_delete_window)  {
              finish_prog();
            }
              break;
          case ButtonPress:
              break;
          case ButtonRelease:
                finish_prog();
              break;
          case Expose:
            if( (expose->x < graph_width) && (expose->y < graph_height))
            {
              if( (expose->x + expose->width) > graph_width)
              {
                exw=graph_width - expose->x;
              }else{
                exw = expose->width;
              }
              if( (expose->y + expose->height) > graph_height)
              {
                exh=graph_height - expose->y;
              }else{
                exh = expose->height;
              }
              XCopyArea(dpy,pix,win,gc,expose->x,expose->y,
                            exw,exh,
                            expose->x,expose->y);
  	  }
  	  break;
          default:
              fprintf(stderr,"unrecognized event %d\n",event.type);
              XCloseDisplay(dpy);
              exit(1);
              break;
      }
    }
  }
}
/*}}}*/

/*{{{void init_graphics(int use_root, int *s_graph_width, int *s_graph_height)*/
void init_graphics( int want_use_root, int *s_graph_width, int *s_graph_height)
{
/*{{{defs*/
  char *display=NULL;       /* name of display to open, NULL for default */
  char *progname;
  int depth=0;              /* positive value requires this many planes */
  Visual *vis;
  int mask;
  int count;
  int x=0;
  int y=0;
  unsigned long attmask;
  XSetWindowAttributes attributes;
  char * winname="Mountains";
  XTextProperty textprop;

  int border;
  unsigned long gcvmask;
  XGCValues gcv;

/*}}}*/

  use_root = want_use_root;
  graph_width = *s_graph_width;
  graph_height = *s_graph_height;
/*{{{open display*/

  dpy = XOpenDisplay(display);
  
  if( ! dpy )
  {
    fprintf(stderr,"failed to open display\n");
    exit(1);
  }
  screen = DefaultScreen(dpy);
  parent = RootWindow(dpy, screen);
/*}}}*/
/*{{{find appropriate vis*/
  vis = DefaultVisual(dpy,screen);
  depth = DefaultDepth(dpy,screen);
/*}}}*/
/*{{{create window*/
  if( use_root )
  {
    win = parent;
  }else{
    attmask = 0;
    attmask |= CWEventMask;
    attributes.event_mask = ButtonPressMask|ButtonReleaseMask|ExposureMask;
    attmask |= CWBackPixel;
    attributes.background_pixel = BlackPixel(dpy,screen);
    attmask |= CWBackingStore;
    attributes.backing_store = NotUseful;
    win = XCreateWindow(dpy,parent,x,y,graph_width,graph_height,0,
      depth,InputOutput,vis,attmask,&attributes);
  
    /* Setup for ICCCM delete window. */
    wm_protocols = XInternAtom(dpy, "WM_PROTOCOLS", False);
    wm_delete_window = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    (void) XSetWMProtocols (dpy, win, &wm_delete_window, 1);
    textprop.value = (unsigned char *) winname;
    textprop.encoding = XA_STRING;
    textprop.format = 8;
    textprop.nitems = strlen(winname);
    XSetWMName(dpy,win,&textprop);
  }
/*}}}*/
/*{{{create pixmap*/
  XGetGeometry(dpy,win,&root,&x,&y,&graph_width,&graph_height,&border,&depth);
  gcvmask = 0;
  gcvmask |= GCForeground;
  gcv.foreground = WhitePixel(dpy,screen);
  gcvmask |= GCBackground;
  gcv.background = BlackPixel(dpy,screen);
  gcvmask |= GCFillStyle;
  gcv.fill_style = FillSolid;
  gcvmask |= GCGraphicsExposures;
  gcv.graphics_exposures = FALSE;
  gc = XCreateGC(dpy,win,gcvmask,&gcv);
  pix = XCreatePixmap(dpy,win,graph_width,graph_height,depth);
/*}}}*/
  XSetForeground(dpy,gc,BlackPixel(dpy,screen));
  XFillRectangle(dpy,pix,gc,0,0,graph_width,graph_height); 
  XMapWindow(dpy, win );

  zap_events();
  
  *s_graph_width = graph_width;
  *s_graph_height = graph_height;
}
/*}}}*/

/*{{{void install_clut( int ncol, unsigned char *red, unsigned char *green, unsigned char *blue )*/
void install_clut( int ncol, unsigned char *red, unsigned char *green, unsigned char *blue )
{
  int i;
  int newmap=FALSE;
  Colormap map;

  map=DefaultColormap(dpy,screen);

  for(i=0; i<ncol ; i++)
  {
    table[i].red   = red[i]*257;
    table[i].green = green[i]*257;
    table[i].blue  = blue[i]*257;
    if( ! XAllocColor(dpy,map,table+i) )
    {
      fprintf(stderr,"failed to allocate colour %d\n",i);
      finish_graphics();
      exit(1);
    }
  }
}
/*}}}*/

/*{{{void scroll_screen( int dist )*/
void scroll_screen( int dist )
{
  zap_events();
  /* scroll the pixmap */
  if( dist > graph_width )
  {
    dist = graph_width;
  }
  /* copy the data */
  XCopyArea(dpy,pix,pix,gc,dist,0,graph_width-dist,graph_height,0,0);
  /* blank new region */
  XSetForeground(dpy,gc,BlackPixel(dpy,screen));
  XFillRectangle(dpy,pix,gc,graph_width-dist,0,dist,graph_height);
  /* update the window to match */
  XCopyArea(dpy,pix,win,gc,0,0,graph_width,graph_height,0,0);

  zap_events();
}
/*}}}*/

/*{{{void plot_pixel( int x, int y, unsigned char value )*/
void plot_pixel( int x, int y, unsigned char value )
{
  zap_events();
  XSetForeground(dpy,gc,table[value].pixel);
  XDrawPoint(dpy,pix,gc,x,y);
}
/*}}}*/


/*{{{void flush_region(int x, int y, int w, int h)*/
void flush_region(int x, int y, int w, int h)
{
  XCopyArea(dpy,pix,win,gc,x,y,w,h,x,y);
}
/*}}}*/

