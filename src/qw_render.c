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

/** 
   qw_render.c contains all functions for drawing objects to the buffer.
   =====================================================================
*/

extern CAMERA camera;
extern OPTIONS options;
extern char *interfacestring;

COLOUR colourmap[MAPPINGSIZE];

/** 
   CreateGeometry takes an integer time step and plots the graph surface 
   and quantum walk data on that surface for the corresponding time. 
   Uses library calls to paulslib and opengllib.
*/
void CreateGeometry(int t, int subt, GRAPH *graph, QWDATA *qwdata)
{
  int i, j;
  XYZ node, top, up = {0,0,1};
  COLOUR c;
  float scaleFactor;

  scaleFactor = (float)subt/(float)options.subframes;

  CreateLighting();
  if ((*graph).firstrender == TRUE) {
    glLineWidth(1.0);
    glPointSize(1.0);
    ComputeNodeRadius(graph);
    ComputeMaxProb(qwdata,graph);
    (*qwdata).scalemax = (*qwdata).maxprob;
  }

  /** Draw the graph with single pixel lines. */
  for (i = 0; i < (*graph).nodes; i++) {
    for (j = 0; j < i; j++){
      if ((*graph).adj[i][j] == 1) {
	glBegin(GL_LINES);
	glColor3f(1.0,1.0,1.0);
	glVertex3f((*graph).Xcoord[i],(*graph).Ycoord[i],0.0);
	glVertex3f((*graph).Xcoord[j],(*graph).Ycoord[j],0.0);
	glEnd();
      }
    }
  }
  
  /** Draw the probabilities as cylinders in the z-direction. */
  for (i = 0; i < (*graph).nodes; i++) {
    node.x = (*graph).Xcoord[i];
    node.y = (*graph).Ycoord[i];
    node.z = 0.0;
    top.x = (*graph).Xcoord[i];
    top.y = (*graph).Ycoord[i];
    top.z = ((*qwdata).prob[i][t]*(1.0 - scaleFactor)
	     + (*qwdata).prob[i][t+1]*scaleFactor)/(*qwdata).scalemax;
    c = GetColour(top.z,0.0,(*qwdata).scalemax,options.colourscheme);
    glColor3f(c.r,c.g,c.b);
    CreateCone(node,top,(*graph).noderadius,(*graph).noderadius,40,0.0,TWOPI);
    CreateDisk(top,up,0.0,(*graph).noderadius,40,0.0,TWOPI);
  }
  if (options.showarrow) {
    CreateVertexArrow(graph);
  }

  if ((*graph).firstrender == TRUE)
    (*graph).firstrender = FALSE;
}

/**
  Compute the radius of the cylinders to represent the quantum walk
  data. (max = 0.05, min = 0.015).
*/
void ComputeNodeRadius(GRAPH *graph)
{
  int i, j;
  float minSepSq;
  float sepSq;

  minSepSq = 1.0;
  for (i = 0; i < (*graph).nodes; i++)
    for (j = 0; j < (*graph).nodes; j++) {
      if (i != j) {
	sepSq = pow((*graph).Xcoord[i] - (*graph).Xcoord[j],2) 
	  + pow((*graph).Ycoord[i] - (*graph).Ycoord[j],2);
	if (sepSq < minSepSq) minSepSq = sepSq;
      }
    }
  (*graph).noderadius = sqrt(minSepSq)/4;
  if ((*graph).noderadius < 0.015)
    (*graph).noderadius = 0.015;
  if ((*graph).noderadius > 0.05)
    (*graph).noderadius = 0.05;
}

/**
  ComputeMaxProb stores the maximum probablity in qwdata.maxprob for 
  initially setting the scale.
*/
void ComputeMaxProb(QWDATA *qwdata, GRAPH *graph)
{
  int i, t;
  float max;
  max = 0.0;
  for (i = 0; i < (*graph).nodes; i++)
    for (t = 0; t < (*qwdata).steps; t++) {
      if ((*qwdata).prob[i][t] > max) max = (*qwdata).prob[i][t];
    }
  (*qwdata).maxprob = max;
}

/**
  ComputeColourMap uses GetColour (paulslib) to compute the colours 
  for the scale.
*/
void ComputeColourMap(void)
{
  int i = 0;
  for (i = 0; i < MAPPINGSIZE; i++) {
    colourmap[i] = GetColour((double)i,0.0,(double)MAPPINGSIZE,options.colourscheme);
  }
}

