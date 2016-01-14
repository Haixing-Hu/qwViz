/*=======================================================================
   qwViz - OpenGL visualisation of quantum walks on graphs
  -----------------------------------------------------------------------
    Copyright (C) 2011 Scott D. Berry
    Contact: scottdberry 'at' gmail

    This file is part of qwViz.

    qwViz is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    qwViz is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with qwViz.  If not, see <http://www.gnu.org/licenses/>.
  ========================================================================*/

#include "qwViz.h"
#include "optionstring.h"
#include "interfacestring.h"

OPTIONS options;
CAMERA camera;
INTERFACESTATE interfacestate;
GRAPH graph;
QWDATA qwdata;
QWPARAM qwparam;
QWFILE qwfile;
XYZ themin = {-1,-1,-1}, themax = {1,1,1}, themid = {0,0,0};

int main(int argc,char **argv)
{
  SetDefaults();
  ParseCommandLine(argc,argv);

  /** Call appropriate routines to populate data structure. */
  if (qwdata.compute == TRUE) {
    if (options.debug)
      fprintf(stderr,".adj file entered...computing quantum walk data\n");
    if (ComputeProbabilities(&graph,&qwdata,&qwparam,&qwfile) != 0)
      fprintf(stderr,"main: Error reading .adj file\n");
  } else if (qwdata.compute == FALSE) {
    if (options.debug)
      fprintf(stderr,".qwml file entered...reading data file\n");
    if (ReadQWML(&qwfile, &qwdata, &graph) != 0)
      fprintf(stderr,"main: error reading .qwml file\n");
  }

  if (graph.graphvizlayout)
    LayoutGraph(&graph);
  else 
    ScaleCoordinatesFromFile(&graph);

  if (qwparam.marked != -1) {
    options.labelledvertex = qwparam.marked;
    options.showarrow = TRUE;
  }

  SetupWindow(argc,argv);
  CreateWindowAndHandlers();
  CreateEnvironment();

  /** Can we handle non power of 2 textures? */
#ifndef POWER2TEXTURE
#if GL_EXT_texture_rectangle
  options.texturetype = GL_TEXTURE_RECTANGLE_EXT;
#else
#if GL_NV_texture_rectangle
  options.texturetype = GL_TEXTURE_RECTANGLE_NV;
#else
  options.texturetype = GL_TEXTURE_2D;
#endif
#endif
#endif

  /** Ready to go! */
  CameraHome(0);
  RotateCamera(0.0,30.0,0.0,1.0);
  if (options.debug)
    fprintf(stderr,"Calling GLUT...\n");
  glutMainLoop();
  
  /** Free Adjacency and CoordinateLists */
  FreeAdjacency(&graph);
  FreeCoordinateLists(&graph);
  FreeQWprob(&qwdata,&graph);
  return(0);
}

/** Sets the default camera settings, options and interface settings */
void SetDefaults(void)
{
  COLOUR grey = {0.0,0.0,0.0}; /** Default background colour */

  /** The default camera attributes  */
  CameraHome(0);
  camera.stereo       = NOSTEREO;
  camera.screenwidth  = 800;
  camera.screenheight = 600;

  /** Default options  */
  options.debug        = FALSE;
  options.record       = FALSE;
  options.windowdump   = FALSE;
  options.exporttiff   = FALSE;
  options.fullscreen   = FALSE;
  options.targetfps    = 60;
  options.showinfo     = TRUE;
  options.showhelp     = FALSE;
  options.autorotate   = 0;
  options.bgcolour     = grey;
  options.texturetype  = GL_TEXTURE_2D;
  options.showarrow    = FALSE;
  options.labelledvertex  = 0;
  options.subframes    = 1;
  options.colourscheme = 1;

  /** State of the input device, mouse in this case  */
  interfacestate.button = -1;
  interfacestate.shift  = FALSE;
  interfacestate.mouseh = 0;
  interfacestate.mousev = 0;
  interfacestate.framerate = 10; 
  interfacestate.paused = FALSE; 
  interfacestate.currenttime = 0; 
  interfacestate.currentsubframe = 0;

  /** Graph initialisation */
  graph.nodes = 0;  
  graph.adj = NULL;
  graph.Xcoord = NULL;
  graph.Ycoord = NULL;
  graph.name = NULL;
  graph.noderadius = 0.0;
  graph.graphvizlayout = TRUE;
  graph.firstrender = TRUE;
  graph.layoutalgorithm = malloc(64*sizeof(char));
  strcpy(graph.layoutalgorithm,"neato");

  /** qwdata initialisation  */
  qwdata.steps = 200;      
  qwdata.prob = NULL;
  qwdata.comment = NULL;
  qwdata.maxprob = 0.01;
  qwdata.compute = FALSE;

  /** Default quantum walk parameters */
  qwparam.procedure = 'w'; 
  qwparam.marked = -1;      
  qwparam.start  = 0;      
  qwparam.write = FALSE;   
 
  /** qwfile initialisation */
  qwfile.in = NULL;
  qwfile.fpin = NULL;
  qwfile.out = NULL;
  qwfile.fpout = NULL;
  qwfile.intype = 'q';
  qwfile.outtype = 'r';
}

