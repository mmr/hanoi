/*
$Id: hanoi_old.c,v 1.4 2006/02/28 20:57:26 mmr Exp $ 

<mmr@b1n.org>, 2004-05-31
Hanoi Tower Inocent Implementation
*/

/* Include */
#include <stdio.h>
#include <stdlib.h>

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
#define b1n_PILLARS_HEIGHT  1.5

  /* Discs */
#define b1n_DISCS_DEFAULT 2
#define b1n_DISCS_MIN     2
#define b1n_DISCS_MAX     10 

  /* Misc */
#define b1n_TRUE    1
#define b1n_FALSE   0
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
typedef struct b1n_disc {
  GLshort v;  /* Disc's value */
  GLubyte r, g, b;  /* Color */  
} b1n_disc;

typedef struct b1n_pillar {
  b1n_disc  d[b1n_DISCS_MAX]; /* Pillar's Discs */
  GLshort   discs;    /* Discs Quantity in this Pilllar */
  GLubyte   r, g, b;  /* Color    */
  GLfloat   x, y, z;  /* Position */
} b1n_pillar;

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Prototypes */
  /* Draw */
void  b1n_drawDisc(GLdouble, GLdouble); /* Draw a disc */
void  b1n_drawCylinder(GLdouble, GLdouble, GLdouble); /* Draw a cylinder */

  /* Game */
void  b1n_gameRenderSetup(void);    /* OpenGL Setup */
void  b1n_gameSetup(int, char **);  /* Pillars and Discs Setup */
void  b1n_gameDraw(void); /* Draw pillars and discs */
void  b1n_gameQuit(void); /* Exit gracefully */

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

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Global Vars */
  /* Pillars */
b1n_pillar  g_pillars[b1n_PILLARS];
b1n_disc    g_discs[b1n_DISCS_MAX];

  /* Position in Space */
GLdouble g_pos_x = 0, g_pos_y = 0;
GLUquadricObj *g_qobj;

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Main */
int
main(int argc, char **argv)
{
  /* Initializing Glut */
  glutInit(&argc, argv);
  glutInitWindowSize(b1n_WINDOW_X, b1n_WINDOW_Y);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutCreateWindow("mmr's Hanoi");

  /* Setting the Game up (render, pillars and discs) */
  b1n_gameRenderSetup();
  b1n_gameSetup(argc, argv);

  /* Setting the Callback up */
  glutReshapeFunc(b1n_callbackReshape);
  glutSpecialFunc(b1n_callbackSpecialkey);
  glutKeyboardFunc(b1n_callbackKeyboard);
  glutDisplayFunc(b1n_gameDraw);

  /* Loop */
  glutMainLoop();

  /* Just to be Nice */
  b1n_gameQuit();
  return b1n_SUCCESS;
}

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Draw */
  /* Draws a cylinder with a flat disc at each end */
void 
b1n_drawCylinder(GLdouble outerRadius, GLdouble innerRadius, GLdouble height)
{
  glPushMatrix();
    gluCylinder(g_qobj, outerRadius, outerRadius, height, 20, 1);
    glPushMatrix();
      glRotatef(180, 0.0, 1.0, 0.0);
      gluDisk(g_qobj, innerRadius, outerRadius, 20, 1);
    glPopMatrix();

    glTranslatef(0.0, 0.0, height);
    gluDisk(g_qobj, innerRadius, outerRadius, 20, 1);
  glPopMatrix();
}

void
b1n_drawDisc(GLdouble innerRadius, GLdouble outerRadius)
{
  gluDisk(g_qobj, innerRadius, outerRadius, 20, 1);
}

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Game */
/* OpenGL Setup (Background, Light, Projection) */
void
b1n_gameRenderSetup(void)
{
  GLfloat mat_specular[]  = {1.0, 1.0, 1.0, 1.0};
  GLfloat mat_shininess[] = {50.0};
  GLfloat light_position1[] = {1.0, 1.0, 1.0, 0.0};
  GLfloat light_position2[] = {-1.0, 1.0, 1.0, 0.0};

  /* Quadric */
  g_qobj = gluNewQuadric();

  /* Set up Lighting */
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
  glLightfv(GL_LIGHT1, GL_POSITION, light_position2);

  /* Initial render mode is with full shading and LIGHT 0 enabled. */
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_ALPHA_TEST);

  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glShadeModel(GL_SMOOTH);
}

