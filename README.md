ThatGameCompany Demo
====================

Content
-------

So it's definitely still a work in progress, but one I like a lot and am going to keep working at. 1990-2010 was years 1 though 21 for me, so I felt like the demo should center around growing up and learning things. The idea is you (a little black circle) wander a world, getting bigger and bigger as a thought bubble above your head fills up with colorful ideas. Originally the circles were going to be stick figures, which could make the story quite a bit easier to follow, but that will have to wait till I have time to implement a decent walking animation system.

I really like how the visuals for the paper world worked out. I'm excited to play with it and add some more content and animation.

Code
----

For the last 5 days or so, I've been in whatever works mode trying to get this coded up. So the structure of the code may have devolved a bit. But ignoring the dubious dependencies and poor state logic, here are some pointers to the cooler bits of code.

path_shape.h/cpp and the circle and quadric curve shaders are where most of the complex rendering code is. The code is for resolution-independent, dynamic path rendering. Most of the technique is from a GPU Gems 3 article on path rendering (http://http.developer.nvidia.com/GPUGems3/gpugems3_ch25.html), but the stenciling tricks are from here (http://staffwww.itn.liu.se/~andyn/courses/tncg08/sketches06/sketches/0125-kokojima.pdf). Haven't had the chance to show off the dynamic capabilities of the path rendering in the demo yet, but it's a neat technique.

The particle simulation isn't nearly as complex, but still worked out pretty well. Only the emitters themselves collide with thought bubble. To keep the particles from leaking out into the black and white world, the stencil buffer is used again. The collisions themselves are quite simple and cheap--the thought bubble is modeled as a bunch of circles.

Libraries
---------

The libraries I choose were not for any practical reasons. I simply went for short non-descriptive acronyms that started with gl. So in total we have glew, glfw, glm, and gli. Here's some links if you really want to know what they stand for.
 
 * glew.sourceforge.net
 * www.glfw.org
 * glm.g-truc.net
 * www.g-truc.net/project-0024.html

Also a huge shout out to Lost and Taken (lostandtaken.com) -- a fantastic texture site.

Requirements
------------

Windows (till I switch this to cmake) and OpenGL 3.3.