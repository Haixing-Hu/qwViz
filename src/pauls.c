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
#if defined(__linux__)
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#warning "Not sure where to find glut.h"
#endif
#include "pauls.h"
#include "bitmap.h"

/*
 Return a colour from one of a number of colour ramps
 type == 1  blue -> cyan -> green -> magenta -> red 
 2  blue -> red 
 3  grey scale 
 4  red -> yellow -> green -> cyan -> blue -> magenta -> red 
 5  green -> yellow 
 6  green -> magenta
 7  blue -> green -> red -> green -> blue
 8  black -> white -> black
 9  red -> blue -> cyan -> magenta
 10  blue -> cyan -> green -> yellow -> red -> white
 11  dark brown -> lighter brown (Mars colours, 2 colour transition)
 12  3 colour transition mars colours
 13  landscape colours, green -> brown -> yellow
 14  yellow -> red
 15  blue -> cyan -> green -> yellow -> brown -> white
 16  blue -> green -> red       (Chromadepth for black background)
 17  yellow -> magenta -> cyan  (Chromadepth for white background)
 18  blue -> cyan
 19  blue -> white
 20  landscape colours, modified, green -> brown -> yellow
 21  yellowish to blueish
 22  yellow to blue
 23  red to white to blue
 24  blue -> yellow -> red
 25  blue -> cyan -> yellow -> red
 v should lie between vmin and vmax otherwise it is clipped
 The colour components range from 0 to 1
 */
