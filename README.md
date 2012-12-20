Watershed_Reduction
===================

An attempt to do map reduction on watershed data
using only the standard libraries, we attempt
to take publically available watershed data found here: 
http://eros.usgs.gov/#/Find_Data/Products_and_Data_Available/gtopo30/hydro
and reduce the level 6 polygons to the outter edge only level 2 polygon.

In essence, we want the "outline" of the level 2 mesh, based on the level 6 polygon information.

There are a couple things that make this a bit "slow":
- The original data is not well formed for graphical edge-reduction work. The creators take no care of sharing verticies or T-junctions. This means you have to worry about all possible T-junction scenarios. So, for example:
---------
|       |-----
|       |    |
|       |-----
|       |
---------

in this case, the shared edge is not actually "shared" in the original code -- we must find this T-junction and eliminate it ourselves. 

- Polygons are not stored as triangles, but more as a line-list of verts. This is (possibly) easy for rendering, but makes it a pain for doing edge-detection work

This is still a work in progress. Currently in place:
- T-junction removal code which checks the following cases:
1) non-touching polygons
2) polygons whose edges share a complete edge
3) polygons a,b where an edge in a is a sub-edge in b (ie it is on the same line but is a line segment that is shorter than b). In this case, the original data does not have the line segment points listed in a, so we must insert them
4) polygons a, b where each polygon is "partial" shared by the other. example:
----
|  |
|  |----
|  |   |
----   |
   |   |
   |   |
   -----

(its bad ascii art, i know. the shared partial is supposed to be the same line)

again, the original data file does not indicate the single shared vertex, and we must add it in

