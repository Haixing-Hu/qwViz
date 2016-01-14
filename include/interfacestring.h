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
char *interfacestring = "Interface\n\
   Right mouse button           Popup menus\n\
   [ ]                          Roll camera\n\
   Left mouse button            Rotate camera\n\
   Arrow keys                   Rotate camera\n\
   Left mouse button + shift    Pan camera\n\
   Arrow keys + shift           Pan camera\n\
   < >                          Move camera forward or backward\n\
   + -                          Zoom in/out by changing aperture\n\
   a A (z)                      Automatic rotation (stop)\n\
   f1 ... f6                    Special camera views\n\
   x                            Return camera to initial position\n\
   p                            Toggle play/paused\n\
   n b                          Next/Back: increment or decrement the time by one step\n\
   u d                          Up/Down: adjust the scale for the plotted data\n\
   s f                          Slow/Fast: adjust the speed of the simulation\n\
   /                            Return to t = 0\n\
   c v                          Increase/decrease the radius of the cylinders\n\
   j                            Toggle vertex identification arrow on/off\n\
   k l                          Increment vertex identification arrow\n\
   w                            Window dump (default is TGA file)\n\
   r                            Record: Toggle continuous window dump (Use with care)\n\
   h                            Help: display interface information\n\
   i                            Info: display scale and time step information\n\
   q esc                        Quit";
