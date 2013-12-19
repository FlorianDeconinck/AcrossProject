The Across Project
=============
The Across Project, is a game engine sandbox. The goal is to develop a flexible architecture to test different algorithm in many different fields (rendering, A.I., gameplay...) on a fun and open platform.
Even if speed and optimization isn't the primary goal here, they'll be sacrificed only for the sake of flexibility.

I'll try to write down some thoughts and technical details on how I implement the system as I go in the [project wiki](https://github.com/FlorianDeconinck/AcrossProject/wiki).

##Demos

[Deferred Rendering](http://www.youtube.com/watch?v=VG5u4h1CPLY) (the video forgot to mention that FXAA is on...)

##Sources

###Reads
This work is based on several reads : 
* For architecture and day-to-day mind-blowing stuff :
  * Jason Gregory's *Game Engine Architecture*
  * #AltDevBlog [(link)](http://www.altdevblogaday.com/)
* For A.I. 
  * Pathfinder - JPS : pruning algorithm for grid-based pathfinder by Daniel Harabor and Alban Grastien [(paper)](http://users.cecs.anu.edu.au/~dharabor/data/papers/harabor-grastien-socs12.pdf)
  * AiGameDev [(link)](http://aigamedev.com/)

###Libraries
* ImGUI by Adrien Herubel [(GitHub)](https://github.com/AdrienHerubel/imgui), which was inspired from Recast GUI by Mikko Mononen [(GitHub)](https://github.com/memononen/recastnavigation)
* For cross-plateform image manipulation :  [DevIL SDK](http://openil.sourceforge.net/)
* For XML document manipulation : [pugixml 1.2](http://pugixml.org/)
* For 3D mesh/animation translation from several modelers (MAYA being the one I use mostly) to the engine format : [AssImp](http://assimp.sourceforge.net/)

##Project implemented list
* Environment
  * 2.5D grid based
  * 3D grid based
* Basic rendering OpenGL dependant structure
  * Thick lines geometry shader
  * Anti - Aliasing : FXAA shader
  * Deferred Rendering (Directional & Point lights) - DEMO #1
* XML base resource manager
* Small editor
* Animation
 * Messy sprites animation
* A* pathfinder - 2D
  * Pruning acceleration : JPS
* Memory
  * Stack allocator (for pre-allocated memory usage) 



[Work of the moment](https://github.com/FlorianDeconinck/AcrossProject/wiki/Across-Wiki---News#and-now-on-the-across-project)
