/*
 *
 * $Id: hanoi.h,v 1.4 2006/02/28 20:52:31 mmr Exp $
 *
 */

#ifdef _WIN32
  #include <time.h>
#else
  #include <sys/time.h>
#endif

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <GL/glut.h>
#endif

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Constants */
  /* Window */
#define b1n_WINDOW_X  640 /* Window's Width   */
#define b1n_WINDOW_Y  480 /* Window's Height  */

  /* Pillars */
#define b1n_PILLARS         3
#define b1n_PILLARS_RADIUS  0.1
#define b1n_PILLARS_HEIGHT  2

  /* Discs */
#define b1n_DISCS_DEFAULT 5
#define b1n_DISCS_MIN     2
#define b1n_DISCS_MAX     10
#define b1n_DISCS_HEIGHT  0.1
#define b1n_DISCS_SPACING 0.18

  /* Misc */
#define b1n_ROT_STEP  10
#define b1n_TRUE    GL_TRUE 
#define b1n_FALSE   GL_FALSE
#define b1n_FAIL    -1
#define b1n_SUCCESS 0

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Macros */
/* #define b1n_FIND_LAST(a) while(a) */
#define b1n_RANDOM2(min,max)  min+((int)((float)rand()/RAND_MAX*(max-min)))
#define b1n_RANDOM(max) b1n_RANDOM2(0, max)
#define b1n_RANDCOLOR   glColor3b(b1n_RANDOM(255), b1n_RANDOM(255), b1n_RANDOM(255));

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Types */
  /* Discs */
typedef struct b1n_disc {
  GLfloat radius;   /* Radius */
  GLubyte r, g, b;  /* Color */  
} b1n_disc;

  /* Pillars */
typedef struct b1n_pillar {
  /* Pillar's Discs (+1 is due to not start count at 1 (and not 0) */
  int d[b1n_DISCS_MAX+1];
  int discs;    /* Discs Quantity in this Pilllar */
  GLubyte r, g, b;  /* Color    */
  GLfloat x, y, z;  /* Position */
} b1n_pillar;

  /* World */
typedef struct b1n_world {
  GLboolean use_light:1;  /* Use light */
  GLboolean use_blend:1;  /* Use Blend (Translucid) */
  GLboolean use_fog:1;    /* Use Fog */
  GLboolean use_texture:1;    /* Use Texture */
  GLboolean use_background:1; /* Use Background */
  GLfloat   rot_x, rot_y, rot_z;  /* Rotation coordinates */
} b1n_world;

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Prototypes */
  /* Draw */
void  b1n_drawDisc(int); /* Draw a disc */
void  b1n_drawCylinder(GLdouble, GLdouble, GLdouble); /* Draw a cylinder */

  /* Setup */
void  b1n_setupRender(void);
void  b1n_setupWorld(void);
void  b1n_setupDiscs(void);
void  b1n_setupPillars(void);

  /* Game */
void  b1n_gameSetup(void);  /* Setup Game */
void  b1n_gameDraw(void);   /* Draw pillars and discs */
void  b1n_gameSolve(void);  /* Solve Game */
void  b1n_gameQuit(void);   /* Exit gracefully */

  /* CallBack */
void  b1n_callbackReshape(GLint, GLint);
void  b1n_callbackMouse(void);
void  b1n_callbackKeyboard(unsigned char, int, int);
static void b1n_callbackSpecialkey(int, int, int);

  /* Stack */
void  b1n_stackPush(int, int);  /* Push an item into the Stack */
int   b1n_stackPop(int);        /* Pop an item from the Stack */

  /* Misc */
void  b1n_usage(const char *);
void  b1n_bail(int, const char *);