/** Command line parsing */
void ParseCommandLine(int argc, char **argv)
{
  int i;
  char *ext;
  char type[4];

  if (argc < 2) 
    GiveUsage(argv[0]);
  for (i=1;i<argc;i++) {
    if (strcmp(argv[i],"-h") == 0) 
      GiveUsage(argv[0]);
    if (strcmp(argv[i],"-d") == 0 || strcmp(argv[i],"-v") == 0)
      options.debug = TRUE;
    if (strcmp(argv[i],"-i") == 0)
      options.subframes = atoi(argv[i+1]);
    if (strcmp(argv[i],"-f") == 0)
      options.fullscreen = !options.fullscreen;
    if (strcmp(argv[i],"-s") == 0)
      camera.stereo = ACTIVESTEREO;
    if (strcmp(argv[i],"-ss") == 0) {
      camera.stereo = DUALSTEREO;
      camera.screenwidth *= 2;
    }
    if (strcmp(argv[i],"-a") == 0)
      options.autorotate = 1;
    if (strcmp(argv[i],"-bg") == 0) {
      options.bgcolour.r = atof(argv[i+1]);
      options.bgcolour.g = atof(argv[i+2]);
      options.bgcolour.b = atof(argv[i+3]);
    }
    if (strcmp(argv[i],"-cs") == 0) {
      options.colourscheme = atoi(argv[i+1]);
      if (options.colourscheme < 1 || options.colourscheme > 25)
	options.colourscheme = 1;
    }
    if (strcmp(argv[i],"-circo") == 0)
      strcpy(graph.layoutalgorithm,"circo");
    if (strcmp(argv[i],"-fdp") == 0)
      strcpy(graph.layoutalgorithm,"fdp");
    if (strcmp(argv[i],"-tiff") == 0)
      options.exporttiff = TRUE;
  }
  /** Read the filename and type from the command line. 
     If adjacency file is given then check command line for 
     quantum walk options */
  if ( (qwfile.intype = ReadFilename(argc,argv,&qwfile)) == 'a') {
    qwdata.compute = TRUE;
    for (i=1;i<argc-1;i++) {
      if (strcmp(argv[i],"-search") == 0) {
	qwparam.procedure = 's';
	qwparam.marked = atoi(argv[i+1])-1;
	if (qwparam.start != 0) {
	  fprintf(stderr,"qwViz error: options -start and \
-search are not compatible.\n");
	  exit(-1);
	}
      } else if (strcmp(argv[i],"-start") == 0) {
	qwparam.procedure = 'w';
	qwparam.start = atoi(argv[i+1])-1;
	if (qwparam.marked != -1) {
	  fprintf(stderr,"qwViz error: options -start and \
-search are not compatible.\n");
	  exit(-1);
	}
      } else if (strcmp(argv[i],"-steps") == 0) {
	qwdata.steps = atoi(argv[i+1]);
      } else if (strcmp(argv[i],"-o") == 0) {
	qwparam.write = TRUE;
	qwfile.out = argv[i+1];
	ext = strrchr(qwfile.out,'.');
	strcpy(type,ext+1);
	if (strcmp(type,"qwml") == 0)
	  qwfile.outtype = 'q';
	else if (strcmp(type,"prob") == 0)
	  qwfile.outtype = 'r';
      }
    }
  } else if ( qwfile.intype == 'q' ) {
    qwdata.compute = FALSE;
  } else {
    fprintf(stderr,"qwViz Error: Not a valid file input:\n");
    fprintf(stderr,"qwViz Error: Extension must be .qwml or .adj.\n");
    exit(-1);
  }
}


