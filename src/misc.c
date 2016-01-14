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
extern char *optionstring;
extern char *interfacestring;

/**
  Display the program usage information
*/
void GiveUsage(char *cmd)
{
  fprintf(stderr,"=======================================================\n"); 
  fprintf(stderr,"This is qwViz.\n"); 
  fprintf(stderr,"Usage: %s [options] infile\n\n",cmd);
  fprintf(stderr,"Infile must be \n   .qwml (for plotting existing data)");
  fprintf(stderr," or \n   .adj file (for computing quantum walk data - see quantum walk options)\n");
  fprintf(stderr,"=======================================================\n\n");
  fprintf(stderr,"%s\n",optionstring);
  fprintf(stderr,"%s\n",interfacestring);
  exit(-1);
}

/**
  Set a node UV depending on texture mode
  u,v are in normalise coordinates
*/
void NodeUV(double u,double v,int width,int height,int texturetype)
{
  if (texturetype == GL_TEXTURE_2D)
    glTexCoord2f(u,v);
  else
    glTexCoord2f(u*width,v*height);
}


