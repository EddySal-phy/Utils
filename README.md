# Utils
Useful scripts to handle files, plots and data in columns in general.


## filter_plot.sh

  A simple bash script to delete points in a plot by selecting an area using gnuplot.
   
### How to use:

> The bash script contains two files: a "reference" and a "modifiable". The data deleted is the one indicated as <DATA2_FILE> (the "modifiable"). The "reference" exists since one might be interested to delete data along some boundary defined by another data or plot;

> Define the path-to-file for "reference" and "modifiable" as needed, as well as the x and y coordinates of points to be deleted;

> Zoom in/out as needed in the gnuplot window by right clicking;

> Press Enter in terminal and select the two diagonal vertices of area to remove

> All data of the "modifiable" in the same row as the x and y coordinates in the plot will be deleted.

> You can undo the delete by running the command indicated in terminal (that will appear when points are deleted)    
