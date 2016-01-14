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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gvc.h>
#include <ctype.h>
#if defined(__linux__)
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#warning "Not sure where to find glut.h"
#endif
#include "pauls.h"
#include "bitmap.h" 

/** Typedefs for all subprograms : */

typedef struct {
  XYZ vp;                /** View position            */
  XYZ vd;                /** View direction vector    */
  XYZ vu;                /** View up direction        */
  XYZ pr;                /** Point to rotate about    */
  double focallength;    /** Focal Length along vd    */
  double aperture;       /** Camera aperture          */
  double eyesep;         /** Eye separation           */
  double near,far;       /** Cutting plane distances  */
  int stereo;		 /** Are we in stereo mode    */
  int screenwidth;       /** Screen dimensions        */
  int screenheight;      
} CAMERA;

typedef struct {
  int debug;             /** Debug mode               */
  int record;            /** Movie recording mode     */
  int windowdump;        /** Image recording modes    */
  int exporttiff;        /** Export Tiff              */
  int fullscreen;        /** Game mode or not         */
  double targetfps;      /** Target frame rate        */
  int rendermode;        /** Shading type             */
  int showinfo;          /** Display model info       */
  int showhelp;          /** Display help info        */
  int showconstr;        /** Display construction     */
  int autorotate;        /** Spin automatically       */
  COLOUR bgcolour;       /** Background colour        */
  int texturetype;
  int showarrow;
  int labelledvertex;
  int subframes;
  int colourscheme;
} OPTIONS;

typedef struct {
  int button;
  int shift;
  int mouseh;
  int mousev;
  double framerate;      /** Rate at which the integer time steps pass  */
  int paused;            /** Is the movie playing or paused             */
  int currenttime;       /** The current integer timestep               */
  int currentsubframe;   /** The current integer sub-timestep           */
} INTERFACESTATE;

typedef struct {
  int nodes;
  int **adj;
  double *Xcoord;
  double *Ycoord;
  char* name;
  float noderadius;
  int graphvizlayout;
  char* layoutalgorithm; 
  int firstrender;
} GRAPH;

typedef struct {
  int compute;
  int steps;
  double **prob;
  char* comment;
  float maxprob;
  float scalemax;
} QWDATA;

typedef struct {
  char procedure;
  int marked;
  int start;
  int write;
} QWPARAM;

typedef struct {
  char* in;
  FILE* fpin;
  char* out;
  FILE* fpout;
  char intype;
  char outtype;
} QWFILE;

typedef int* VECINT;
typedef double* VECDBL;
typedef double** MATDBL;

#define NOSTEREO     0
#define ACTIVESTEREO 1
#define DUALSTEREO   2

#define SCALELENGTH 256
#define SCALEHEIGHT 16
#define SCALELABELHEIGHT 20
#define MAPPINGSIZE 256

/** Function prototypes for all subprograms : */

/** qwViz.c */
void SetDefaults(void);
void ParseCommandLine(int, char **);
void SetupWindow(int, char **);
void CreateWindowAndHandlers(void);
void CreateEnvironment(void);
void CreateLighting(void);
void HandleDisplay(void);
void HandleKeyboard(unsigned char,int,int);
void HandleSpecialKeyboard(int,int,int);
void HandleMouse(int,int,int,int);
void HandleReshape(int,int);
void HandleMouseMotion(int,int);
void HandlePassiveMotion(int,int);
void HandleVisibility(int);
void HandleIdle(void);
void RotateCamera(double,double,double,double);
void TranslateCamera(double,double,double);
void CameraHome(int);
void GiveUsage(char *);
void IncrementTime(int);
void IncrementVertexLabel(int);

/** misc.c */
void GiveUsage(char *);
void NodeUV(double,double,int,int,int);

/** menus.c */
void CreateMenus(void);
void HandleMainMenu(int);
void HandleCameraMenu(int);
void HandleOptionsMenu(int);
void HandleRenderMenu(int);
void HandleWindowMenu(int);

/** qw_graphlayout.c */
void LayoutGraph(GRAPH *);
void ScaleCoordinates(GRAPH *, double, double);
void ScaleCoordinatesFromFile(GRAPH *);

/** qw_malloc.c */
void MallocAdjacency(GRAPH *);
void FreeAdjacency(GRAPH *);
void MallocCoordinateLists(GRAPH *);
void FreeCoordinateLists(GRAPH *);
void MallocQWprob(QWDATA *, GRAPH *);
void FreeQWprob(QWDATA *, GRAPH *);
void MallocVecInt(VECINT *, int);
void FreeVecInt(VECINT *);
void MallocVecDbl(VECDBL *, int);
void FreeVecDbl(VECDBL *);
void MallocMatDbl(MATDBL *, int, int);
void FreeMatDbl(MATDBL *, int);

/** qw_readfiles.c */
int ReadQWML(QWFILE *, QWDATA *, GRAPH *);
int ParseLineXML(QWFILE *, char *, char *);
int StoreFilename(QWDATA *, char *);
int StoreGraphFamily(GRAPH *, char *);
int StoreGraphName(GRAPH *, char *);
int StoreComment(QWDATA *, char *);
int StoreAdjacency(GRAPH *, QWFILE *);
int StoreProb(QWDATA *, QWFILE *);
int StoreGraphLayout(GRAPH *, QWFILE *);
int ReadAdjacency(QWFILE *, GRAPH *);
int LinesToSkip(QWFILE *);
char ReadFilename(int , char **, QWFILE *);

/** qw_render.c */
void CreateGeometry(int, int, GRAPH *, QWDATA *);
void ComputeMaxProb(QWDATA *, GRAPH *);
void ComputeNodeRadius(GRAPH *);
void ComputeColourMap(void);
void DrawScale(QWDATA *, COLOUR *);
void DrawExtras(INTERFACESTATE, QWDATA *);
void CreateVertexArrow(GRAPH *);
void LabelVertex(void);

/** qw_compute.c */
void DegreeVec(VECINT *, GRAPH *);
double Normalisation(MATDBL, int );
void InitialiseSingleVertex(MATDBL *, GRAPH *, QWPARAM * );
void InitialiseEqualSuperposition(MATDBL *, GRAPH *);
void Grover(MATDBL *, int );
void NegativeIdentity(MATDBL *, int );
void CoinOperation(MATDBL *, GRAPH *, QWPARAM *);
void TranslationOperation(MATDBL *, GRAPH *);
void QuantumSearch(GRAPH *, QWDATA *, QWPARAM *);
void QuantumWalk(GRAPH *, QWDATA *, QWPARAM *);
char* Trim(char *);
int WriteRawData(GRAPH *, QWDATA *, QWFILE *); 
int WriteQWML(GRAPH *, QWDATA *, QWFILE *); 
int ComputeProbabilities(GRAPH *, QWDATA *, QWPARAM *, QWFILE *);

