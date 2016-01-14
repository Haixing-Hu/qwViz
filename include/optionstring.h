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
char *optionstring = "Options\n\
   -h                           This text\n\
   -d                           Debug/verbose mode\n\
   -f                           Full screen\n\
   -s                           Active stereo\n\
   -ss                          Dual screen stereo\n\
   -bg r g b                    Background colour\n\
   -cs i                        Colour scheme\n\
   -circo                       Layout the vertices in a circle\n\
   -fdp                         Use the Fruchterman-Reingold force-based graph layout algorithm\n\
   -tiff                        Change image export format to TIFF\n\
   -i [int]                     Linearly interpolate probability distribution [smoothness]\n\
\n\
Quantum walk options (.adj input required)\n\
   -start [int]                 Quantum walk starting from vertex [start position]\n\
   -search [int]                Quantum-walk-based search procedure [marked vertex]\n\
   -steps [int]                 [Number] of steps in the walk\n\
   -o [char]                    Write data to a file, .qwml or .prob extension determines output format\n\
";

