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
   ReadQWML is the function to be called when accessing data 
   from a QWML file. The data structures qwdata and graph will be 
   populated from the file given by fname. Set options.debug = TRUE 
   (run with -d option) to see what is happening during the program 
   run.
*/

#define PARSE_OK 0
#define EMPTYLINE_WARNING 1
#define PARSE_ERROR 2

int ReadQWML(QWFILE *qwfile, QWDATA *qwdata, GRAPH *graph)
{
  char tag[256] = "";
  char data[256] = "";
  int err = 0, parseVal = 0;

  fprintf(stderr,"Reading qwml file...\n");
  if (((*qwfile).fpin = fopen((*qwfile).in,"r")) == NULL) {
    fprintf(stderr,"ReadQWML Error: File not found\n");
    exit(-1);
  }
  /** Parse XML one line at a time. Call a storage function depending 
      on the tag. */
  while (err != -1 && parseVal != PARSE_ERROR) {
    if ((parseVal = ParseLineXML(qwfile,tag,data)) == PARSE_OK) {
      if (strcmp(tag,"?xml") == 0) {
	if (options.debug) 
	  fprintf(stderr,"ReadQWML: Found \"?xml\" tag.\n");

      } else if (strcmp(tag,"qwml") == 0) {
	if (options.debug) 
	  fprintf(stderr,"ReadQWML: Found \"qwml\" tag.\n");
      
      } else if (strcmp(tag,"adjacency") == 0) {
	if (options.debug) 
	  fprintf(stderr,"ReadQWML: Found \"adjacency\" tag.\n");
	err += StoreAdjacency(graph,qwfile);

      } else if (strcmp(tag,"probdist") == 0) {
	if (options.debug) 
	  fprintf(stderr,"ReadQWML: Found \"probdist\" tag.\n");
	err += StoreProb(qwdata,qwfile);

      } else if (strcmp(tag,"graphlayout") == 0) {
	if (options.debug) 
	  fprintf(stderr,"ReadQWML: Found \"graphlayout\" tag.\n");
	err += StoreGraphLayout(graph,qwfile);

      } else if (strcmp(tag,"/qwml") == 0) {
	if (err != 0) {
	  fprintf(stderr,"ReadQWML warning: not all data storage successful.\n");
	  return(-1);
	} else {
	  if (options.debug) 
	    fprintf(stderr,"ReadQWML: Found \"/qwml\" tag.\n");
	  fprintf(stderr,"Reading qwml file...done\n");
	  return(0);
	}
	/** If tag is not recognised */
      } else {
	if (options.debug)
	  fprintf(stderr,"ReadQWML warning: Tag \"%s\" in qwml file was not recognised.\n", tag);
      }
	/** If there is no start character, print a warning and continue. */
    } else if (parseVal == EMPTYLINE_WARNING) {
      if (options.debug) {
	fprintf(stderr,"ReadQWML warning: Empty line in qwml file.\n");
      }
    }
  }

	/** If function has not returned correctly, analyse errors and print a warning. */
  if (parseVal == PARSE_ERROR) {
    fprintf(stderr,"ReadQWML warning: .qwml file not properly terminated.\n");
    return(err);
  } else if (err == -1) {
    fprintf(stderr,"ReadQWML Error: not all data storage successful...Fatal\n");
    exit(-1);
  } else if (err == 1) {
    fprintf(stderr,"ReadQWML Error: not all data storage successful...\n");
    return(-1);
  } else {
    fprintf(stderr,"ReadQWML: Unknown Error. err = %d, parseVal = %d\n", err, parseVal);
    return(0);
  }
}