void SetupWindow(int argc, char **argv)
{
  glutInit(&argc,argv);
  if (camera.stereo == ACTIVESTEREO)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
  else
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
}

void CreateWindowAndHandlers(void)
{
  glutCreateWindow(qwfile.in);
  glutReshapeWindow(camera.screenwidth,camera.screenheight);
  if (options.fullscreen)
    glutFullScreen();
  glutDisplayFunc(HandleDisplay);
  glutReshapeFunc(HandleReshape);
  CreateMenus();
  glutKeyboardFunc(HandleKeyboard);
  glutSpecialFunc(HandleSpecialKeyboard);
  glutMouseFunc(HandleMouse);
  glutMotionFunc(HandleMouseMotion);
  glutPassiveMotionFunc(HandlePassiveMotion);
  glutVisibilityFunc(HandleVisibility);      
  if (camera.stereo == DUALSTEREO)
    glutSetCursor(GLUT_CURSOR_NONE);
  glutIdleFunc(HandleIdle);
}

/** 
   This is where global OpenGL settings are made. 
*/
void CreateEnvironment(void)
{
  /** Miscellaneous settings  */
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);

  /** These are hardware dependent */
  glDisable(GL_DITHER);
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_POINT_SMOOTH);
  glDisable(GL_POLYGON_SMOOTH);
  glHint(GL_POINT_SMOOTH_HINT,GL_FASTEST);
  glHint(GL_LINE_SMOOTH_HINT,GL_FASTEST);
  glHint(GL_POLYGON_SMOOTH_HINT,GL_FASTEST);
  glHint(GL_FOG_HINT,GL_FASTEST);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_FASTEST);

  /** Default geometry rendering  */
  glFrontFace(GL_CCW);
  glShadeModel(GL_SMOOTH);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

  /** Default meterials  */
  glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);

  /** Turn off all the lights  */
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHT1);
  glDisable(GL_LIGHT2);
  glDisable(GL_LIGHT3);
  glDisable(GL_LIGHT4);
  glDisable(GL_LIGHT5);
  glDisable(GL_LIGHT6);
  glDisable(GL_LIGHT7);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
  glClearColor(options.bgcolour.r,options.bgcolour.g,options.bgcolour.b,0.0);
}

void CreateLighting(void)
{
  GLfloat p[4] = {0,0,0,1};
  GLfloat black[4] = {0.0,0.0,0.0,0.0};
  GLfloat white[4] = {1.0,1.0,1.0,1.0};
  GLfloat midgrey[4] = {0.2,0.2,0.2,1.0};
  GLfloat noshin[1] = {30}; 

  /** Create two lights */
  p[0] = -10.0 ;
  p[1] = 10.0 ; 
  p[2] = 10.0 ; 
  glLightfv(GL_LIGHT0,GL_POSITION,p);
  glLightfv(GL_LIGHT0,GL_DIFFUSE,white);
  glLightfv(GL_LIGHT0,GL_SPECULAR,white);
  glLightfv(GL_LIGHT0,GL_AMBIENT,black);
  glEnable(GL_LIGHT0);

  p[0] = 10.0 ;
  p[1] = -10. ;
  p[2] = 5.0 ; 
  glLightfv(GL_LIGHT1,GL_POSITION,p);
  glLightfv(GL_LIGHT1,GL_DIFFUSE,white);
  glLightfv(GL_LIGHT1,GL_SPECULAR,white);
  glLightfv(GL_LIGHT1,GL_AMBIENT,black);
  glEnable(GL_LIGHT1);

  /** The global shading settings */
  glShadeModel(GL_FLAT);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); /** Filled rather than wireframe */
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT,midgrey);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHTING); /** Turn on light calculations */

  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,noshin);
}

