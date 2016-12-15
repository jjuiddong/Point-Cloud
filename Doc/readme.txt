
Robotic 3D Scan Repository
http://kos.informatik.uni-osnabrueck.de/3Dscans/


=================================================================================================================

24. by Dorit Borrmann, Helge Lauterbach, Sven Jorissen from the University of Wurzburg, 
	Germany and Marija Seder from the University of Zagreb (FER), Croatia. 

	zagreb_cathedral



This data set was recorded as part of the DAAD/ZMOS project ThermoBIM. It
contains 8 3D laser scans collected with the Riegl VZ-400 in front of the
cathedral in Zagreb. Each scan has up to 22,500,000 points with reflectance
values. You can download the complete data set with the 3D scans and rough
initial pose estimates acquired by manual registration. 
The final poses are computed by slam6D.

Description of Files:
---------------------

scan00?.3d contains the data points in a left-handed coordinate system (scanners
own coordinate system, SOCS). Each line represents one point, including the x-,
y-, z-coordinate abd the reflectance value.

scan00?.pose contains the pose estimates. The first line contains the position
as x-, y-, and z-coordinates. The second line contains the Euler angles in
degrees.

zagreb.net is the graph file used to match the scans with slam6d.

Intructions on matching the data set using slam6D
-------------------------------------------------
1. Download and unzip the scans to DIRECTORY
2. Download the graph file 'zagreb.net' to your main 3DTK directory.
3. Match the scans using
./bin/slam6D -s 0 -e 7 -f uosr -a 2 --anim=1 -d 300 --epsICP=0.00000001 -r 10 -O 1 -G 1 -I 300 -D 100 --epsSLAM=0.0000000001 -p -n zagreb.net DIRECTORY
4. View the results using
./bin/show -s 0 -e 7 -f uosr --reflectance DIRECTORY




=================================================================================================================

20. by Dorit Borrmann from Jacobs University Bremen gGmbH, Germany. 

	thermocolorlab



This data set was recorded in the Automation Lab at Jacobs University Bremen.
It contains scans recorded at 8 poses. At each pose 9 thermal images, 9 camera 
images, and 9 scans of 40 degrees are taken thus covering the full 360 degrees. 

Description of Files:
---------------------
thermocolorlab.tgz: 
scanXXX.3d contains the data collected at scan position scanXXX.pose. The 
points from each scan part are colored with the color information from the 
webcam and attributed with the temperature value from the thermal images. The 
pose information is obtained from odometry, refined using slam6D 
(downloadable from http://threedtk.de). Each line corresponds to one point, 
containing x-, y-, z-coordinates, 0, R, G, B color information, temperature.

1. Unpack the files to thermocolorlab.
2. View the result
./bin/show -s 0 -e 62 -f uos_rrgbt thermocolorlab



=================================================================================================================

19. by Dorit Borrmann, Jan Elseberg, HamidReza Houshiar, and Andreas Nuchter from Jacobs University Bremen gGmbH, Germany.

This data set was recorded using a Riegl VZ-400 and a Canon 1000D camera with 10 Megapixel. 
The camera has been co-calibrated with the scanner, such that a color is mapped to the 
3D points of the laser scanner. The complete data set is given as text file (x,y,z,r,g,b) 
using Riegl's coordinate system definitions as well as in the .las format.

tower.xyz
towerComplete.las