/** 
   ParseLineXML parsesone line of XML, returning the tag <"tag"> and 
   the data enclosed in >"data"<. Returns 0 for successful excecution, 
   -1 for fatal Error, 1 for missing tag on line and 2 for EOF
*/
int ParseLineXML(QWFILE *qwfile, char *tag, char *data)
{
  char tmp[256] = "";
  char line[256] = "";
  char nextChar = '\0';
  char* startChar = NULL;
  unsigned int tagPos = 0;
  unsigned int dataPos = 0;
  unsigned int pos = 0;
  int i = 0;
  int readdata = FALSE;

  if (fgets(line,256,(*qwfile).fpin) == NULL) {
    fprintf(stderr,"ParseLineXML: End of file.\n");
    return(PARSE_ERROR);
  }

  /** locate start character '<' */
  startChar = strchr(line,'<');
  if ( startChar == NULL ) {
	  if (options.debug) {
    fprintf(stderr,"ParseLineXML: No start-tag character, '<' found.\n");
	  }
	return(EMPTYLINE_WARNING);
  } else {
    pos = (startChar - line + 1);
  }
  /** record the tag to a buffer */
  while ((nextChar = line[pos]) != '>') {
    if (!isspace(nextChar)) {
    tmp[tagPos] = nextChar;
    tagPos++;
    }
    pos++;
  }
  /** copy tmp buffer to tag and blank remaining characters */
  strncpy(tag, tmp, (tagPos)*sizeof(char));
  if (strlen(tag) > tagPos) {
    for (i = tagPos; i <= (int)strlen(tag); ++i)
      tag[i] = '\0';
  }
  
  pos++; /** skip the '>' character and record the data to a buffer */
  while ((nextChar = line[pos]) != '<' && !isspace(nextChar) ) {
    readdata = 1;
    tmp[dataPos] = nextChar;
    dataPos++;
    pos++;
  }

  if (readdata == 1) {
    /** copy the tmp buffer to *data and blank remaining characters */
    strncpy(data, tmp, dataPos*sizeof(char));
    if (strlen(data) > dataPos) {
      for (i = dataPos; i <= (int)strlen(data); ++i)
	data[i] = '\0';
    }
  } else {
    strcpy(data,"");
  }
  return(PARSE_OK);
}