COLOUR GetColour(double v,double vmin,double vmax,int type)
{
   double dv,vmid;
	COLOUR c = {1.0,1.0,1.0};
	COLOUR c1,c2,c3;
	double ratio;

	if (vmax < vmin) {
		dv = vmin;
		vmin = vmax;
		vmax = dv;
	}
	if (vmax - vmin < 0.000001) {
		vmin -= 1;
		vmax += 1;
	}
   if (v < vmin)
      v = vmin;
   if (v > vmax)
      v = vmax;
   dv = vmax - vmin;

	switch (type) {
	case 1:
   	if (v < (vmin + 0.25 * dv)) {
      	c.r = 0;
      	c.g = 4 * (v - vmin) / dv;
			c.b = 1;
   	} else if (v < (vmin + 0.5 * dv)) {
      	c.r = 0;
			c.g = 1;
      	c.b = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
   	} else if (v < (vmin + 0.75 * dv)) {
      	c.r = 4 * (v - vmin - 0.5 * dv) / dv;
			c.g = 1;
      	c.b = 0;
   	} else {
			c.r = 1;
      	c.g = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
      	c.b = 0;
   	}
		break;
	case 2:
		c.r = (v - vmin) / dv;
		c.g = 0;
		c.b = (vmax - v) / dv;
		break;
	case 3:
   	c.r = (v - vmin) / dv;
   	c.b = c.r;
   	c.g = c.r;
		break;
	case 4:
      if (v < (vmin + dv / 6.0)) {
         c.r = 1; 
         c.g = 6 * (v - vmin) / dv;
         c.b = 0;
      } else if (v < (vmin + 2.0 * dv / 6.0)) {
         c.r = 1 + 6 * (vmin + dv / 6.0 - v) / dv;
         c.g = 1;
         c.b = 0;
      } else if (v < (vmin + 3.0 * dv / 6.0)) {
         c.r = 0;
         c.g = 1;
         c.b = 6 * (v - vmin - 2.0 * dv / 6.0) / dv;
      } else if (v < (vmin + 4.0 * dv / 6.0)) {
         c.r = 0;
         c.g = 1 + 6 * (vmin + 3.0 * dv / 6.0 - v) / dv;
         c.b = 1;
      } else if (v < (vmin + 5.0 * dv / 6.0)) {
         c.r = 6 * (v - vmin - 4.0 * dv / 6.0) / dv;
         c.g = 0;
         c.b = 1;
      } else {
         c.r = 1;
         c.g = 0;
         c.b = 1 + 6 * (vmin + 5.0 * dv / 6.0 - v) / dv;
      }
		break;
   case 5:
      c.r = (v - vmin) / (vmax - vmin);
      c.g = 1;
      c.b = 0;
		break;
   case 6:
      c.r = (v - vmin) / (vmax - vmin);
      c.g = (vmax - v) / (vmax - vmin);
      c.b = c.r;
		break;
   case 7:
      if (v < (vmin + 0.25 * dv)) {
         c.r = 0;
         c.g = 4 * (v - vmin) / dv;
         c.b = 1 - c.g;
      } else if (v < (vmin + 0.5 * dv)) {
			c.r = 4 * (v - vmin - 0.25 * dv) / dv;
         c.g = 1 - c.r;
         c.b = 0;
      } else if (v < (vmin + 0.75 * dv)) {
         c.g = 4 * (v - vmin - 0.5 * dv) / dv;
			c.r = 1 - c.g;
         c.b = 0;
      } else {
         c.r = 0;
         c.b = 4 * (v - vmin - 0.75 * dv) / dv;
			c.g = 1 - c.b;
      }
      break;
   case 8:
      if (v < (vmin + 0.5 * dv)) {
         c.r = 2 * (v - vmin) / dv;
         c.g = c.r;
         c.b = c.r;
      } else {
         c.r = 1 - 2 * (v - vmin - 0.5 * dv) / dv;
			c.g = c.r;
         c.b = c.r;
      }
      break;
   case 9:
      if (v < (vmin + dv / 3)) {
         c.b = 3 * (v - vmin) / dv;
			c.g = 0;
         c.r = 1 - c.b;
      } else if (v < (vmin + 2 * dv / 3)) {
         c.r = 0;
         c.g = 3 * (v - vmin - dv / 3) / dv;
         c.b = 1;
      } else {
         c.r = 3 * (v - vmin - 2 * dv / 3) / dv;
         c.g = 1 - c.r;
			c.b = 1;
      }
      break;
   case 10:
      if (v < (vmin + 0.2 * dv)) {
         c.r = 0;
         c.g = 5 * (v - vmin) / dv;
         c.b = 1;
      } else if (v < (vmin + 0.4 * dv)) {
         c.r = 0;
         c.g = 1;
         c.b = 1 + 5 * (vmin + 0.2 * dv - v) / dv;
      } else if (v < (vmin + 0.6 * dv)) {
         c.r = 5 * (v - vmin - 0.4 * dv) / dv;
         c.g = 1;
         c.b = 0;
      } else if (v < (vmin + 0.8 * dv)) {
         c.r = 1;
         c.g = 1 - 5 * (v - vmin - 0.6 * dv) / dv;
         c.b = 0;
      } else {
         c.r = 1;
         c.g = 5 * (v - vmin - 0.8 * dv) / dv;
         c.b = 5 * (v - vmin - 0.8 * dv) / dv;
      }
      break;
   case 11:
		c1.r = 200 / 255.0; c1.g =  60 / 255.0; c1.b =   0 / 255.0;
		c2.r = 250 / 255.0; c2.g = 160 / 255.0; c2.b = 110 / 255.0;
      c.r = (c2.r - c1.r) * (v - vmin) / dv + c1.r;
      c.g = (c2.g - c1.g) * (v - vmin) / dv + c1.g;
      c.b = (c2.b - c1.b) * (v - vmin) / dv + c1.b;
      break;
	case 12:
		c1.r =  55 / 255.0; c1.g =  55 / 255.0; c1.b =  45 / 255.0;
      /* c2.r = 200 / 255.0; c2.g =  60 / 255.0; c2.b =   0 / 255.0; */
		c2.r = 235 / 255.0; c2.g =  90 / 255.0; c2.b =  30 / 255.0;
		c3.r = 250 / 255.0; c3.g = 160 / 255.0; c3.b = 110 / 255.0;
		ratio = 0.4;
		vmid = vmin + ratio * dv;
		if (v < vmid) {
      	c.r = (c2.r - c1.r) * (v - vmin) / (ratio*dv) + c1.r;
      	c.g = (c2.g - c1.g) * (v - vmin) / (ratio*dv) + c1.g;
      	c.b = (c2.b - c1.b) * (v - vmin) / (ratio*dv) + c1.b;
		} else {
         c.r = (c3.r - c2.r) * (v - vmid) / ((1-ratio)*dv) + c2.r;
         c.g = (c3.g - c2.g) * (v - vmid) / ((1-ratio)*dv) + c2.g;
         c.b = (c3.b - c2.b) * (v - vmid) / ((1-ratio)*dv) + c2.b;
		}
		break;
	case 13:
      c1.r =   0 / 255.0; c1.g = 255 / 255.0; c1.b =   0 / 255.0;
      c2.r = 255 / 255.0; c2.g = 150 / 255.0; c2.b =   0 / 255.0;
      c3.r = 255 / 255.0; c3.g = 250 / 255.0; c3.b = 240 / 255.0;
      ratio = 0.3;
      vmid = vmin + ratio * dv;
      if (v < vmid) {
         c.r = (c2.r - c1.r) * (v - vmin) / (ratio*dv) + c1.r;
         c.g = (c2.g - c1.g) * (v - vmin) / (ratio*dv) + c1.g;
         c.b = (c2.b - c1.b) * (v - vmin) / (ratio*dv) + c1.b;
      } else {
         c.r = (c3.r - c2.r) * (v - vmid) / ((1-ratio)*dv) + c2.r;
         c.g = (c3.g - c2.g) * (v - vmid) / ((1-ratio)*dv) + c2.g;
         c.b = (c3.b - c2.b) * (v - vmid) / ((1-ratio)*dv) + c2.b;
      }
		break;
   case 14:
      c.r = 1;
      c.g = 1 - (v - vmin) / dv;
      c.b = 0;
      break;
   case 15:
      if (v < (vmin + 0.25 * dv)) {
         c.r = 0;
         c.g = 4 * (v - vmin) / dv;
         c.b = 1;
      } else if (v < (vmin + 0.5 * dv)) {
         c.r = 0;
         c.g = 1;
         c.b = 1 - 4 * (v - vmin - 0.25 * dv) / dv;
      } else if (v < (vmin + 0.75 * dv)) {
			c.r = 4 * (v - vmin - 0.5 * dv) / dv;
         c.g = 1;
         c.b = 0;
      } else {
         c.r = 1;
			c.g = 1;
         c.b = 4 * (v - vmin - 0.75 * dv) / dv;
      }
      break;
   case 16:
      if (v < (vmin + 0.5 * dv)) {
         c.r = 0.0;
         c.g = 2 * (v - vmin) / dv;
         c.b = 1 - 2 * (v - vmin) / dv;
      } else {
         c.r = 2 * (v - vmin - 0.5 * dv) / dv;
         c.g = 1 - 2 * (v - vmin - 0.5 * dv) / dv;
         c.b = 0.0;
      }
      break;
   case 17:
      if (v < (vmin + 0.5 * dv)) {
         c.r = 1.0;
         c.g = 1 - 2 * (v - vmin) / dv;
         c.b = 2 * (v - vmin) / dv;
      } else {
         c.r = 1 - 2 * (v - vmin - 0.5 * dv) / dv;
         c.g = 2 * (v - vmin - 0.5 * dv) / dv;
         c.b = 1.0;
      }
      break;
   case 18:
      c.r = 0;
      c.g = (v - vmin) / (vmax - vmin);
      c.b = 1;
      break;
   case 19:
      c.r = (v - vmin) / (vmax - vmin);
      c.g = c.r;
      c.b = 1;
      break;
   case 20:
      c1.r =   0 / 255.0; c1.g = 160 / 255.0; c1.b =   0 / 255.0;
      c2.r = 180 / 255.0; c2.g = 220 / 255.0; c2.b =   0 / 255.0;
      c3.r = 250 / 255.0; c3.g = 220 / 255.0; c3.b = 170 / 255.0;
      ratio = 0.3;
      vmid = vmin + ratio * dv;
      if (v < vmid) {
         c.r = (c2.r - c1.r) * (v - vmin) / (ratio*dv) + c1.r;
         c.g = (c2.g - c1.g) * (v - vmin) / (ratio*dv) + c1.g;
         c.b = (c2.b - c1.b) * (v - vmin) / (ratio*dv) + c1.b;
      } else {
         c.r = (c3.r - c2.r) * (v - vmid) / ((1-ratio)*dv) + c2.r;
         c.g = (c3.g - c2.g) * (v - vmid) / ((1-ratio)*dv) + c2.g;
         c.b = (c3.b - c2.b) * (v - vmid) / ((1-ratio)*dv) + c2.b;
      }
      break;
   case 21:
      c1.r = 255 / 255.0; c1.g = 255 / 255.0; c1.b = 200 / 255.0;
      c2.r = 150 / 255.0; c2.g = 150 / 255.0; c2.b = 255 / 255.0;
      c.r = (c2.r - c1.r) * (v - vmin) / dv + c1.r;
      c.g = (c2.g - c1.g) * (v - vmin) / dv + c1.g;
      c.b = (c2.b - c1.b) * (v - vmin) / dv + c1.b;
      break;
   case 22:
      c.r = 1 - (v - vmin) / dv;
      c.g = 1 - (v - vmin) / dv;
      c.b = (v - vmin) / dv;
      break;
   case 23:
      if (v < (vmin + 0.5 * dv)) {
         c.r = 1;
         c.g = 2 * (v - vmin) / dv;
         c.b = c.g;
      } else {
         c.r = 1 - 2 * (v - vmin - 0.5 * dv) / dv;
         c.g = c.r;
         c.b = 1;
      }
      break;
   case 24:
      if (v < (vmin + 0.5 * dv)) {
         c.r = 2 * (v - vmin) / dv;
         c.g = c.r;
         c.b = 1 - c.r;
      } else {
         c.r = 1;
         c.g = 1 - 2 * (v - vmin - 0.5 * dv) / dv;
         c.b = 0;
      }
      break;
   case 25:
      if (v < (vmin + dv / 3)) {
         c.r = 0;
         c.g = 3 * (v - vmin) / dv;
         c.b = 1;
      } else if (v < (vmin + 2 * dv / 3)) {
         c.r = 3 * (v - vmin - dv / 3) / dv;
         c.g = 1 - c.r;
         c.b = 1;
      } else {
         c.r = 1;
         c.g = 0;
         c.b = 1 - 3 * (v - vmin - 2 * dv / 3) / dv;
      }
      break;
	}
	return(c);
}