/* Game Setup (Discs and Pillars) */
void
b1n_gameSetup(int argc, char **argv)
{
  int i, j; /* Counters */
  int d;    /* Discs */

  /* Seeding the PRNG */
#ifdef _WIN32
  srand(time(NULL));
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  srand(tv.tv_usec);
#endif

  /* Checking if the user suggested discs quantity */
  if(argc>1){
    d = atoi(argv[1]);

    /* Checking if its within the valid range */
    if(d < b1n_DISCS_MIN || d > b1n_DISCS_MAX){
      b1n_usage(argv[0]);
    }
  }
  else {
    d = b1n_DISCS_DEFAULT;
  }

  /* Configuring Discs */
  for(i=0; i<b1n_DISCS_MAX; i++){
    g_discs[i].v = i;
    g_discs[i].r = b1n_RANDOM(255);
    g_discs[i].g = b1n_RANDOM(255);
    g_discs[i].b = b1n_RANDOM(255);
  }

  /* Configuring Pillars */
  for(i=0; i<b1n_PILLARS; i++){
    /* Discs Quantity */
    g_pillars[i].discs = 0;

    /* Position */
    g_pillars[i].x = i;
    g_pillars[i].y = 0;
    g_pillars[i].z = 0;

    /* Color */
    g_pillars[i].r = b1n_RANDOM2(64,128);
    g_pillars[i].g = b1n_RANDOM2(64,128);
    g_pillars[i].b = b1n_RANDOM2(128,255);
  }

  /* In the initial setup, all discs are at pillar 0 */
  for(i=d, j=0; i>0; i--, j++){
    /* Pushing disc in Pillar 0 */
    b1n_stackPush(0, i);
    g_pillars[0].d[j] = g_discs[i];
  }
}

/* Draw Pillars and Discs to the Screen */
void
b1n_gameDraw(void)
{
  int i, j;

  /* Clearing Buffer */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  glPushMatrix();
    /* Rotating the World */
    glRotatef(g_pos_x, 0, 1, 0);
    glRotatef(g_pos_y, 1, 0, 0);
    glTranslatef(-1, 0, 0);

    /* Drawing Pillars */
    for(i=0; i<b1n_PILLARS; i++){
      glPushMatrix();
        /* Rotating */
        glRotatef(90, 1, 0, 0);

        /* Color */
        glColor3ub(g_pillars[i].r, g_pillars[i].g, g_pillars[i].b);

        /* Translating to Position */
        glTranslatef(g_pillars[i].x, g_pillars[i].y, g_pillars[i].z);

        /* Drawing Cylinder */
        b1n_drawCylinder(b1n_PILLARS_RADIUS, 0, b1n_PILLARS_HEIGHT);

        /* Drawing Discs */
        for(j=0; j<g_pillars[i].discs; j++){
          /* Color */
          glColor3ub(g_pillars[i].d[j].r, g_pillars[i].d[j].g, g_pillars[i].d[j].b);
          glPushMatrix();
            /* Translating to Position */
            glTranslatef(0.0, 0.0, b1n_PILLARS_HEIGHT-j);

            /* Drawing Disc */
            glutSolidTorus(0.1, g_pillars[i].d[j].v*0.1, 20, 20);
          glPopMatrix();
        }
      glPopMatrix();
    }
    /*
    glColor3f(1, 0, 0);
    glBegin(GL_TRIANGLES);
      glVertex3f(1, 0, 0);
      glVertex3f(0, 1, 0);
      glVertex3f(1, 1, 0);
    glEnd();
    */
  glPopMatrix();

  glutSwapBuffers();
}

/* Exit Gracefully */
void
b1n_gameQuit(void)
{
  exit(b1n_SUCCESS);    /* Quitting */
}

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Stack */
/* Pop an item from the Stack */
int
b1n_stackPop(int i)
{
  /* Getting last item (to be popped) from the Stack */
  int last = g_pillars[i].d[g_pillars[i].discs].v;

  /* Decrease the item count */
  g_pillars[i].discs--;

  /* Return last */
  return last;
}

/* Push an item into the Stack */
void
b1n_stackPush(int i, int v)
{
  /* Increase item count */
  g_pillars[i].discs++;

  /* Set new item */
  g_pillars[i].d[g_pillars[i].discs].v = v;
}

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* CallBack */
void 
b1n_callbackReshape(GLint w, GLint h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(65.0, (GLfloat) w / (GLfloat) h, 1.0, 20.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -5.0);
  glScalef(1.5, 1.5, 1.5);
}

void
b1n_callbackKeyboard(unsigned char k, int x, int y)
{
  switch(k){
  case 27:
  case 'q':
  case 'Q':
    b1n_gameQuit();
    break;
  }
}

static void
b1n_callbackSpecialkey(int k, int x, int y)
{
  switch(k){
  case GLUT_KEY_LEFT:
    if ((g_pos_x -= 10) <= 0) g_pos_x = 360;
    break;
  case GLUT_KEY_RIGHT:
    if ((g_pos_x += 10) >= 360) g_pos_x = 0;
    break;
  case GLUT_KEY_UP:
    if ((g_pos_y += 10) >= 360) g_pos_y = 0;
    break;
  case GLUT_KEY_DOWN:
    if ((g_pos_y -= 10) <= 0) g_pos_y = 360;
    break;
  }
  glutPostRedisplay();
}

void
b1n_callbackMouse(void)
{
}

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Misc */
void
b1n_bail(int c, const char *m)
{
  fprintf(stderr, "%s\n", m);
  exit(c);
}

void
b1n_usage(const char *p)
{
  fprintf(stderr, 
    "Usage: %s [discs]"
    "\nDiscs Min:\t%02d"
    "\nDiscs Max:\t%02d"
    "\nDiscs Default:\t%02d\n", p,
    b1n_DISCS_MIN, b1n_DISCS_MAX, b1n_DISCS_DEFAULT); 

  exit(b1n_FAIL);
}