/**
   This is the basic display callback routine
	Deals with
	- single display
	- side by side stereo (-ss)
	- active frame sequential stereo (-s)
*/
void HandleDisplay(void)
{
  XYZ r;
  double ratio,radians,wd2,ndfl;
  double left,right,top,bottom;

  if (options.record)
    options.targetfps = 30;
  else
    options.targetfps = 60;

  /** Misc stuff needed for the frustum  */
  ratio   = camera.screenwidth / (double)camera.screenheight;
  if (camera.stereo == DUALSTEREO)
    ratio /= 2;
  radians = DTOR * camera.aperture / 2;
  wd2     = camera.near * tan(radians);
  ndfl    = camera.near / camera.focallength;
  top     =   wd2;
  bottom  = - wd2;

  /** Determine the right eye vector  */
  CROSSPROD(camera.vd,camera.vu,r);
  Normalise(&r);
  r.x *= camera.eyesep / 2.0;
  r.y *= camera.eyesep / 2.0;
  r.z *= camera.eyesep / 2.0;

  if (camera.stereo == ACTIVESTEREO || camera.stereo == DUALSTEREO) {

    if (camera.stereo == DUALSTEREO) {
      glDrawBuffer(GL_BACK);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    left  = - ratio * wd2 - 0.5 * camera.eyesep * ndfl;
    right =   ratio * wd2 - 0.5 * camera.eyesep * ndfl;
    glFrustum(left,right,bottom,top,camera.near,camera.far);
    if (camera.stereo == DUALSTEREO)
      glViewport(camera.screenwidth/2,0,camera.screenwidth/2,camera.screenheight);
    else
      glViewport(0,0,camera.screenwidth,camera.screenheight);
    glMatrixMode(GL_MODELVIEW);
    glDrawBuffer(GL_BACK_RIGHT);
    if (camera.stereo == ACTIVESTEREO)
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(camera.vp.x + r.x,camera.vp.y + r.y,camera.vp.z + r.z,
	      camera.vp.x + r.x + camera.vd.x,
	      camera.vp.y + r.y + camera.vd.y,
	      camera.vp.z + r.z + camera.vd.z,
	      camera.vu.x,camera.vu.y,camera.vu.z);
    CreateGeometry(interfacestate.currenttime, interfacestate.currentsubframe, &graph, &qwdata);
    DrawExtras(interfacestate, &qwdata);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    left  = - ratio * wd2 + 0.5 * camera.eyesep * ndfl;
    right =   ratio * wd2 + 0.5 * camera.eyesep * ndfl;
    glFrustum(left,right,bottom,top,camera.near,camera.far);
    if (camera.stereo == DUALSTEREO)
      glViewport(0,0,camera.screenwidth/2,camera.screenheight);
    else
      glViewport(0,0,camera.screenwidth,camera.screenheight);
    glMatrixMode(GL_MODELVIEW);
    glDrawBuffer(GL_BACK_LEFT);
    if (camera.stereo == ACTIVESTEREO)
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(camera.vp.x - r.x,camera.vp.y - r.y,camera.vp.z - r.z,
	      camera.vp.x - r.x + camera.vd.x,
	      camera.vp.y - r.y + camera.vd.y,
	      camera.vp.z - r.z + camera.vd.z,
	      camera.vu.x,camera.vu.y,camera.vu.z);
    CreateGeometry(interfacestate.currenttime, interfacestate.currentsubframe, &graph, &qwdata);
    DrawExtras(interfacestate, &qwdata);

  } else {

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0,0,camera.screenwidth,camera.screenheight); 
    left  = - ratio * wd2;
    right =   ratio * wd2;
    glFrustum(left,right,bottom,top,camera.near,camera.far);
    glMatrixMode(GL_MODELVIEW);
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(camera.vp.x,camera.vp.y,camera.vp.z,
	      camera.vp.x + camera.vd.x,
	      camera.vp.y + camera.vd.y,
	      camera.vp.z + camera.vd.z,
	      camera.vu.x,camera.vu.y,camera.vu.z);
    CreateGeometry(interfacestate.currenttime, interfacestate.currentsubframe, &graph, &qwdata);
    DrawExtras(interfacestate, &qwdata);
  }

  /** Dump the window in compressed TGA format  */
  if (options.record || options.windowdump) {
    if (options.exporttiff) {
      if (camera.stereo == ACTIVESTEREO)
	WindowDump("",camera.screenwidth,camera.screenheight,TRUE,-5);
      else
	WindowDump("",camera.screenwidth,camera.screenheight,FALSE,-5);
      options.windowdump = FALSE;
    } else {
      if (camera.stereo == ACTIVESTEREO)
	WindowDump("",camera.screenwidth,camera.screenheight,TRUE,12);
      else
	WindowDump("",camera.screenwidth,camera.screenheight,FALSE,12);
      options.windowdump = FALSE;
    }
  }

  /** Swap buffers  */
  glutSwapBuffers();

  /** Autorotate */
  if (options.autorotate != 0) {
    RotateCamera(1.0,0.0,0.0,options.autorotate/50.0);
  }

}

