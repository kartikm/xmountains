/* $Id: crinkle.h,v 1.4 1994/01/21 11:53:47 spb Rel $ */
#ifndef CRINKLE
#define CRINKLE
/*{{{  typedefs */
typedef float Height;
typedef float Length;
/*}}}*/
/*{{{  defines */
#ifndef NULL
#define NULL (void *) 0
#endif
#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

#define START 0
#define STORE 1

/*}}}*/
/*{{{  structs */
/* strip of altitudes */
typedef struct strip{
  int level;
  Height *d;    /* should have 2^level + 1 points */
}Strip;

/* The parameter struct for the recursive procedure */
typedef struct fold{
  int level;                /* levels of recursion below us */
  int stop;                 /* level to stop recursion */
  int state;                /* internal stat of algorithm */
  int smooth;               /* is smoothing on or off */
  int slope;                /* constrain the front of the surface */
  Height mean;              /* mean altitude */
  Length scale;             /* scale factor for perturbations */
  Length midscale;          /* as above but for diagonal offsets */
  struct fold *next;        /* struct for next level of recursion */
  struct strip *new;        /* incoming results from the previous level */
  struct strip *working;    /* strip being expanded up */
  struct strip *regen;      /* strip being recalculated as part of smoothing */
  struct strip *old;        /* finished results retained for averaging */
} Fold;
/*}}}*/
/*{{{  prototypes */
#ifdef ANSI
Strip *make_strip (int );
void free_strip (Strip *);
Strip *double_strip (Strip *);;
Strip *set_strip (int , Height );
void side_update (Strip *, Length );
void mid_update (Strip *, Strip *, Strip *,Length , Length );
void recalc (Strip *, Strip *, Strip *,Length );
Strip *next_strip (Fold *);
Fold *make_fold (int ,int ,int , int, int, Length , Height , Height , float );
void free_fold (Fold *);
Length gaussian ();
#else
Strip *make_strip ();
void free_strip ();
Strip *double_strip ();
Strip *set_strip ();
void side_update ();
void mid_update ();
void recalc ();
Strip *next_strip ();
Fold *make_fold ();
void free_fold ();
Length gaussian ();
#endif
/*}}}*/
#endif
