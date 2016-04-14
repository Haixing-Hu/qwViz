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

extern OPTIONS options;

/**
   Calls GraphViz routines to position the vertices of a graph structure
   in 2D, including the allocation of the coordinate lists
*/
void LayoutGraph(GRAPH *graph)
{
  GVC_t* gvc = NULL;		/** GraphViz Context pointer    */
  Agraph_t* g = NULL;		/** GraphViz graph pointer      */
  Agnode_t** nodeArray = NULL;	/** GraphViz node pointer array */
  Agedge_t* e = NULL;		/** GraphViz edge pointer       */

  int n = (*graph).nodes;
  int i = 0;
  int j = 0;
  int err = 0;
  char stringdata[256];
  char num[5];
  double xmin, xmax, ymin, ymax;

  if (options.debug == TRUE) {
    fprintf(stderr,"LayoutGraph: Data contains %d vertices.\n",n);
    fprintf(stderr,"LayoutGraph: Using %s algorithm.\n",(*graph).layoutalgorithm);
  }
  MallocCoordinateLists(graph);
  nodeArray = malloc( n * sizeof(Agnode_t *));
  if (nodeArray == NULL) {
    fprintf(stderr,"LayoutGraph: NodeArray memory allocation failed.");
  }
  /** Create a GraphViz context. */
  gvc = gvContext();

  /** Open a graph and add nodes and edges according
     to the adjacency matrix. */
  g = agopen("FromAdj",Agundirected, NULL);
  for (i = 0; i < n; i++) {
    sprintf(num,"%d",i);
    nodeArray[i] = agnode(g,num,TRUE);
  }
  for (i = 0; i < n; i++) {
    for (j = 0; j <= i; j++) {
      if ((*graph).adj[i][j] == 1) {
	e = agedge(g, nodeArray[i], nodeArray[j], NULL, TRUE);
      }
    }
  }

  /** Call the layout and rendering routines (GraphViz) */
  if (options.debug == TRUE) fprintf(stderr,"LayoutGraph: Calling \
graphViz - gvLayout...");
  gvLayout(gvc, g, (*graph).layoutalgorithm);
  if (options.debug == TRUE) fprintf(stderr,"done.\n");
  if (options.debug == TRUE) fprintf(stderr,"LayoutGraph: Calling \
graphViz - gvRender...");
  gvRender(gvc, g, "dot", NULL);
  if (options.debug == TRUE) fprintf(stderr,"done.\n");

  if (options.debug == TRUE)
    fprintf(stderr,"LayoutGraph: Extracting vertex coordinates from \
GraphViz...");
  for (i = 0; i < n; i++) {
    strcpy(stringdata,agget(nodeArray[i],"pos"));
    sscanf(stringdata,"%lf,%lf", &((*graph).Xcoord[i]), &((*graph).Ycoord[i]));
  }
  if (options.debug == TRUE) fprintf(stderr,"done.\n");

  /** Calculate the size of the bounding box and normalise data */
  strcpy(stringdata,agget(g,"bb"));
  sscanf(stringdata,"%lf,%lf,%lf,%lf", &xmin, &ymin, &xmax, &ymax);
  ScaleCoordinates(graph, xmax, ymax);
  ComputeNodeRadius(graph);

  /** Free the layout, close graph and free the context. */
  if (options.debug == TRUE)
    fprintf(stderr,"LayoutGraph: Closing GraphViz...");
  free(nodeArray);
  gvFreeLayout(gvc, g);
  agclose(g);
  err = gvFreeContext(gvc);
  if (options.debug == TRUE) {
    fprintf(stderr,"done.\n");
    fprintf(stderr,"LayoutGraph: Completed with %d errors.\n",err);
  }
}
/**
  ScaleCoordinates takes the bounding box of the graph calculated from GraphViz and scales the coordinates to fit in the box (-1,-1) -> (1,1)
*/
void ScaleCoordinates(GRAPH *graph, double xmax, double ymax)
{
  int i;
  double max;
  double avgX = 0.0;
  double avgY = 0.0;

  if (options.debug == TRUE) fprintf(stderr,"LayoutGraph: Scaling \
coordinates...");
  if (xmax < ymax) max = ymax;
  else max = xmax;

  for (i = 0; i < (*graph).nodes; i++) {
    (*graph).Xcoord[i] = 2.0*(*graph).Xcoord[i]/max - 1.0;
    (*graph).Ycoord[i] = 2.0*(*graph).Ycoord[i]/max - 1.0;
    avgX += (*graph).Xcoord[i];
    avgY += (*graph).Ycoord[i];
  }

  /** Reposition the centre of the graph at the origin */
  avgX /= (*graph).nodes;
  avgY /= (*graph).nodes;
  for (i = 0; i < (*graph).nodes; i++) {
    (*graph).Xcoord[i] += -avgX;
    (*graph).Ycoord[i] += -avgY;
  }
  if (options.debug == TRUE) fprintf(stderr,"done.\n");
}
/**
  As for ScaleCoordinates but when the coordinate positions come from a file.
*/
void ScaleCoordinatesFromFile(GRAPH *graph)
{
  int i;
  double max = 0.0;
  double min = 0.0;
  double avgX = 0.0;
  double avgY = 0.0;

  /** find min */
  if (options.debug == TRUE) fprintf(stderr,"ScaleCoordinatesFromFile: Scaling coordinates...");
  for (i = 0; i < (*graph).nodes; i++) {
    if (min > (*graph).Xcoord[i])
      min = (*graph).Xcoord[i];
    if (min > (*graph).Ycoord[i])
      min = (*graph).Ycoord[i];
  }

  /** make all values non-negative */
  for (i = 0; i < (*graph).nodes; i++) {
    (*graph).Xcoord[i] += fabs(min);
    (*graph).Ycoord[i] += fabs(min);
  }

  /** find max */
  for (i = 0; i < (*graph).nodes; i++) {
    if (max < (*graph).Xcoord[i])
      max = (*graph).Xcoord[i];
    if (max < (*graph).Ycoord[i])
      max = (*graph).Ycoord[i];
  }

  /** scale to the range -1 to 1 */
  for (i = 0; i < (*graph).nodes; i++) {
    (*graph).Xcoord[i] = 2.0*(*graph).Xcoord[i]/max - 1.0;
    (*graph).Ycoord[i] = 2.0*(*graph).Ycoord[i]/max - 1.0;
    avgX += (*graph).Xcoord[i];
    avgY += (*graph).Ycoord[i];
  }

  /** Reposition the average centre of the graph at the origin */
  avgX /= (*graph).nodes;
  avgY /= (*graph).nodes;
  for (i = 0; i < (*graph).nodes; i++) {
    (*graph).Xcoord[i] += -avgX;
    (*graph).Ycoord[i] += -avgY;
  }
  if (options.debug == TRUE) fprintf(stderr,"done.\n");
}