/**
   Deal with plain key strokes
*/
void HandleKeyboard(unsigned char key, int x, int y)
{
  interfacestate.mouseh = x;
  interfacestate.mousev = y;

  if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
    interfacestate.shift = TRUE;
  else
    interfacestate.shift = FALSE;

  /** Handle keyboard commands */
  switch (key) {
  case ESC: 		
  case 'Q':
  case 'q': 
    exit(0);
    break;
  case 'a':
    options.autorotate--;
    break;
  case 'A':
    options.autorotate++;
    break;
  case 'z':
  case 'Z':
    options.autorotate = 0;
    break;
  case 'i':
  case 'I':
    options.showinfo = !options.showinfo;
    break;
  case 'h':
  case 'H':
    options.showhelp = !options.showhelp;
    break;
  case '[':
    RotateCamera(0.0,0.0,-1.0,1.0);
    break;
  case ']':
    RotateCamera(0.0,0.0,1.0,1.0);
    break;
  case 'w':
  case 'W':
    options.windowdump = TRUE;
    break;
  case 'r':
  case 'R':
    options.record = !options.record;
    break;
  case '<':
  case ',':
    TranslateCamera(0.0,0.0,-camera.focallength/50);
    break;
  case '>':
  case '.':
    TranslateCamera(0.0,0.0,+camera.focallength/50);
    break;
  case '-':
  case '_':
    if (camera.aperture < 90)
      camera.aperture *= 1.025;
    break;
  case '+':
  case '=':
    if (camera.aperture > 5)
      camera.aperture /= 1.025;
    break;
  case '(':
  case '9':
    camera.focallength *= 1.05;
    break;
  case ')':
  case '0':
    camera.focallength /= 1.05;
    break;
  case 'p':
  case 'P':
    interfacestate.paused = !interfacestate.paused;
    break;
  case 'b':
  case 'B':
    interfacestate.currentsubframe = 0;
    IncrementTime(-1*options.subframes);
    break;
  case 'n':
  case 'N':
    interfacestate.currentsubframe = 0;
    IncrementTime(1*options.subframes);
    break;
  case 'u':
  case 'U':
    qwdata.scalemax *= 0.95;
    break;
  case 'd':
  case 'D':
    qwdata.scalemax *= 1.05;
    break;
  case 's':
  case 'S':
    if (interfacestate.framerate > 0)
      interfacestate.framerate--;
    break;
  case 'f':
  case 'F':
    if (interfacestate.framerate < 200)
      interfacestate.framerate++;
    break;
  case '/':
  case '?':
    interfacestate.currenttime = 0;
    interfacestate.currentsubframe = 0;
    break;
  case 'c':
  case 'C':
    graph.noderadius *=0.95;
    break;
  case 'v':
  case 'V':
    graph.noderadius *=1.05;
    break;
  case 'j':
  case 'J':
    options.showarrow = !options.showarrow;
    break;
  case 'k':
  case 'K':
    IncrementVertexLabel(-1);
    break;
  case 'l':
  case 'L':
    IncrementVertexLabel(1);
    break;
  case 'x':
  case 'X':
    CameraHome(0);
    RotateCamera(0.0,30.0,0.0,1.0);
    break;
  }
}