/**
   Write the time and the framerate on the screen and draw the scale.
   If required print the help text on the screen.
*/
void DrawExtras(INTERFACESTATE interfacestate, QWDATA *qwdata)
{
  char s[64];

  if (options.showarrow) {
    LabelVertex();
  }

  if (interfacestate.currenttime == 0)
    ComputeColourMap();
  
  if (options.showinfo) {
    glDisable(GL_LIGHTING);
    glColor3f(1.0,1.0,1.0);
    sprintf(s,"Frame rate: %.1f fps",interfacestate.framerate);
    DrawGLText(10,10,s);
    sprintf(s,"t = %d",interfacestate.currenttime);
    DrawGLText(10,25,s);
    glDisable(GL_DEPTH_TEST);
    DrawScale(qwdata,colourmap);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
  }

  if (options.showhelp) {
    glDisable(GL_LIGHTING);
    glColor3f(1.0,1.0,1.0);
    DrawGLText(10,camera.screenheight-20,interfacestring);
    glEnable(GL_LIGHTING);
  }

}
/**
  DrawScale draws the scale on the buffer.
*/
void DrawScale(QWDATA *qwdata, COLOUR *colourmap)
{
  int i;
  int j;
  int offset = 10;
  int vborder = 6;
  int hborder = 32;
  int top;
  int bottom;
  int left;
  int right;
  int sw;
  char *p = NULL;
  char s[10]; 

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  if (camera.stereo == DUALSTEREO) {
    sw = camera.screenwidth/2;
    glOrtho(0,sw,0,camera.screenheight,-1,1);
  } else {
    sw = camera.screenwidth;
    glOrtho(0,camera.screenwidth,0,camera.screenheight,-1,1);
  }
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

  /** Transparent patch */
  bottom = offset;
  top = offset + 2*vborder + SCALEHEIGHT + SCALELABELHEIGHT;
  left = sw - offset - 2*hborder - SCALELENGTH;
  right = sw - offset;

  glColor4f(1.0,1.0,1.0,0.2);
  glBegin(GL_QUADS);
  glVertex3i(left, bottom, 0);
  glVertex3i(left, top, 0);
  glVertex3i(right, top, 0);
  glVertex3i(right, bottom, 0);
  glEnd();

  /** Colour ramp */
  glBegin(GL_LINES);
  for (i=0;i<SCALELENGTH;i++) {
    j = i * (MAPPINGSIZE/SCALELENGTH);
    glColor3f(colourmap[j].r,colourmap[j].g,colourmap[j].b);
    glVertex3i(left + hborder + i, bottom + vborder + SCALELABELHEIGHT + SCALEHEIGHT,0);
    glVertex3i(left + hborder + i,  bottom + vborder + SCALELABELHEIGHT,0);
  }
  glEnd();

  /** Ticks */
  glColor3f(0.0,0.0,0.0);
  glBegin(GL_LINES);
  for (i=0;i<=50;i++) {
    if (i % 5 == 0) {
      glVertex3i(left + hborder + i * SCALELENGTH / 50.0,bottom + SCALELABELHEIGHT+ vborder,0);
      glVertex3i(left + hborder + i * SCALELENGTH / 50.0,bottom + SCALELABELHEIGHT,0);
    } else {
      glVertex3i(left + hborder + i * SCALELENGTH / 50.0,bottom + SCALELABELHEIGHT + vborder,0);
      glVertex3i(left + hborder + i * SCALELENGTH / 50.0,bottom + SCALELABELHEIGHT + vborder/2,0);
    }
  }
  glEnd();

  /** Scale Labels */
  glColor3f(1.0,1.0,1.0);
  sprintf(s,"%2.1f",0.0);
  glRasterPos2i(left + hborder/2 + 4, bottom + vborder);
  for (p=s;*p;p++) {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13,*p);
  }
  sprintf(s,"%2.1E",(*qwdata).scalemax);
  glRasterPos2i(right - vborder - 7*8, bottom + vborder);
  for (p=s;*p;p++) {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13,*p);
  }

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

}
/** Create a 3D arrow under vertex. */
void CreateVertexArrow(GRAPH *graph)
{
  XYZ top, head, bottom, up = {0,0,1};
  float offset = 0.02;
  float headlength = 0.06;
  float stemlength = 0.12;
  float stemradius = 0.02;
  float headradius = 0.04;

  CreateLighting();
  top.x = (*graph).Xcoord[options.labelledvertex];
  top.y = (*graph).Ycoord[options.labelledvertex];
  top.z = -offset;
  head.x = top.x;
  head.y = top.y;
  head.z = top.z - headlength;
  bottom.x = top.x;
  bottom.y = top.y;
  bottom.z = top.z - headlength - stemlength;
  glColor3f(1.0,1.0,1.0);
  CreateCone(head,top,headradius,0.0,40,0.0,TWOPI);
  CreateDisk(head,up,0.0,headradius,40,0.0,TWOPI);  
  CreateCone(bottom,head,stemradius,stemradius,20,0.0,TWOPI);
  CreateDisk(bottom,up,0.0,stemradius,40,0.0,TWOPI);  
}

void LabelVertex(void)
{
  char s[64];
  glDisable(GL_LIGHTING);
  glColor3f(1.0,1.0,1.0);
  sprintf(s,"Vertex %d",options.labelledvertex+1);
  DrawGLText(10,40,s);
  glEnable(GL_LIGHTING);
}
