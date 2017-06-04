/*
$Id: hanoi.c,v 1.8 2006/02/28 20:57:26 mmr Exp $ 

<mmr@b1n.org>, 2004-05-31
Hanoi Tower Inocent Implementation
*/

/* Include */
#include <stdio.h>
#include <stdlib.h>
#include "hanoi.h"

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Global Vars */
  /* World */
b1n_world g_world;

  /* Discs */
b1n_disc  g_discs[b1n_DISCS_MAX + 1];
int       g_discs_qt;

  /* Pillars */
b1n_pillar  g_pillars[b1n_PILLARS];

  /* Quadric Object */
GLUquadricObj *g_qobj;

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Main */
int
main(int argc, char **argv)
{
  /* Checking if the user suggested discs quantity */
  if(argc>1){
    g_discs_qt = atoi(argv[1]);

    /* Checking if its within the valid range */
    if(g_discs_qt < b1n_DISCS_MIN || g_discs_qt > b1n_DISCS_MAX){
      /* It is not, show usage */
      b1n_usage(argv[0]);
    }
  }
  else {
    /* The user didnt enter discs quantity, using Default */
    g_discs_qt = b1n_DISCS_DEFAULT;
  }

  /* Initializing Glut */
  glutInit(&argc, argv);
  glutInitWindowSize(b1n_WINDOW_X, b1n_WINDOW_X);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutCreateWindow("mmr's Hanoi");

  /* Setting the Game up (render, pillars and discs) */
  b1n_gameSetup();

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

  /* Draws a Donnut with glut Torus function */
void
b1n_drawDisc(int d)
{
  glutSolidTorus(b1n_DISCS_HEIGHT, g_discs[d].radius, 15, 15);
}

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Setup */
  /* OpenGL Setup (Background, Light, Projection) */
void
b1n_setupRender(void)
{
  GLfloat mat_specular[]  = {1.0, 1.0, 1.0, 1.0};
  GLfloat mat_shininess[] = {50.0};
  GLfloat light_position1[] = {1.0, 1.0, 1.0, 0.0};
  GLfloat light_position2[] = {-1.0, 1.0, 1.0, 0.0};

  /* Set up Lighting */
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
  glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  /* Quadric */
  g_qobj = gluNewQuadric();

  /* Color Material */
  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glShadeModel(GL_SMOOTH);

  /* We need Depth Test */
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);
}

void
b1n_setupWorld(void)
{
  /* Toggles */
  g_world.use_light = b1n_TRUE;
  g_world.use_blend = b1n_FALSE;
  g_world.use_texture = b1n_FALSE;
  g_world.use_background  = b1n_FALSE;
  g_world.use_fog = b1n_FALSE;

  /* Rotation */
  g_world.rot_x = g_world.rot_y = g_world.rot_z = 0;
}

void
b1n_setupDiscs(void)
{
  int i;  

  /* Configuring Discs */
  for(i=1; i<=g_discs_qt; i++){
    /* Radius */
    g_discs[i].radius = ((GLfloat) i*0.05);

    /* Color */
    g_discs[i].r = b1n_RANDOM(255);
    g_discs[i].g = b1n_RANDOM(255);
    g_discs[i].b = b1n_RANDOM(255);

    /* Creating Display List */
    glNewList(i, GL_COMPILE);
      b1n_drawDisc(i);
    glEndList();
  }
}

void
b1n_setupPillars(void)
{
  int i,j;

  /* Configuring Pillars */
  for(i=0; i<b1n_PILLARS; i++){
    /* Discs Quantity */
    g_pillars[i].discs = 0;

    /* Position */
    g_pillars[i].x = i-1;
    g_pillars[i].y = 0;
    g_pillars[i].z = -((GLfloat) b1n_PILLARS_HEIGHT/2);

    /* Color */
    g_pillars[i].r = b1n_RANDOM2(64,128);
    g_pillars[i].g = b1n_RANDOM2(64,128);
    g_pillars[i].b = b1n_RANDOM2(200,255);

    /* Cleaning all disks */
    for(j=0; j<=g_discs_qt; j++){
      g_pillars[i].d[j] = 0;
    }

    /* Creating Display List*/
    /*
     * FIXME
     *
    glNewList(g_discs_qt+i, GL_COMPILE);
      b1n_drawCylinder(b1n_PILLARS_RADIUS, 0, b1n_PILLARS_HEIGHT);
    glEndList();
    */
  }

  /* In the initial setup, all discs are at pillar 0 */
  for(i=g_discs_qt; i>0; i--){
    /* Pushing disc in Pillar 0 */
    b1n_stackPush(i, 0);
  }
}

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* Game */
void
b1n_gameSetup(void)
{
  int i;

  /* Seeding the PRNG for Random colors */
#ifdef _WIN32
  srand(time(NULL));
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  srand(tv.tv_usec);
#endif

  /* Setting everything up */
  b1n_setupRender();
  b1n_setupWorld();
  b1n_setupDiscs();
  b1n_setupPillars();
}