/**
   Deal with special key strokes
*/
void HandleSpecialKeyboard(int key, int x, int y)
{
  interfacestate.mouseh = x;
  interfacestate.mousev = y;

  if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
    interfacestate.shift = TRUE;
  else
    interfacestate.shift = FALSE;

  switch (key) {
  case GLUT_KEY_LEFT:  
    if (interfacestate.shift)
      TranslateCamera(-camera.focallength/200,0.0,0.0);
    else
      RotateCamera(-1.0,0.0,0.0,1.0); 
    break;
  case GLUT_KEY_RIGHT: 
    if (interfacestate.shift)
      TranslateCamera(camera.focallength/200,0.0,0.0);
    else
      RotateCamera(1.0,0.0,0.0,1.0);  
    break;
  case GLUT_KEY_UP:    
    if (interfacestate.shift)
      TranslateCamera(0.0,camera.focallength/200,0.0);
    else
      RotateCamera(0.0,1.0,0.0,1.0);  
    break;
  case GLUT_KEY_DOWN:  
    if (interfacestate.shift)
      TranslateCamera(0.0,-camera.focallength/200,0.0);
    else
      RotateCamera(0.0,-1.0,0.0,1.0); 
    break;
  case GLUT_KEY_F1:
    CameraHome(1);
    break;
  case GLUT_KEY_F2:
    CameraHome(2);
    break;
  case GLUT_KEY_F3:
    CameraHome(3);
    break;
  case GLUT_KEY_F4:
    CameraHome(4);
    break;
  case GLUT_KEY_F5:
    CameraHome(5);
    break;
  case GLUT_KEY_F6:
    CameraHome(6);
    break;
  }
}

/**
   Rotate (ix,iy) or roll (iz) the camera about the focal point
   ix,iy,iz are flags, 0 do nothing, +- 1 rotates in opposite directions
   Correctly updating all camera attributes
*/
void RotateCamera(double ix,double iy,double iz,double dtheta)
{
  XYZ vp,vu,vd,vr;
  XYZ newvp,newr;
  double radius,dd,radians;
  double dx,dy,dz;

  vu = camera.vu;
  vp = camera.vp;
  vd = camera.vd;
  CROSSPROD(vd,vu,vr);
  Normalise(&vr);
  radians = dtheta * PI / 180.0;

  /** Handle the roll  */
  if (ABS(iz) > 0.00001) {
    camera.vu = ArbitraryRotate(vu,iz*radians,vd);
    return;
  }
 
  /** Distance from the rotate point  */
  dx = camera.vp.x - camera.pr.x;
  dy = camera.vp.y - camera.pr.y;
  dz = camera.vp.z - camera.pr.z;
  radius = sqrt(dx*dx + dy*dy + dz*dz);
	 
  /** Determine the new view point  */
  dd = radius * radians;
  newvp.x = vp.x + dd * ix * vr.x + dd * iy * vu.x - camera.pr.x;
  newvp.y = vp.y + dd * ix * vr.y + dd * iy * vu.y - camera.pr.y;
  newvp.z = vp.z + dd * ix * vr.z + dd * iy * vu.z - camera.pr.z;
  Normalise(&newvp);
  camera.vp.x = camera.pr.x + radius * newvp.x;
  camera.vp.y = camera.pr.y + radius * newvp.y;
  camera.vp.z = camera.pr.z + radius * newvp.z;
 	
  /** Determine the new right vector  */
  newr.x = camera.vp.x + vr.x - camera.pr.x;
  newr.y = camera.vp.y + vr.y - camera.pr.y;
  newr.z = camera.vp.z + vr.z - camera.pr.z;
  Normalise(&newr);
  newr.x = camera.pr.x + radius * newr.x - camera.vp.x;
  newr.y = camera.pr.y + radius * newr.y - camera.vp.y;
  newr.z = camera.pr.z + radius * newr.z - camera.vp.z;
  Normalise(&newr);
 	
  camera.vd.x = camera.pr.x - camera.vp.x;
  camera.vd.y = camera.pr.y - camera.vp.y;
  camera.vd.z = camera.pr.z - camera.vp.z;
 	
  /** Determine the new up vector  */
  CROSSPROD(newr,camera.vd,camera.vu);

  Normalise(&camera.vu);
  Normalise(&camera.vd);
}

