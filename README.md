<script>
  (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){
  (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),
  m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)
  })(window,document,'script','//www.google-analytics.com/analytics.js','ga');

  ga('create', 'UA-45375729-1', 'github.com');
  ga('send', 'pageview');

</script>

The Across Project
=============
The Across Project, is a 2.5D engine sandbox. The goal is to develop a flexible architecture to test different algorithm in many different fields (rendering, A.I., gameplay...) on a fun and open platform.
Even if speed and optimization isn't the primary goal here, they'll be sacrificed only for the sake of flexibility.

I'll try to write down some thoughts and technical details on how I implement the system as I go in the [project wiki](https://github.com/FlorianDeconinck/AcrossProject/wiki).

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
* For cross-plateform image manipulation :  [(DevIL SDK)](http://openil.sourceforge.net/)

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
