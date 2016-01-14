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
extern CAMERA camera;

/**
   Form the menu structure
   Associate the menus with the right mouse button
*/
void CreateMenus(void)
{
   int mainmenu,cameramenu,optionsmenu,windowmenu;

   windowmenu = glutCreateMenu(HandleWindowMenu);
   glutAddMenuEntry(" 160 x 120 (NTSC 1/16)",1);
   glutAddMenuEntry(" 192 x 144 (PAL 1/16)",2);
   glutAddMenuEntry(" 320 x 240 (NTSC 1/4)",3);
   glutAddMenuEntry(" 384 x 256 (Photo CD)",4);
   glutAddMenuEntry(" 384 x 288 (PAL 1/4)",5);
   glutAddMenuEntry(" 400 x 300",6);
   glutAddMenuEntry(" 400 x 400",7);
   glutAddMenuEntry(" 640 x 480 (Full NTSC)",8);
   glutAddMenuEntry(" 600 x 600",9);
   glutAddMenuEntry(" 720 x 576 (DVD PAL)",10);
   glutAddMenuEntry(" 768 x 576 (Full PAL)",11);
   glutAddMenuEntry(" 800 x 600",12);
   glutAddMenuEntry("1024 x 768",13);
   glutAddMenuEntry("1000 x 1000",14);
   glutAddMenuEntry("1400 x 1050 SXGA+",15);
   glutAddMenuEntry("1920 x 1080 HD",16);

   cameramenu = glutCreateMenu(HandleCameraMenu);
   glutAddMenuEntry("Home   [X]",0);
   glutAddMenuEntry("Front  [F1]",1);
   glutAddMenuEntry("Back   [F2]",2);
   glutAddMenuEntry("Left   [F3]",3);
   glutAddMenuEntry("Right  [F4]",4);
   glutAddMenuEntry("Top    [F5]",5);
   glutAddMenuEntry("Bottom [F6]",6);

   optionsmenu = glutCreateMenu(HandleOptionsMenu);
   glutAddMenuEntry("Interface information [h]",1);
   glutAddMenuEntry("Information [i]",2);
   glutAddMenuEntry("Vertex identification [j]",3);

   mainmenu = glutCreateMenu(HandleMainMenu);
   glutAddSubMenu("Camera",cameramenu);
   glutAddSubMenu("Options",optionsmenu);
   glutAddSubMenu("Window size",windowmenu);
   glutAddMenuEntry("Quit [ESC]",10);

   glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/**
   Handle the window menu
*/
void HandleWindowMenu(int whichone)
{
  switch (whichone) {
  case  1: camera.screenwidth =  160; camera.screenheight = 120; break;
  case  2: camera.screenwidth =  192; camera.screenheight = 144; break;
  case  3: camera.screenwidth =  320; camera.screenheight = 240; break;
  case  4: camera.screenwidth =  384; camera.screenheight = 256; break;
  case  5: camera.screenwidth =  384; camera.screenheight = 288; break;
  case  6: camera.screenwidth =  400; camera.screenheight = 300; break;
  case  7: camera.screenwidth =  400; camera.screenheight = 400; break;
  case  8: camera.screenwidth =  640; camera.screenheight = 480; break;
  case  9: camera.screenwidth =  600; camera.screenheight = 600; break;
  case 10: camera.screenwidth =  720; camera.screenheight = 576; break;
  case 11: camera.screenwidth =  768; camera.screenheight = 576; break;
  case 12: camera.screenwidth =  800; camera.screenheight = 600; break;
  case 13: camera.screenwidth = 1024; camera.screenheight = 768; break;
  case 14: camera.screenwidth = 1000; camera.screenheight = 1000; break;
  case 15: camera.screenwidth = 1400; camera.screenheight = 1050; break;
  case 16: camera.screenwidth = 1920; camera.screenheight = 1080; break;
  }
  if (camera.stereo == DUALSTEREO)
    camera.screenwidth *= 2;
  glutReshapeWindow(camera.screenwidth,camera.screenheight);
}

/**
   Handle the camera menu
*/
void HandleCameraMenu(int whichone)
{
  CameraHome(whichone);
  if (whichone == 0)
    RotateCamera(0.0,30.0,0.0,1.0);
}

/**
   Handle the options menu
*/
void HandleOptionsMenu(int whichone)
{
  switch (whichone) {
  case 1:
    options.showhelp = !options.showhelp;
    break;
  case 2:
    options.showinfo = !options.showinfo;
    break;
  case 3:
    options.showarrow = !options.showarrow;
    break;
  }
}

/**
   Handle the main menu
*/
void HandleMainMenu(int whichone)
{
  switch (whichone) {
  case 10:
    exit(0);
    break;
  }
}