/**
   Translate (pan) the camera view point
*/
void TranslateCamera(double ix,double iy,double iz)
{
  XYZ vr;

  CROSSPROD(camera.vd,camera.vu,vr);
  Normalise(&vr);

  camera.vp.x += iy*camera.vu.x + ix*vr.x + iz*camera.vd.x;
  camera.vp.y += iy*camera.vu.y + ix*vr.y + iz*camera.vd.y;
  camera.vp.z += iy*camera.vu.z + ix*vr.z + iz*camera.vd.z;

  camera.pr.x += iy*camera.vu.x + ix*vr.x + iz*camera.vd.x;
  camera.pr.y += iy*camera.vu.y + ix*vr.y + iz*camera.vd.y;
  camera.pr.z += iy*camera.vu.z + ix*vr.z + iz*camera.vd.z;
}

/**
  Handle mouse events.
  Do nothing special, just remember the state.
*/
void HandleMouse(int button,int state,int x,int y)
{
  if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
    interfacestate.shift = TRUE;
  else
    interfacestate.shift = FALSE;

  if (state == GLUT_DOWN) {
    if (button == GLUT_LEFT_BUTTON) {
      interfacestate.button = GLUT_LEFT_BUTTON;
    } else if (button == GLUT_MIDDLE_BUTTON) {
      interfacestate.button = GLUT_MIDDLE_BUTTON;
    } else {
      /** Assigned to menus */
    }
  }
  interfacestate.mouseh = x;
  interfacestate.mousev = y;
}

/**
   Handle mouse motion
*/
void HandleMouseMotion(int x,int y)
{
  double dx,dy;

  dx = x - interfacestate.mouseh;
  dy = y - interfacestate.mousev;

  if (interfacestate.button == GLUT_LEFT_BUTTON) {
    if (interfacestate.shift)
      TranslateCamera(-camera.focallength*dx/200,camera.focallength*dy/200,0.0);
    else
      RotateCamera(-dx,dy,0.0,0.5);
  } else if (interfacestate.button == GLUT_MIDDLE_BUTTON) {
    if (interfacestate.shift)
      TranslateCamera(0.0,0.0,camera.focallength*dy/200);
    else
      RotateCamera(0.0,0.0,dx,0.5);
  }

  interfacestate.mouseh = x;
  interfacestate.mousev = y;
}

/**
  What to do on moviing nouse, the button isn't down.
*/
void HandlePassiveMotion(int x,int y)
{
  interfacestate.mouseh = x;
  interfacestate.mousev = y;
}

/**
  How to handle visibility
  If the window is not visible then don't draw it,
  allows layered windows without a processor load blowout.
*/
void HandleVisibility(int visible)
{
  if (visible == GLUT_VISIBLE)
    glutIdleFunc(HandleIdle);
  else
    glutIdleFunc(NULL);
}

/**
  What to do on an idle event.
  Maintain a constant frame rate.
*/
void HandleIdle(void)
{
  static double tstart = -1;
  static double movietstart = -1;
  double tstop;
  double movietstop;

  if (tstart < 0) {
    tstart = GetRunTime();
    movietstart = tstart;
  }
  tstop = GetRunTime();
  movietstop = tstop;

  if (tstop - tstart > 1.0/options.targetfps) {
    glutPostRedisplay();
    tstart = tstop;
  }

  if (!interfacestate.paused && 
      movietstop - movietstart > 1.0/(interfacestate.framerate*options.subframes)) {
    IncrementTime(1);
    movietstart = movietstop;
  }
}