/** 
   StoreAdjacency reads an adjacency matrix from a qwml file. The size of 
   the first row is used to determine the number of vertices in the graph.
   graph.adj is allocated here (n x n).
*/
int StoreAdjacency(GRAPH *graph, QWFILE *qwfile)
{
  char tag[256] = "adjacency";
  char data[256] = "";
  int row = 0;
  int col = 0;
  int *tmp = NULL;
  int i, parseVal = 0;

  if (options.debug) fprintf(stderr,"StoreAdjacency: Reading adjacency matrix.\n");

  while (strcmp(tag,"/adjacency") != 0 && parseVal != PARSE_ERROR) {
    if ((parseVal = ParseLineXML(qwfile,tag,data)) == PARSE_ERROR) {
      fprintf(stderr,"StoreAdjacency: Error adjacency matrix format is invalid.\n");
      return(-1);
    } else if (strcmp(tag,"row") == 0) {
      while (strcmp(tag,"/row") != 0 && parseVal != PARSE_ERROR) {
	if ((parseVal = ParseLineXML(qwfile,tag,data)) == PARSE_ERROR) {
	  fprintf(stderr,"StoreAdjacency: Error adjacency matrix format is invalid.\n");
	  return(-1);
	} else if (strcmp(tag,"col") == 0) {
	  if (row == 0) {
	    (*graph).nodes = col+1; 
	    tmp = realloc(tmp, (col+1) * sizeof(int));
	    if (tmp == NULL) {
	      fprintf(stderr,"StoreAdjacency: Memory allocation failed.\n");
	      return(-1);
	    } 
	    tmp[col] = atoi(data);
	  } else {
	    if (row == 1 && col == 0) {
	      MallocAdjacency(graph);
	      for (i = 0; i < (*graph).nodes; i++) 
		(*graph).adj[0][i] = tmp[i];
	      free(tmp);
	    }
	    ((*graph).adj)[row][col] = atoi(data);
	  }
	  col++;
	}
      }
      col = 0;
    }
    row++;
  }
  if (options.debug) 
  fprintf(stderr,"StoreAdjacency: Found \"/adjacency\" tag.\n");
  return(0);
}
/**
  StoreProb reads the probability distribution from a file. The length
  of the distribution for the first vertex is taken as the number of 
  steps in the walk. qwdata.prob is allocated here.
*/
int StoreProb(QWDATA *qwdata, QWFILE *qwfile)
{
  char tag[256] = "probdist";
  char data[256] = "";
  int vertex = 0;
  int time = 0;
  double *tmp = NULL;
  int t, parseVal = 0;

  if (options.debug) fprintf(stderr,"StoreProb: Reading probability distribution.\n");

  while (strcmp(tag,"/probdist") != 0 && parseVal != PARSE_ERROR) {
    if ((parseVal = ParseLineXML(qwfile,tag,data)) == PARSE_ERROR) {
      fprintf(stderr,"StoreProb Error: probdist format is invalid.\n");
      return(-1);
    } else if (strcmp(tag,"vertex") == 0) {
      while (strcmp(tag,"/vertex") != 0 && parseVal != PARSE_ERROR) {
	if ((parseVal = ParseLineXML(qwfile,tag,data)) == PARSE_ERROR) {
	  fprintf(stderr,"StoreProb Error: probdist format is invalid.\n");
	  return(-1);
	} else if (strcmp(tag,"prob") == 0) {
	  (*qwdata).steps = time+1; 
	  tmp = (double *)realloc(tmp, (time+1) * sizeof(double));
	  if (tmp == NULL) {
	    fprintf(stderr,"StoreProb: Memory allocation failed.\n");
	    return(-1);
	  } 
	  tmp[time] = strtod(data,NULL);
	}
	time++;
      }
      (*qwdata).prob = (double **)realloc((*qwdata).prob, (vertex + 1) * sizeof(double *));
      ((*qwdata).prob)[vertex] = (double *)malloc( (time + 1) * sizeof(double)); 
      for (t = 0; t < (*qwdata).steps; t++)
	((*qwdata).prob)[vertex][t] = tmp[t];
      time = 0;
    }
    vertex++;
  }
  free(tmp);
  if (options.debug) 
    fprintf(stderr,"StoreProb: Found \"/probdist\" tag.\n");
  return(0);
}
/**
  StoreGraphLayout reads the coordinates of the vertices. 
  qwdata.Xcoord and qwdata.Ycoord are allocated here. 
  graph.graphvizlayout is set to FALSE here so that coordinate 
  lists will not be reallocated/calculated.
*/
int StoreGraphLayout(GRAPH *graph, QWFILE *qwfile)
{
  char tag[256] = "graphlayout";
  char data[256] = "";
  double *tmpX = NULL;
  double *tmpY = NULL;
  int vertices = 0;
  int i, parseVal = 0;

  if (options.debug) fprintf(stderr,"StoreGraphLayout: Reading graphlayout from file.\n");

  while (strcmp(tag,"/graphlayout") != 0 && parseVal != PARSE_ERROR) {
    if ((parseVal = ParseLineXML(qwfile,tag,data)) == PARSE_ERROR) {
      fprintf(stderr,"StoreGraphLayout Error: graphlayout format is invalid.\n");
      fprintf(stderr,"StoreGraphLayout: Graphviz will be called to layout the vertices.\n");
      return(1);

    } else if (strcmp(tag,"vertex") == 0) {
      while (strcmp(tag,"/vertex") != 0 && parseVal != PARSE_ERROR) {
	if ((parseVal = ParseLineXML(qwfile,tag,data)) != PARSE_ERROR) {
	  if (strcmp(tag,"xcoord") == 0) {
	    tmpX = realloc(tmpX,(vertices+1)*sizeof(double));
	    if (tmpX == NULL) {
	      fprintf(stderr,"StoreGraphLayout: Memory allocation failed.\n");
	      return(-1);
	    }
	    tmpX[vertices] = strtod(data,NULL);
	  }
	  if (strcmp(tag,"ycoord") == 0) {
	    tmpY = realloc(tmpY,(vertices+1)*sizeof(double));
	    if (tmpY == NULL) {
	      fprintf(stderr,"StoreGraphLayout: Memory allocation failed.\n");
	      return(-1);
	    } 
	    tmpY[vertices] = strtod(data,NULL);
	  }
	}
      }
      vertices++;
    }
  }
  if (options.debug) 
    fprintf(stderr,"StoreGraphLayout: Found \"/graphlayout\" tag.\n");

  /** If adjacency is not yet read. */
  if ((*graph).nodes == 0) {
    (*graph).nodes = vertices; 
  }

  /** If the number of vertices now agrees then set the logical
      variable graph.graphvizlayout = FALSE and store the coordinate
      lists in the graph struct. */
  if ((*graph).nodes != vertices) {
    fprintf(stderr,"StoreGraphLayout: Error there are %d vertices in graph layout and %d in adjacency matrix.\n",vertices,(*graph).nodes);
    fprintf(stderr,"StoreGraphLayout: Graphviz will be called to layout the vertices.\n");
    (*graph).graphvizlayout = TRUE;
  } else {
    (*graph).graphvizlayout = FALSE;
    MallocCoordinateLists(graph);
    for (i = 0; i < (*graph).nodes; i++) {
      (*graph).Xcoord[i] = tmpX[i];
      (*graph).Ycoord[i] = tmpY[i];
    }
  }
  free(tmpX);
  free(tmpY);
  
  if (options.debug)
    fprintf(stderr,"StoreGraphLayout: Read graphlayout from .qwml file.\n");
  if ((*graph).graphvizlayout == FALSE)
    return(0);
  else
    return(1);
}

