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
   qw_malloc.c contains the dynamic memory handling subprograms for qwViz.
   Failure to allocate memory results in program termination, exit(-1). 
*/

void MallocAdjacency(GRAPH *g)
{
  int i, n;
  n = (*g).nodes;
  (*g).adj = (int **)malloc(n * sizeof(int *));
  if ((*g).adj == NULL) {
    fprintf(stderr,"MallocAdjacency: Memory allocation failed.\n");
    exit(-1);
  }
  for (i = 0; i < n; i++) {
    (*g).adj[i] = (int *)malloc(n * sizeof(int));
    if ((*g).adj[i] == NULL) {
      fprintf(stderr,"MallocAdjacency: Memory allocation failed.\n");
      exit(-1);
    }
  }
}

void FreeAdjacency(GRAPH *g)
{
  int i, n;
  n = (*g).nodes;
  for (i = 0; i < n; i++) {
    free((*g).adj[i]);
    (*g).adj[i] = NULL;
  }
  free((*g).adj);
  (*g).adj = NULL;
}		

void MallocCoordinateLists(GRAPH *g)
{
  int n;
  n = (*g).nodes;
  if (( (*g).Xcoord = malloc(n * sizeof(double)) ) == NULL) {
    fprintf(stderr,"MallocCoordinateLists: Memory allocation failed.\n");
    exit(-1);
  }
  if (( (*g).Ycoord = malloc(n * sizeof(double)) ) == NULL) {
    fprintf(stderr,"MallocCoordinateLists: Memory allocation failed.\n");
    exit(-1);
  }
}

void FreeCoordinateLists(GRAPH *g)
{
  free((*g).Xcoord);
  (*g).Xcoord = NULL;
  free((*g).Ycoord);
  (*g).Ycoord = NULL;
}

void MallocQWprob(QWDATA *q, GRAPH *g)
{
  int i, n, t;
  t = (*q).steps;
  n = (*g).nodes;
  (*q).prob = malloc(n * sizeof(double *));
  if ((*q).prob == NULL) {
    fprintf(stderr,"MallocQWprob: Memory allocation failed.\n");
    exit(-1);
  }
  for (i = 0; i < n; i++) {
    (*q).prob[i] = malloc(t * sizeof(double));
    if ((*q).prob[i] == NULL) {
      fprintf(stderr,"MallocQWprob: Memory allocation failed.\n");
      exit(-1);
    }
  }
}
void FreeQWprob(QWDATA *q, GRAPH *g)
{
  int i, n, t;
  t = (*q).steps;
  n = (*g).nodes;
  for (i = 0; i < t; i++) {
    free((*q).prob[i]);
    (*q).prob[i] = NULL;
  }
  free((*q).prob);
  (*q).prob = NULL;
}

void MallocVecInt(VECINT *v, int len) {
  if (( *v = malloc(len * sizeof(int))) == NULL) {
    fprintf(stderr,"QW: MallocVecInt failed");
    exit(-1);
  }
}

void FreeVecInt(VECINT *v) {
  free(*v);
  *v = NULL;
}

void MallocVecDbl(VECDBL *v, int len) {
  if (( *v = malloc(len * sizeof(double))) == NULL) {
    fprintf(stderr,"QW: MallocVecInt failed");
    exit(-1);
  }
}

void FreeVecDbl(VECDBL *v) {
  free(*v);
  *v = NULL;
}

void MallocMatDbl(MATDBL *mat, int m, int n) {
  int i;
  if ( (*mat = malloc(m * sizeof(double *)) ) == NULL) {
    fprintf(stderr,"QW: MallocMatReal failed");
  }
  for (i = 0; i < m; i++) {
    if (( (*mat)[i] = malloc(n * sizeof(double)) ) == NULL) {
      fprintf(stderr,"QW: MallocMatReal failed");
    }
  }
}

void FreeMatDbl(MATDBL *mat, int m) {
  int i;
  for (i = 0; i < m; i++) {
    free((*mat)[i]);
    (*mat)[i] = NULL;
  }
  free(*mat);
  *mat = NULL;
}