/**
   Handle a window reshape/resize
*/
void HandleReshape(int w,int h)
{
  if (options.debug)
    fprintf(stderr,"HandleReshape: Reshape to %d x %d\n",w,h);

  camera.screenwidth = w;
  camera.screenheight = h;
}

/**
  Move the camera to the home position 
*/
void CameraHome(int position)
{
  XYZ up = {0.0,0.0,1.0},x = {1.0,0.0,0.0};

  camera.aperture = 60;
  camera.focallength = 0.75*VectorLength(themin,themax);
  camera.eyesep = camera.focallength / 25;
  camera.near = camera.focallength / 30;
  camera.far = camera.focallength * 10;

  options.autorotate = 0;

  if (options.debug)
    fprintf(stderr,"Resetting camera\n");

  switch (position) {
  case 0:
    camera.vp.x = themid.x - camera.focallength;
    camera.vp.y = themid.y; 
    camera.vp.z = themid.z;
    camera.vd.x = 1;
    camera.vd.y = 0;
    camera.vd.z = 0;
    camera.vu = up;
    break;
  case 1: /** Front */
    camera.vp.x = themid.x - camera.focallength;
    camera.vp.y = themid.y;
    camera.vp.z = themid.z;
    camera.vd.x = 1;
    camera.vd.y = 0;
    camera.vd.z = 0;
    camera.vu = up;
    break;
  case 2: /** Back */
    camera.vp.x = themid.x + camera.focallength;
    camera.vp.y = themid.y;
    camera.vp.z = themid.z;
    camera.vd.x = -1;
    camera.vd.y = 0;
    camera.vd.z = 0;
    camera.vu = up;
    break;
  case 3: /** Left */
    camera.vp.x = themid.x;
    camera.vp.y = themid.y + camera.focallength;
    camera.vp.z = themid.z;
    camera.vd.x = 0;
    camera.vd.y = -1;
    camera.vd.z = 0;
    camera.vu = up;
    break;
  case 4: /** Right */
    camera.vp.x = themid.x;
    camera.vp.y = themid.y - camera.focallength;
    camera.vp.z = themid.z;
    camera.vd.x = 0;
    camera.vd.y = 1;
    camera.vd.z = 0;
    camera.vu = up;
    break;
  case 5: /** Top */
    camera.vp.x = themid.x;
    camera.vp.y = themid.y;
    camera.vp.z = themid.z + camera.focallength;
    camera.vd.x = 0;
    camera.vd.y = 0;
    camera.vd.z = -1;
    camera.vu = x;
    break;
  case 6: /** Bottom */
    camera.vp.x = themid.x;
    camera.vp.y = themid.y; 
    camera.vp.z = themid.z - camera.focallength;
    camera.vd.x = 0;
    camera.vd.y = 0;
    camera.vd.z = 1;
    camera.vu = x;
    break;
  }

  camera.pr = themid;
  Normalise(&camera.vd);
  Normalise(&camera.vu);
}

void IncrementTime(int step)
{
  interfacestate.currentsubframe += step;
  if (interfacestate.currentsubframe < 0) {
    interfacestate.currentsubframe += options.subframes;
    interfacestate.currenttime -= 1;
  }
  if (interfacestate.currentsubframe >= options.subframes) {
    interfacestate.currentsubframe -= options.subframes;
    interfacestate.currenttime += 1;
  }
  if (interfacestate.currenttime < 0)
    interfacestate.currenttime += qwdata.steps;
  if (interfacestate.currenttime >= qwdata.steps)
    interfacestate.currenttime -= qwdata.steps;
}

void IncrementVertexLabel(int step)
{
  options.labelledvertex += step;
  if (options.labelledvertex < 0)
    options.labelledvertex += graph.nodes;
  if (options.labelledvertex >= graph.nodes)
    options.labelledvertex -= graph.nodes;
}
