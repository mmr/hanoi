/*
$Id: hanoi_ll.c,v 1.4 2006/02/28 20:57:26 mmr Exp $ 

<mmr@b1n.org>, 2004-05-31
Hanoi Tower Inocent Implementation
*/

/* Include */
#include <stdio.h>
#include <stdlib.h>

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
#define b1n_PILLARS   3
#define b1n_PILLARS_BASE  -1 /* Pillars' Base (ie. not a disc) */
#define b1n_PILLARS_RADIUS  0.1
#define b1n_PILLARS_HEIGHT  1.5

  /* Discs */
#define b1n_DISCS_MIN 2
#define b1n_DISCS_MAX 10 
#define b1n_DISCS_DEFAULT 2

  /* Misc */
#define b1n_TRUE      1
#define b1n_FALSE     0
#define b1n_FAIL      -1
#define b1n_SUCCESS   0

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Macros */
/* #define b1n_FIND_LAST(a) while(a) */
#define b1n_RANDOM2(min,max)  min+((int)((float)rand()/RAND_MAX*(max-min)))
#define b1n_RANDOM(max) b1n_RANDOM2(0, max)
#define b1n_RANDCOLOR   glColor3b(b1n_RANDOM(255), b1n_RANDOM(255), b1n_RANDOM(255));

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Types */
typedef struct b1n_stack {
  int value;
  struct b1n_stack *next;
} b1n_stack;

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Prototypes */
  /* Draw */
void  b1n_drawDisk(GLdouble, GLdouble); /* Draw a disc */
void  b1n_drawCylinder(GLdouble, GLdouble, GLdouble); /* Draw a cylinder */

  /* Game */
void  b1n_gameRenderSetup(void);    /* OpenGL Setup */
void  b1n_gameSetup(int, char **);  /* Pillars and Discs Setup */
void  b1n_gameDraw(void);       /* Draw pillars and discs */
void  b1n_gameDrawPillars(void);/* Draw pillars */
void  b1n_gameDrawDiscs(void);  /* Draw discs */
void  b1n_gameQuit(void);       /* Free memory and exit gracefully */
int   b1n_pillarsCreate(void);  /* Create Pillars (stacks) */
int   b1n_pillarsSetup(int);    /* Initial Pillars Setup  */
void  b1n_pillarsDestroy(void); /* Destroy Pillars (free memory) */

  /* CallBack */
void  b1n_callbackReshape(GLint, GLint);
void  b1n_callbackMouse(void);
void  b1n_callbackKeyboard(unsigned char, int, int);
static void b1n_callbackSpecialkey(int, int, int);

  /* Stack */
b1n_stack *b1n_stackCreate(void); /* Create a Stack */
b1n_stack *b1n_stackPush(b1n_stack *, int); /* Push an item into a Stack */
int b1n_stackPop(b1n_stack *);    /* Pop an item from a Stack */


  /* Misc */
void  b1n_usage(const char *);
void  b1n_bail(int, const char *);

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Global Vars */
  /* Pillars */
b1n_stack *g_pillars[b1n_PILLARS];
  /* Position in Space */
GLdouble g_pos_x = 0, g_pos_y = 90;
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

  /* Draws a disc */
void
b1n_drawDisk(GLdouble innerRadius, GLdouble outerRadius)
{
  glPushMatrix();
    gluDisk(g_qobj, innerRadius, outerRadius, 20, 1);
  glPopMatrix();
}

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Game */
void
b1n_gameDraw(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();
    glRotatef(g_pos_x, 0, 1, 0);
    glRotatef(g_pos_y, 1, 0, 0);
    glTranslatef(-1.0, 0.0, 0.0);

    b1n_gameDrawPillars();
    b1n_gameDrawDiscs();

    glutSwapBuffers();
  glPopMatrix();
}


void
b1n_gameDrawPillars(void)
{
  int i, j;
  GLdouble  dpos;
  b1n_stack *cur;

  glPushMatrix();
    for(i=0, j=b1n_PILLARS_HEIGHT; i<b1n_PILLARS; i++){
      glPushMatrix();

        /* Space between cylinders */
        glTranslatef(i, 0.0, 0.0);

        /* Drawing Cylinder */
        b1n_drawCylinder(b1n_PILLARS_RADIUS, 0.0, b1n_PILLARS_HEIGHT);

        /* Drawing Disks */
        glPushMatrix();
          b1n_RANDCOLOR;
          cur = g_pillars[i];
          dpos = 0;
          while(cur != NULL){
            if(cur->value != b1n_PILLARS_BASE){
             // b1n_RANDCOLOR;
              dpos += cur->value*0.01;
              glTranslatef(0.0, 0.0, 0.2);
              glutSolidTorus(0.1, cur->value*0.1, 20, 20);
              j++;
            }
            cur = cur->next;
          }
        glPopMatrix();
        glColor3f(1, 1, 1);
      glPopMatrix();
    }
  glPopMatrix();
}

