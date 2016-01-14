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
   qw_compute.c contains all functions for the simulation of 
   quantum walks with a single particle starting at a particular vertex
   and for simulating grover search.
   ==================================================================== 
*/

/** 
   DegreeVec fills the vector d with the number of edges at each vertex. 
*/
void DegreeVec(VECINT *d, GRAPH *graph) {
  int i = 0;
  int j = 0;
  
  for (i = 0; i < (*graph).nodes; i++) {
    (*d)[i] = 0;
    for (j = 0; j < (*graph).nodes; j++)
      if ((*graph).adj[i][j] == 1) (*d)[i] += 1;
  }
}

/** 
   Normalisation returns the sum of the probabilities for each state. 
*/
double Normalisation(MATDBL x, int n) {
  int i = 0;
  int j = 0;
  double sum = 0.0;
  
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) sum += pow(x[i][j],2);
  return sum;
}

/** 
   InitialiseSingleVertex initialises the space to have a single particle
   located at qwparam.start vertex in an equal (real) superposition of all
   subnode states 
*/
void InitialiseSingleVertex(MATDBL *space, GRAPH *graph, QWPARAM *qwparam) {
  int i = 0;
  int j = 0;
  VECINT d = NULL;

  MallocVecInt(&d,(*graph).nodes);
  DegreeVec(&d,graph);

  for (i = 0; i < (*graph).nodes; i++) 
    for (j = 0; j < (*graph).nodes; j++) {
      if (i != (*qwparam).start || (*graph).adj[i][j] == 0)
	(*space)[i][j] = 0.0;
      else
	(*space)[i][j] = sqrt(1.0/d[(*qwparam).start]);
    }
  FreeVecInt(&d);
}

/** 
   InitialiseEqualSuperposition initialises the space to have equal 
   probability of being pound at each vertex. For an irregular graph 
   this is not an equal superposition of all vertex states. 
*/
void InitialiseEqualSuperposition(MATDBL *space, GRAPH *graph) {
  int i = 0;
  int j = 0;
  int subnodes = 0;
  double val = 0.0;

  for (i = 0; i < (*graph).nodes; i++) {
    subnodes = 0;
    for (j = 0; j < (*graph).nodes; j++) {
      if ((*graph).adj[i][j] == 1) subnodes += 1;
    }
    for (j = 0; j < (*graph).nodes; j++){
      if ((*graph).adj[i][j] == 0) 
	(*space)[i][j] = 0.0;
      else {
	val = (double)subnodes*(*graph).nodes;
      (*space)[i][j] = sqrt(1.0/val);
      }
    }
  }
}

/** 
   Grover fills coin (with pre-allocated memory) with a grover matrix 
   of size d. 
*/
void Grover(MATDBL *coin, int d) {
  int i;
  int j;

  for (i = 0; i < d; i++)
    for (j = 0; j < d; j++) {
      if (i != j) 
	(*coin)[i][j] = 2.0/(double)d;
      else
	(*coin)[i][j] = -1.0 + 2.0/(double)d;
    }
}

/** 
   NegativeIdentity fills coin (with pre-allocated memory) with a 
   -I matrix of size d. 
*/
void NegativeIdentity(MATDBL *coin, int d) {
  int i;
  int j;
  for (i = 0; i < d; i++)
    for (j = 0; j < d; j++) {
      if (i != j) 
	(*coin)[i][j] = 0.0;
      else
	(*coin)[i][j] = -1.0;
    }
}

/** 
   CoinOperation performs the unitary application of a grover coin 
   operator to each of the vertex states in the graph. If vertex = 
   qwparam.marked then the -I coin is used. 
*/
void CoinOperation(MATDBL *space, GRAPH *graph, QWPARAM *qwparam) {
  VECINT d;
  MATDBL coin;
  VECDBL v, vNew;
  int i, j, k, l;
  int take, replace;

  /** allocate and fill degree vector */
  MallocVecInt(&d,(*graph).nodes);
  DegreeVec(&d,graph);

  for (i = 0; i < (*graph).nodes; i++) {
    /** allocate vectors to hold the probability amplitudes. */
    MallocVecDbl(&v, d[i]);
    MallocVecDbl(&vNew, d[i]);
    /** allocate a matrix for the coin depending on the degree. */
    MallocMatDbl(&coin, d[i], d[i]);
    Grover(&coin,d[i]);

    /** collect non-zero prob amplitudes into a vector. */
    take = 0;
    for (j = 0; j < (*graph).nodes; j++) 
      if ((*graph).adj[i][j] == 1) {
	v[take] = (*space)[i][j];
	take += 1;
      }
    
    /** call the negativeIdentity operator for marked vertices. */
    if (i == (*qwparam).marked) 
      NegativeIdentity(&coin, d[i]);
    else
      Grover(&coin, d[i]);

    /** matrix multiplication. */
    for (k = 0; k < d[i]; k++) {
      vNew[k] = 0.0;
      for (l = 0; l < d[i]; l++) {
	vNew[k] += coin[k][l] * v[l];
      }
    }
    
    /** put the modified probability amplitudes back into the 
       space matrix. */
    replace = 0;
    for (j = 0; j < (*graph).nodes; j++)
      if ((*graph).adj[i][j] == 1) {
	(*space)[i][j] = vNew[replace];
	replace += 1;
      }
    /** Free the probability amplitude vectors and the coin matrix. */ 
    FreeMatDbl(&coin, d[i]);
    FreeVecDbl(&vNew);
    FreeVecDbl(&v);
  }
  FreeVecInt(&d);
}

