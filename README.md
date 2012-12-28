Watershed_Reduction
===================

An attempt to do map reduction on watershed data
using only the standard libraries, we attempt
to take publically available watershed data found here: 
http://eros.usgs.gov/#/Find_Data/Products_and_Data_Available/gtopo30/hydro
and reduce the level 6 polygons to the outter edge only level 2 polygon.

In essence, we want the "outline" of the level 2 mesh, based on the level 6 polygon information.

There are a few hangups we need to concern ourselves with:

- the data wasn't constructed for optimizaed visualization in mind, so the winding order is not uniform. To guarantee quick edge-detection, we rewind the edges into clockwise form

- pertaining to the lack of optimization, vertex values are repeated accross the entire data. In other words, there is no vertex indexing built in to the data we can leverage; if 3 lines share the same point, the point's actual X and Y values are repeated 3 times. For optimization and efficient memory usage, we must construct the vertex array and indexing ourselves. 

- There are no guarantees that neighboring watersheds share points. Lines are broken up into intervals of distinct size, but there is no guarantee on all lines having the same interval accross polygons. So far, this has not been violated in our sample data. We also have no guarantees that T-junctions don't occur, which ends up being the biggest factor in our reduction code.

- Unfortunatly, do to there being no guarantees about the data layout, we are forced to make a design decision. Either assume that T-junctions won't occur and that intervals are guaranteed, or do T-junction elimination ourselves. In some cases, particularly rendering where "hanging" edges my not be an issue, we can get away with the former. However, our end result is to have complete accuracy to be used with non-visualization projects. We need to do full T-junction checking

- Full T-junction detection involves checking a few cases:
1) two lines share both points. in other we can ignore these two lines since they are the same
2) two lines share a single point and their respective remaining points do not lie on the same direction vector. These are two different lines, and cannot be eliminated
3) two lines A,B share a single point, and the remaining point of A lies on the interior of line B. In this case, we need to eliminate the T-junction in B by adding a vertex at the location of A
4) two lines A,B partially overlap, sharing no points. In this case, we need to do T-junction removal on A where the point B lies, and on B where the point in A lies.

** So far, we have not had sample data that actually causes T-junctions, but again there are no guarantees. 