/* SYSTEM SPECIFIC  --------------------------------------------------*/

/*
 Time scale at microsecond resolution but returned as seconds
 */
double GetRunTime(void)
{
#ifndef WIN32
	double sec = 0;
	struct timeval tp;
	
	gettimeofday(&tp,NULL);
	sec = tp.tv_sec + tp.tv_usec / 1000000.0;
#else
	DWORD systime;
	double sec = 0;
	
	systime = timeGetTime();
	sec  = systime/1000.0;
#endif
	
	return(sec);
}

/*-------------------------------------------------------------------------
 Normalise a vector
 */
void Normalise(XYZ *p)
{
	double length;
	
	length = p->x * p->x + p->y * p->y + p->z * p->z;
	if (length > 0) {
		length = sqrt(length);
		p->x /= length;
		p->y /= length;
		p->z /= length;
	} else {
		p->x = 0;
		p->y = 0;
		p->z = 0;
	}	
}


/*
 Rotate a point p by angle theta around an arbitrary normal r
 Return the rotated point.
 Positive angles are anticlockwise looking down the axis towards the origin.
 Assume right hand coordinate system.  
 */
XYZ ArbitraryRotate(XYZ p,double theta,XYZ r)
{
	XYZ q = {0.0,0.0,0.0};
	double costheta,sintheta;
	
	Normalise(&r);
	costheta = cos(theta);
	sintheta = sin(theta);
	
	q.x += (costheta + (1 - costheta) * r.x * r.x) * p.x;
	q.x += ((1 - costheta) * r.x * r.y - r.z * sintheta) * p.y;
	q.x += ((1 - costheta) * r.x * r.z + r.y * sintheta) * p.z;
	
	q.y += ((1 - costheta) * r.x * r.y + r.z * sintheta) * p.x;
	q.y += (costheta + (1 - costheta) * r.y * r.y) * p.y;
	q.y += ((1 - costheta) * r.y * r.z - r.x * sintheta) * p.z;
	
	q.z += ((1 - costheta) * r.x * r.z - r.y * sintheta) * p.x;
	q.z += ((1 - costheta) * r.y * r.z + r.x * sintheta) * p.y;
	q.z += (costheta + (1 - costheta) * r.z * r.z) * p.z;
	
	return(q);
}

