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
#ifndef PAULSLIB_H
#define PAULSLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <termios.h>
#include <unistd.h>

#define ABS(x) (x < 0 ? -(x) : (x))
#define CROSSPROD(p1,p2,p3) \
p3.x = p1.y*p2.z - p1.z*p2.y; \
p3.y = p1.z*p2.x - p1.x*p2.z; \
p3.z = p1.x*p2.y - p1.y*p2.x

#define YES 1
#define NO  0
#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif
#define CLOCKWISE 1
#define ANTICLOCKWISE (-1)
#define COUNTERCLOCKWISE (-1)
#define CONVEX 1
#define CONCAVE (-1)
#define ESC 27

#define THREEPI         9.424777960769379715387930
#define EPSILON         0.00000001
#define EPS             0.00001

#define DTOR            0.0174532925
#define RTOD            57.2957795
#define TWOPI           6.283185307179586476925287
#define PI              3.141592653589793238462643
#define PID2            1.570796326794896619231322
#define PID3            1.047197551196397746154214
#define PID4            0.7853981633974483096156608
#define PIPI            9.869604401089358618834491
#define E               2.718281828459045235360287
#define GAMMA           0.5772156649015328606065120
#define LOG2            0.3010299956639811952137389
#define LOG3            0.4772112547196624372950279
#define LOGPI           0.4971498726941338543512683
#define LOGE            0.4342944819032518276511289
#define LOG2E           1.4426950408889634073
#define LN2             0.6931471805599453094172321
#define LN3             1.098612288668109691395245
#define LNPI            1.144729885849400174143427
#define LNSQRT2         0.3465735902799726547
#define LNGAMMA         -0.549539293981644822337662
#define SQRT2           1.4142135623730950488
#define SQRT3           1.7320508075688772935
#define SQRT5           2.2360679774997896964
#define SQRT7           2.6457513110645905905
#define SQRTPI          1.772453850905516027298167

typedef struct {
	double r,g,b;
} COLOUR;

typedef struct {
	double x,y,z;
} XYZ;

void   Normalise(XYZ *);
XYZ    ArbitraryRotate(XYZ,double,XYZ);
COLOUR GetColour(double,double,double,int);
double GetRunTime(void);
double VectorLength(XYZ,XYZ);

#endif /* PAULSLIB_H */

#ifndef OPENGLLIB_H
#define OPENGLLIB_H

typedef struct {
	int id;
	char name[128];
} PANELITEM;
typedef struct {
	int id;
	int nitems;
	PANELITEM *items;
} PANEL;

int WindowDump(char *,int,int,int,int);
void CreateDisk(XYZ,XYZ,double,double,int,double,double);
void CreateCone(XYZ,XYZ,double,double,int,double,double);
void DrawGLText(int,int,char *);

#endif /* OPENGLLIB_H */



