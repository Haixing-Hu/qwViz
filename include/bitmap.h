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
#include "pauls.h"

#ifndef BITMAPLIB_H
#define BITMAPLIB_H

typedef struct {
	unsigned char r,g,b,a;
} BITMAP4;

typedef struct {
   unsigned char r,g,b;
} BITMAP3;

/* 18 bytes long */
typedef struct {
   unsigned char  idlength;
   char  colourmaptype;
   char  datatypecode;
   short int colourmaporigin;
   short int colourmaplength;
   char  colourmapdepth;
   short int x_origin;
   short int y_origin;
   short int width;
   short int height;
   char  bitsperpixel;
   char  imagedescriptor;
} TGAHEADER;

BITMAP4 *Create_Bitmap(int,int);
void Destroy_Bitmap(BITMAP4 *);
void Write_Bitmap(FILE *,BITMAP4 *,int,int,int);
void Erase_Bitmap(BITMAP4 *,int,int,BITMAP4);
void GaussianScale(BITMAP4 *,int,int,BITMAP4 *,int,int,double);
void BiCubicScale(BITMAP4 *,int,int,BITMAP4 *,int,int);
double BiCubicR(double);
int Draw_Pixel(BITMAP4 *,int,int,int,int,BITMAP4);
BITMAP4 Get_Pixel(BITMAP4 *,int,int,int,int);
void Draw_Line(BITMAP4 *,int,int,int,int,int,int,BITMAP4);
BITMAP4 Scale_Pixel(BITMAP4,double);
void Flip_Bitmap(BITMAP4 *,int,int,int);
int Same_BitmapPixel(BITMAP4,BITMAP4);
BITMAP4 YUV_to_Bitmap(int,int,int);

void BM_WriteLongInt(FILE *,char *,long);
void BM_WriteHexString(FILE *,char *);

void TGA_Info(FILE *,int *,int *,int *);
int TGA_Read(FILE *,BITMAP4 *,int *,int *);
void TGA_MergeBytes(BITMAP4 *,unsigned char *,int);
void WriteTGACompressedRow(FILE *,BITMAP4 *,int,int);

#endif /* BITMAPLIB_H */
