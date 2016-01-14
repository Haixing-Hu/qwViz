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
   WriteRawData creates a data file containing a t x n array of 
   probabilities computed during the program run.
*/
int WriteRawData(GRAPH *graph, QWDATA *qwdata, QWFILE *qwfile) 
{
  int t;
  int i;

  if (options.debug == TRUE)
    fprintf(stderr,"WriteRawData: Writing probability data to \
%s...",(*qwfile).out);
  
  (*qwfile).fpout = fopen((*qwfile).out,"w");
  if ((*qwfile).fpout == NULL) {
    fprintf(stderr,"WriteRawData: error opening output file");
    return(-1);
  }
  for (t = 0; t < (*qwdata).steps; t++) {
    for (i = 0; i < (*graph).nodes; i++) {
      fprintf((*qwfile).fpout,"%12.10f  ",(*qwdata).prob[i][t]);
    }
    fprintf((*qwfile).fpout,"\n");
  }
  fclose((*qwfile).fpout);

  if (options.debug == TRUE)
    fprintf(stderr,"done.\n");
  return(0);
}

/** 
   WriteQWML writes a .qwml output file containing the 
   probabilities computed during the program run.
*/
int WriteQWML(GRAPH *graph, QWDATA *qwdata, QWFILE *qwfile) 
{
  int i;
  int j;
  int t;
  
  if (options.debug == TRUE)
    fprintf(stderr,"WriteQWML: Writing probability data to \
%s...",(*qwfile).out);
  
  (*qwfile).fpout = fopen((*qwfile).out,"w");
  if ((*qwfile).fpout == NULL) {
    fprintf(stderr,"WriteQWML: error opening output file");
    return(-1);
  }
  fprintf((*qwfile).fpout,"<?xml version=\"1.0\"?>\n");
  fprintf((*qwfile).fpout,"<qwml>\n");
  fprintf((*qwfile).fpout,"<adjacency>\n");
  for (i = 0; i < (*graph).nodes; i++) {
    fprintf((*qwfile).fpout,"<row>\n");
    for (j = 0; j < (*graph).nodes; j++)
      fprintf((*qwfile).fpout,"<col>%1d</col>\n",(*graph).adj[i][j]);
    fprintf((*qwfile).fpout,"</row>\n");
  }
  fprintf((*qwfile).fpout,"</adjacency>\n");
  fprintf((*qwfile).fpout,"<probdist>\n");
  for (i = 0; i < (*graph).nodes; i++) {
    fprintf((*qwfile).fpout,"<vertex>\n");
    for (t = 0; t < (*qwdata).steps; t++)
      fprintf((*qwfile).fpout,"<prob>%10.8f</prob>\n",(*qwdata).prob[i][t]);
    fprintf((*qwfile).fpout,"</vertex>\n");
  }
  fprintf((*qwfile).fpout,"</probdist>\n");
  fprintf((*qwfile).fpout,"<filename>%s</filename>\n",Trim((*qwfile).out));
  fprintf((*qwfile).fpout,"<comment>computed_by_qwViz</comment>\n");
  fprintf((*qwfile).fpout,"</qwml>\n");

  fclose((*qwfile).fpout);
  if (options.debug == TRUE)
    fprintf(stderr,"done.\n");
  return(0);
}
/**
  Trim the whitespace from a string.
*/
char* Trim(char *str)
{
  char *end;
  while(isspace(*str)) str++;
  if(*str == 0)  /** All spaces? */
    return str;
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;
  *(end+1) = 0;
  return str;
}
