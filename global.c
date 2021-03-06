#include "crinkle.h"
#include "paint.h"

Fold *top;
int levels = 10;
int stop = 2;
int smooth = 1;
int cross = TRUE;
int slope = 1;
int snooze_time = 10;
int n_col=DEF_COL;
int band_size=BAND_SIZE;
int request_clear = FALSE;
#ifdef USE_E_EVENTS
int e_events = TRUE;
#else
int e_events = FALSE;
#endif
Height start=0.0;      /* starting value for the surface */
Height mean=0.0;   /* mean value of surface */
Height varience;   /* rough estimate of the height of the range */
Height shift=0.5;    
Height delta_shadow; /* offset of shadow at each step */
float stretch=0.6;   
float contour = 0.3;
float ambient = 0.3; 
float contrast = 1.0; 
float vfract   = 0.6; 
float altitude = 2.5;
float distance = 4.0;
double phi=(40.0 * PI)/180.0; 
double alpha=0.0;             
                              
double shadow_slip;
double shadow_register=0.0;
double cos_phi;
double sin_phi;
double tan_phi;
double x_fact;
double y_fact;
Height sealevel = 0.0;
Height forceheight  = -1.0;
int width;        /* width of the landscape, (function of levels) */
int seed=0;       /* zero means read the clock */

/*
 * viewport parameters
 */
int height;                       /* height of the screen */
double vangle;                    /* view angle 0 == horizontal
                                   *increase to look down
                                   */
double tan_vangle;                                     
float vscale;           
float viewpos;        
float viewheight;      /* height of viewpoint */
float focal;                  /* focal length, calc to preserve aspect ratio */

Height *shadow;               /* height of the shadows */
Height *a_strip, *b_strip;    /* the two most recent strips */