/* Draw Pillars and Discs to the Screen */
void
b1n_gameDraw(void)
{
  int d;  /* Disc */
  int i, j; /* Counters */
  GLfloat dpos; /* Disc pos */
  GLfloat fog_color[] = {.05,0,.05,0};

  /* Clearing Buffer */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Toggles */
    /* Light */
  if(g_world.use_light){
    glEnable(GL_LIGHTING);
  }
  else {
    if (glIsEnabled(GL_LIGHTING)){
      glDisable(GL_LIGHTING);
    }
  }

    /* Fog */
  if(g_world.use_fog){
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP);
    glFogfv(GL_FOG_COLOR, fog_color);
    glFogf(GL_FOG_DENSITY, .1);
  }
  else {
    if (glIsEnabled(GL_FOG)){
      glDisable(GL_FOG);
    }
  }

  /* Drawing */
  glPushMatrix();
    /* Moving to screen's center */
    glTranslatef(0, 0, 0);

    /* Rotating the World */
    glRotatef(g_world.rot_x, 0, 1, 0);
    glRotatef(g_world.rot_y, 1, 0, 0);


    /* Drawing Floor */
    /*
    glPushMatrix();
      glColor3f(1, 1, 1);
      glBegin(GL_QUADS);
        glVertex3f( 1.5, -1.05,  1.0);
        glVertex3f( 1.5, -1.0,  -2.0);
        glVertex3f(-1.5, -1.0,  -2.0);
        glVertex3f(-1.5, -1.05,  1.0);
      glEnd();
    glPopMatrix();
    */

    /* Drawing Pillars */
    for(i=0; i<b1n_PILLARS; i++){
      glPushMatrix();
        /* Rotating */
        glRotatef(90, 1, 0, 0);

        /* Translating to Position */
        glTranslatef(g_pillars[i].x, g_pillars[i].y, g_pillars[i].z);

        /* Drawing Cylinder */
        glColor3ub(g_pillars[i].r, g_pillars[i].g, g_pillars[i].b);
        b1n_drawCylinder(b1n_PILLARS_RADIUS, 0, b1n_PILLARS_HEIGHT);

        /* Drawing Discs */
          /* Positioning first disc correctly */
        dpos = b1n_PILLARS_HEIGHT -((GLfloat) g_pillars[i].discs * b1n_DISCS_SPACING) - 0.1;
        for(j=g_pillars[i].discs; j>=0; j--){
          d = g_pillars[i].d[j]; /* Saving Disc Number */

          glPushMatrix();
            glTranslatef(0.0, 0.0, dpos); /* Translating to Position */
            glColor3ub(g_discs[d].r, g_discs[d].g, g_discs[d].b); /* Color */
            glCallList(d); /* Drawing Disc */
          glPopMatrix();

          dpos += b1n_DISCS_SPACING;
        }
      glPopMatrix();
    }
  glPopMatrix();
  glutSwapBuffers();
}

/* Solve Game */
  /* Recursive function which moves all discs to the right pillar */
void
b1n_gameSolve(void)
{
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
  /* Decrease the item count */
  g_pillars[i].discs--;

  /* Return last */
  return g_pillars[i].d[g_pillars[i].discs];
}

/* Push an item into the Stack */
void
b1n_stackPush(int v, int i)
{
  /* Set new item */
  g_pillars[i].d[g_pillars[i].discs] = v;

  /* Increase item count */
  g_pillars[i].discs++;
}

/* =-=-=-==-==-==-==-==-==-==-==-==-==-==-=-= */
/* CallBack */
void 
b1n_callbackReshape(GLint w, GLint h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(35.0, (GLfloat) w / (GLfloat) h, 1.0, 20.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -5.0);
}

void
b1n_callbackKeyboard(unsigned char k, int x, int y)
{
  switch(k){
  case 'l':
    g_world.use_light = b1n_TRUE;
    break;
  case 'L':
    g_world.use_light = b1n_FALSE;
    break;
  case 'b':
    g_world.use_blend = b1n_TRUE;
    break;
  case 'B':
    g_world.use_blend = b1n_FALSE;
    break;
  case 'x':
    g_world.use_texture = b1n_TRUE;
    break;
  case 'X':
    g_world.use_texture = b1n_FALSE;
    break;
  case 'g':
    g_world.use_background = b1n_TRUE;
    break;
  case 'G':
    g_world.use_background = b1n_FALSE;
    break;
  case 's':
    b1n_gameSolve();
    break;
  case 'r':
    b1n_gameSetup();
    break;
  case 'f':
    g_world.use_fog = b1n_TRUE;
    break;
  case 'F':
    g_world.use_fog = b1n_FALSE;
    break;
  case '+':
    if(++g_discs_qt >= b1n_DISCS_MAX) g_discs_qt = b1n_DISCS_MAX;
    b1n_gameSetup();
    break;
  case '-':
    if(--g_discs_qt <= b1n_DISCS_MIN) g_discs_qt = b1n_DISCS_MIN;
    b1n_gameSetup();
    break;
  case 27:
  case 'q':
  case 'Q':
    b1n_gameQuit();
    break;
  }
  glutPostRedisplay();
}

static void
b1n_callbackSpecialkey(int k, int x, int y)
{
  switch(k){
  case GLUT_KEY_LEFT:
    if ((g_world.rot_x -= b1n_ROT_STEP) <= 0) g_world.rot_x = 360;
    break;
  case GLUT_KEY_RIGHT:
    if ((g_world.rot_x += b1n_ROT_STEP) >= 360) g_world.rot_x = 0;
    break;
  case GLUT_KEY_UP:
    if ((g_world.rot_y += b1n_ROT_STEP) >= 360) g_world.rot_y = 0;
    break;
  case GLUT_KEY_DOWN:
    if ((g_world.rot_y -= b1n_ROT_STEP) <= 0) g_world.rot_y = 360;
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