/*-------------------------------------------------------------------------
 Return the distance between two points
 */
double VectorLength(XYZ p1,XYZ p2)
{
	XYZ d;
	
	d.x = p1.x - p2.x;
	d.y = p1.y - p2.y;
	d.z = p1.z - p2.z;
	
	return(sqrt(d.x*d.x + d.y*d.y + d.z*d.z));
}


/*
 Write the current view to an image file
 Do the right thing for stereo, ie: two images
 The format corresponds to the Write_Bitmap() formats. 
 Use a negative format to get the image flipped vertically
 Honour the name if supplied, else do automatic naming
 Don't overwrite existing files if automatic naming is in effect.
 
 */
int WindowDump(char *name,int width,int height,int stereo,int format)
{
	FILE *fptr;
	static int counter = 0;
	char fname[32],ext[8];
	BITMAP4 *image = NULL;
	
	/* Allocate our buffer for the image */
	if ((image = Create_Bitmap(width,height)) == NULL) {
		fprintf(stderr,"WindowDump - Failed to allocate memory for image\n");
		return(FALSE);
	}
	
	glFinish();
	glPixelStorei(GL_PACK_ALIGNMENT,1);
	
	/* Open the file */
	switch (ABS(format)) {
		case 1: strcpy(ext,"tga"); break;
		case 11: strcpy(ext,"tga"); break;
		case 12: strcpy(ext,"tga"); break;
		case 13: strcpy(ext,"tga"); break;
		case 2: strcpy(ext,"ppm"); break;
		case 3: strcpy(ext,"rgb"); break;
		case 4: strcpy(ext,"raw"); break;
		case 5: strcpy(ext,"tif"); break;
		case 6: strcpy(ext,"eps"); break;
		case 7: strcpy(ext,"eps"); break;
		case 8: strcpy(ext,"raw"); break;
		case 9: strcpy(ext,"bmp"); break;
	}
	if (strlen(name) <= 0) {
		if (stereo)
			sprintf(fname,"L_%04d.%s",counter,ext);
		else
			sprintf(fname,"%04d.%s",counter,ext);
	} else {
		if (stereo)
			sprintf(fname,"L_%s.%s",name,ext);
		else
			sprintf(fname,"%s.%s",name,ext);
	}
	while (strlen(name) <= 0 && (fptr = fopen(fname,"rb")) != NULL) {
		counter++;
		fclose(fptr);
		if (stereo)
			sprintf(fname,"L_%04d.%s",counter,ext);
		else
			sprintf(fname,"%04d.%s",counter,ext);
	}
	if ((fptr = fopen(fname,"wb")) == NULL) {
		fprintf(stderr,"WindowDump - Failed to open file for window dump\n");
		return(FALSE);
	}
	
	/* Copy the image into our buffer */
	glReadBuffer(GL_BACK_LEFT);
	glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,image);
	
	/* Write the file */
	Write_Bitmap(fptr,image,width,height,format);
	fclose(fptr);
	
	if (stereo) {
		
		/* Open the file */
		if (strlen(name) <= 0) {
			sprintf(fname,"R_%04d.%s",counter,ext);
		} else {
			sprintf(fname,"R_%s.%s",name,ext);
		}
		while (strlen(name) <= 0 && (fptr = fopen(fname,"rb")) != NULL) {
			counter++;
			fclose(fptr);
			sprintf(fname,"R_%04d.%s",counter,ext);
		}
		if ((fptr = fopen(fname,"wb")) == NULL) {
			fprintf(stderr,"WindowDump - Failed to open file for window dump\n");
			return(FALSE);
		}
		
		/* Copy the image into our buffer */
		glReadBuffer(GL_BACK_RIGHT);
		glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,image);
		
		/* Write the file */
		Write_Bitmap(fptr,image,width,height,format);
		fclose(fptr);
	}
	
	Destroy_Bitmap(image);
	counter++;
	return(TRUE);
}


