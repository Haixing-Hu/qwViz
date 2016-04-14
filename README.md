# qwViz

Visualisation of quantum walks on graphs.

This code was originally taken from the folllowing paper:

Scott D. Berry, Paul Bourke and Jingbo B. Wang, [qwViz: visualisation of quantum walks on graphs](http://www.sciencedirect.com/science/article/pii/S0010465511002128), Computer Physics Communications, vol 182, issue 10, 2011, p2295–2302.

The v1.0 souce code could be downloaded from:

http://cpc.cs.qub.ac.uk/summaries/AEJN_v1_0.html

Since the code was written many years ago, and it cannot be compiled under the recent OS X platform.

The author Scott D. Berry allowed me to port the code to the github and fix the compiling issues.

The code was fixed by me and Xiaogang Qiang. Now it works perfectly on the latest Mac OS X (10.11.3).

## Install

You may need to install the following required library before compiling the tool:

- graphviz
- openGL
- glut

On Mac OS X only the `graphviz` is required, which can be install via [brew](https://brew.sh).

Then go to the `src` directory and type `make`.