void
b1n_gameDrawDiscs(void)
{
  //glutSolidTorus(0.05, 0.15, 20, 20);
}

/* Game Setup (Pillars and Discs) */
void
b1n_gameSetup(int argc, char **argv)
{
  int discs;

  /* Checking if the user suggested discs quantity */
  if(argc>1){
    discs = atoi(argv[1]);
    /* Checking if its within the valid range */
    if(discs < b1n_DISCS_MIN || discs > b1n_DISCS_MAX){
      b1n_usage(argv[0]);
    }
  }
  else {
    discs = b1n_DISCS_DEFAULT;
  }

  /* Creating pillars (stacks) */
  if(b1n_pillarsCreate()){
    /* Setting pillars up */
    if(!b1n_pillarsSetup(discs)){
      b1n_bail(b1n_FAIL, "Could not setup pillars.");
    }
  }
  else {
    b1n_bail(b1n_FAIL, "Could not create pillars.");
  }
}

/* OpenGL Setup (Background, Light, Projection) */
void
b1n_gameRenderSetup(void)
{
  GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
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

/* Free Memory and Quit gracefully */
void
b1n_gameQuit(void)
{
  b1n_pillarsDestroy(); /* Freeing memory */
  exit(b1n_SUCCESS);    /* Quitting */
}

/* Create Pillars (stacks) */
int
b1n_pillarsCreate(void)
{
  int i, ret = b1n_TRUE;

  for(i=0; i<b1n_PILLARS && ret; i++){
#ifdef DEBUG
    printf("Creating pillar %d\n", i);
#endif
    if((g_pillars[i] = b1n_stackCreate()) != NULL){
      g_pillars[i]->value = b1n_PILLARS_BASE;
      g_pillars[i]->next  = NULL;
      ret = b1n_TRUE;
    }
    else {
      ret = b1n_FALSE;
    }
  }

  return ret;
}

/* Initial Pillars Setup */
int
b1n_pillarsSetup(int d)
{
  int i;
  int ret = b1n_TRUE;

  /*
  The initial Setup is: 
  pillar 0 => d discs
  pillar 1 => 0 discs
  pillar 2 => 0 discs
  */

  /* Popullating pillar 0 from d to 0 */
  for(i=d; i>0 && ret; i--){
#ifdef DEBUG
    printf("Pushed Disc %d on pillar 0\n", i);
#endif
    g_pillars[0] = b1n_stackPush(g_pillars[0], i);
  }

  return ret;
}

/* Destroy Pillars (free memory) */
void
b1n_pillarsDestroy(void)
{
  int i;
  b1n_stack *cur, *next;

  /* Freeing memory for each one of the Pillars */
  for(i=0; i<b1n_PILLARS; i++){
#ifdef DEBUG
    printf("Destroying pillar %d\n", i);
#endif
    cur = g_pillars[i];

    /* Freeing memory for each disc on this Pillar */
    while(cur != NULL){
#ifdef DEBUG
      if(cur->value != b1n_PILLARS_BASE){
        printf("\tDestroying disc %d!\n", cur->value);
      }
#endif
      next = cur->next;
      free(cur);
      cur = next;
    }
  }
}

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Stack */
/* Create a Stack */
b1n_stack *
b1n_stackCreate(void)
{
  return (b1n_stack *) malloc(sizeof(b1n_stack));
}

/* Pop an item from a Stack */
int
b1n_stackPop(b1n_stack *stack)
{
  b1n_stack *next;
  int value = 0;
  
  if(stack != NULL){
    next  = stack->next; 
    value = stack->value;
    free(stack);
    stack = next;
  }
  return value;
}

/* Push an item from a Stack */
b1n_stack *
b1n_stackPush(b1n_stack *stack, int value)
{
  b1n_stack *new = (b1n_stack *) malloc(sizeof(b1n_stack));
  new->next  = stack;
  new->value = value;
  stack = new;
  return stack;
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