/*
 Draw some text on the screen in a particular colour
 Coordinates are 0 to screen width and 0 to screenheight
 This particular implementation uses a 8 by 13 nonproprotional font
 Other bitmap fonts should be
 GLUT_BITMAP_9_BY_15
 GLUT_BITMAP_8_BY_13
 GLUT_BITMAP_TIMES_ROMAN_10
 GLUT_BITMAP_TIMES_ROMAN_24
 GLUT_BITMAP_HELVETICA_10
 GLUT_BITMAP_HELVETICA_12
 GLUT_BITMAP_HELVETICA_18
 */
void DrawGLText(int x,int y,char *s)
{
	int lines;
	char *p;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,glutGet(GLUT_WINDOW_WIDTH),0,glutGet(GLUT_WINDOW_HEIGHT),-1,1); 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glRasterPos2i(x,y);
	for (p=s,lines=0;*p;p++) {
		if (*p == '\n' || *p == '\r') {
			lines++;
			glRasterPos2i(x,y-(lines*13));
		} else {
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13,*p);
		}
	}
	
	glPopMatrix();
	glPopMatrix();
}


/*
 Create a cone/cylinder uncapped between end points p1, p2
 radius r1, r2, and precision m
 Create the cylinder between theta1 and theta2 in radians
 */
