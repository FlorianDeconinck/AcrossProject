The Across Project
=============
The Across Project, is a 2.5D engine sandbox. The goal is to develop a flexible architecture to test different algorithm in many different fields (rendering, A.I., gameplay...) on a fun and open platform.
Even if speed and optimization isn't the primary goal here, they'll be sacrificed only for the sake of flexibility.
##Sources

###Reads
This work is based on several reads : 
* For architecture and day-to-day mind-blowing stuff :
  * Jason Gregory's *Game Engine Architecture*
  * #AltDevBlog [(link)](http://www.altdevblogaday.com/)
* For A.I. 
  * Pathfinder - JPS : pruning algorithm for grid-based pathfinder by Daniel Harabor and Alban Grastien [(paper)](http://users.cecs.anu.edu.au/~dharabor/data/papers/harabor-grastien-socs12.pdf)
  * AiGameDev [(link)](http://aigamedev.com/)
* For cross-plateform image manipulation :  [(DevIL SDK)](http://openil.sourceforge.net/)

###Libraries
* ImGUI by Adrien Herubel [(GitHub)](https://github.com/AdrienHerubel/imgui), which was inspired from Recast GUI by Mikko Mononen [(GitHub)](https://github.com/memononen/recastnavigation)

##Project implemented list
* Grid-based environment
* Basic rendering OpenGL dependant structure
  * ThickLines geometry shader
  * FXAA shader
  * Basic texture shader
* Animation
 * Messy sprites animation
* A* pathfinder
  * Pruning acceleration : JPS



[Work of the moment](https://github.com/FlorianDeconinck/AcrossProject/wiki/Across-Wiki---News#and-now-on-the-across-project)
