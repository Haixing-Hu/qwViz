#include <stdio.h>
/** =================================================================
  WriteQWML for writing quantum walk probability distributions
  to qwml files compatible with qwViz.

  Developed by Scott D. Berry for release with qwViz 1.0
  Modified: 4.3.11
=====================================================================

  write_qwml requires: 
  filename: a character string with the filename, including extension.
  adjacency: an n x n integer array with the adjacency matrix.
  probdist: a t x n array of probabilities for each vertex in the graph.
  graphname: (non-essential) character string (pass "none" for unnamed graph).
  comment: (non-essential) character string (pass "none" for unnamed graph).
  vertices: integer with the number of vertices in the graph
  steps: integer with the number of steps in the walk
*/

int WriteQWML(char* filename, int** adjacency, double** probdist, char* graphName,
	      char* comment, int vertices, int steps) 
{
  int i, j, t;
  FILE* fptr = NULL;

  fptr = fopen(filename,"w");
  if (fptr == NULL) {
    fprintf(stderr,"WriteQWML: error opening output file");
    return(-1);
  }
  fprintf(fptr,"<?xml version=\"1.0\"?>\n");
  fprintf(fptr,"<qwml>\n");
  fprintf(fptr,"<adjacency>\n");
   for (i = 0; i < vertices; i++) {
    fprintf(fptr,"<row>\n");
    for (j = 0; j < vertices; j++)
      fprintf(fptr,"<col>%1d</col>\n",adjacency[i][j]);
    fprintf(fptr,"</row>\n");
  }
  fprintf(fptr,"</adjacency>\n");
  fprintf(fptr,"<probdist>\n");
  for (i = 0; i < vertices; i++) {
    fprintf(fptr,"<vertex>\n");
    for (t = 0; t < steps; t++)
      fprintf(fptr,"<prob>%10.8f</prob>\n",probdist[t][i]);
    fprintf(fptr,"</vertex>\n");
  }
  fprintf(fptr,"</probdist>\n");
  fprintf(fptr,"<filename>%s</filename>\n",filename);
  fprintf(fptr,"<comment>%s</comment>\n",comment);
  fprintf(fptr,"</qwml>\n");

  fclose(fptr);
  return(0);
}