void CreateCone(XYZ p1,XYZ p2,double r1,double r2,int m,double theta1,double theta2)
{
	int i;
	double theta;
	XYZ n,p,q,perp;
	
	/* Normal pointing from p1 to p2 */
	n.x = p1.x - p2.x;
	n.y = p1.y - p2.y;
	n.z = p1.z - p2.z;
	Normalise(&n);
	
	/*
	 Create two perpendicular vectors perp and q
	 on the plane of the disk
	 */
	perp = n;
	if (n.x == 0 && n.z == 0)
		perp.x += 1;
	else
		perp.y += 1;
	CROSSPROD(perp,n,q);
	CROSSPROD(n,q,perp);
	Normalise(&perp);
	Normalise(&q);
	
	glBegin(GL_QUAD_STRIP);
	for (i=0;i<=m;i++) {
		theta = theta1 + i * (theta2 - theta1) / m;
		n.x = cos(theta) * perp.x + sin(theta) * q.x;
		n.y = cos(theta) * perp.y + sin(theta) * q.y;
		n.z = cos(theta) * perp.z + sin(theta) * q.z;
		Normalise(&n);
		
		p.x = p1.x + r1 * n.x;
		p.y = p1.y + r1 * n.y;
		p.z = p1.z + r1 * n.z;
		glNormal3f(n.x,n.y,n.z);
		glTexCoord2f(i/(double)m,0.0);
		glVertex3f(p.x,p.y,p.z);
		
		p.x = p2.x + r2 * n.x;
		p.y = p2.y + r2 * n.y;
		p.z = p2.z + r2 * n.z;
		glNormal3f(n.x,n.y,n.z);
		glTexCoord2f(i/(double)m,1.0);
		glVertex3f(p.x,p.y,p.z);
		
	}
	glEnd();
}


/*
 Create a disk centered at c, with normal n,
 inner radius r0, and outer radius r1, and precision m
 theta1 and theta2 are the start and end angles in radians
 */
void CreateDisk(XYZ c,XYZ n,double r0,double r1,int m,double theta1,double theta2)
{
	int i;
	double theta;
	XYZ p,q,perp;
	
	/* 
     Create two perpendicular vectors perp and q 
     on the plane of the disk 
	 */
	Normalise(&n);
	perp = n;
	if (n.x == 0 && n.z == 0)
		perp.x += 1;
	else
		perp.y += 1;
	CROSSPROD(perp,n,q);
	CROSSPROD(n,q,perp);
	Normalise(&perp);
	Normalise(&q);
	
	glBegin(GL_QUAD_STRIP);
	for (i=0;i<=m;i++) {
		theta = theta1 + i * (theta2 - theta1) / m;
		p.x = c.x + r0 * (cos(theta) * perp.x + sin(theta) * q.x);
		p.y = c.y + r0 * (cos(theta) * perp.y + sin(theta) * q.y);
		p.z = c.z + r0 * (cos(theta) * perp.z + sin(theta) * q.z);
		glNormal3f(n.x,n.y,n.z);
		glTexCoord2f(i/(double)m,0.0);
		glVertex3f(p.x,p.y,p.z);
		p.x = c.x + r1 * (cos(theta) * perp.x + sin(theta) * q.x);
		p.y = c.y + r1 * (cos(theta) * perp.y + sin(theta) * q.y);
		p.z = c.z + r1 * (cos(theta) * perp.z + sin(theta) * q.z);
		glNormal3f(n.x,n.y,n.z);
		glTexCoord2f(i/(double)m,1.0);
		glVertex3f(p.x,p.y,p.z);
	}
	glEnd();
}