/** 
   ReadAdjacency reads an adjacency matrix from a .adj file and 
   allocates and fills graph.adj in the data structure 
*/
int ReadAdjacency(QWFILE *qwfile, GRAPH *graph) 
{
  char line[256];
  int col = 0;
  int row = 0;
  char curCh;
  int i;
  int* topRow = NULL;
  int skip = 0;

  skip = LinesToSkip(qwfile);
  /** open the adjacency file */
  if (((*qwfile).fpin = fopen((*qwfile).in,"r")) == NULL) {
    fprintf(stderr,"ReadAdjacency Error: File not found\n");
    exit(-1);
  }
  /** skip the first (skip) lines */
  for (i = 0; i < skip; i++) {
    fgets(line,sizeof(line),(*qwfile).fpin);
  }
  topRow = malloc(sizeof(int));
  while ((curCh = fgetc((*qwfile).fpin)) != EOF)
    {
      if (curCh == '0' || curCh == '1') {
	if (row == 0) {
	  /** store the top row of the matrix in a growing vector */
	  topRow = realloc(topRow,(col+1)*sizeof(int));
	  topRow[col] = curCh - '0';
	  (*graph).nodes = col+1;
	} else if (row == 1 && col == 0) {
	  /** allocate the adjacency and copy the top row of the matrix */
	  MallocAdjacency(graph);
	    for (i = 0; i < (*graph).nodes; i ++)
	      (*graph).adj[0][i] = topRow[i];
	}
	if (row > 0) {
	  (*graph).adj[row][col] = curCh - '0';
	}
	col++;
      } else if (curCh == '\n') {
	col = 0;
	row++;
      }
    }
  free(topRow);
  fclose((*qwfile).fpin);
  return(0);
}
/**
  LinesToSkip calculates the number of blank or comment lines 
  before the adjacency matrix starts.
*/
int LinesToSkip(QWFILE *qwfile)
{
  int curCh;
  int countLn = 0;
  int countCh = 0;

  if (((*qwfile).fpin = fopen((*qwfile).in,"r")) == NULL) {
    fprintf(stderr,"ReadAdjacency Error: File not found\n");
    exit(-1);
  }
  while ((curCh = fgetc((*qwfile).fpin)) != EOF) { 
    if ((countCh == 0) && (curCh == '0' || curCh == '1'))
      break;
    else if (curCh != '\n')
      countCh++;
    else {
		countCh = 0;
      countLn++;
	}
	}
  fclose((*qwfile).fpin);
  return(countLn);
}

char ReadFilename(int argc, char **argv, QWFILE *qwfile)
{
  int i = 0;
  char *ext;
  char type[256];

  /** find the input filename */
  for (i=1;i<argc;i++) {
    if (argv[i][0] != '-') {
      if (!(argv[i-1][0] == '-' && argv[i-1][1] == 'o')) {
	(*qwfile).in = argv[i];
      }
    }
  }

  /** read the file extension from the end of the string */
  if ((ext = strrchr((*qwfile).in,'.')) != NULL) {
    strcpy(type,ext+1);
  if (strcmp(type,"adj") == 0)
    return('a');
  else if (strcmp(type,"qwml") == 0)
    return('q');
  else
    return('n');
  } else {
    return('n');
  }
}