/** 
   TranslationOperation performs the unitary shift or translation 
   operation of shifting probability amplitudes between connected 
   subnodes.
*/
void TranslationOperation(MATDBL *space, GRAPH *graph) {
  int i, j;
  MATDBL spaceOld;

  /** Allocate a space matrix to hold the incoming probability 
     amplitudes. */
  MallocMatDbl(&spaceOld,(*graph).nodes,(*graph).nodes);
  for (i = 0; i < (*graph).nodes; i++)
    for (j = 0; j < (*graph).nodes; j++) {
      spaceOld[i][j] = (*space)[i][j];
    }
  /** Perform the translation. */
  for (i = 0; i < (*graph).nodes; i++)
    for (j = 0; j < (*graph).nodes; j++) {
      if ((*graph).adj[i][j] == 1) (*space)[i][j] = spaceOld[j][i];
    }
  /** Free the space matrix. */
  FreeMatDbl(&spaceOld,(*graph).nodes);
}

/** 
   QuantumSearch performs a quantum-walk-based Grover search for 
   a single marked vertex on a graph. qwdata.prob is allocated here 
   but not freed. 
*/
void QuantumSearch(GRAPH *graph, QWDATA *qwdata, QWPARAM *qwparam) {
  int i = 0;
  int j = 0;
  int t = 0;
  MATDBL space = NULL;
  
  MallocMatDbl(&space,(*graph).nodes,(*graph).nodes);
  MallocQWprob(qwdata,graph);
  InitialiseEqualSuperposition(&space,graph);
  for (t = 0; t < (*qwdata).steps; t++) {
    for (i = 0; i < (*graph).nodes; i++) {
      (*qwdata).prob[i][t] = 0.0;
      for (j = 0; j < (*graph).nodes; j++)
	(*qwdata).prob[i][t] += pow(space[i][j],2);
    }
    CoinOperation(&space,graph,qwparam);
    TranslationOperation(&space,graph);
  }
  FreeMatDbl(&space,(*graph).nodes);
}

/** 
   QuantumWalk performs a quantum walk starting from a single vertex 
   with a Grover coin operator. qwdata.prob is allocated here 
   but not freed. 
*/
void QuantumWalk(GRAPH *graph, QWDATA *qwdata, QWPARAM *qwparam) {
  int i, j, t;
  MATDBL space = NULL;
  
  MallocMatDbl(&space,(*graph).nodes,(*graph).nodes);
  MallocQWprob(qwdata,graph);
  InitialiseSingleVertex(&space,graph,qwparam);
  for (t = 0; t < (*qwdata).steps; t++) {
    for (i = 0; i < (*graph).nodes; i++) {
      (*qwdata).prob[i][t] = 0.0;
      for (j = 0; j < (*graph).nodes; j++)
	(*qwdata).prob[i][t] += pow(space[i][j],2);
    }
    /** note final argument to coinOperation is the marked vertex
       but vertex n does not exist so when qwparam.marked = 
       graph.nodes, all vertices use the grover coin. */
    CoinOperation(&space,graph,qwparam); 
    TranslationOperation(&space,graph);
  }
  FreeMatDbl(&space,(*graph).nodes);
}

/** 
   ComputeProbabilities calls the relevant subprograms to read 
   the adjacency matrix and compute the quantum walk data for qwViz. 
   Returned value is from the rotines used to write the data file. 
*/
int ComputeProbabilities(GRAPH *graph, QWDATA *qwdata, QWPARAM *qwparam, QWFILE *qwfile)
{
  int err = 0;
  /** Read adjacency and call quantum walk routines */
  ReadAdjacency(qwfile, graph);
  if ((*qwparam).procedure == 'w') {
    if ((*qwparam).start >= (*graph).nodes || (*qwparam).start < 0) {
      fprintf(stderr,"ComputeProbabilities error: vertex %d does not exist.\n",(*qwparam).start+1);
      exit(-1);
    } else {
      QuantumWalk(graph,qwdata,qwparam);
    } 
  } else if ((*qwparam).procedure == 's') {
    if ((*qwparam).marked >= (*graph).nodes || (*qwparam).start < 0) {
      fprintf(stderr,"ComputeProbabilities error: vertex %d does not exist.\n",(*qwparam).marked+1);
      exit(-1);
    } else {   
      QuantumSearch(graph,qwdata,qwparam);
    }
  }
  /** write data to a file? */
  if ((*qwparam).write == TRUE) {
    if ((*qwfile).outtype == 'r')
      err += WriteRawData(graph,qwdata,qwfile);
    else if ((*qwfile).outtype == 'q')
      err += WriteQWML(graph,qwdata,qwfile);
  }
  if (err !=0) 
    fprintf(stderr,"ComputeProbabilities: Error writing data to file.");
  return(err);
}
