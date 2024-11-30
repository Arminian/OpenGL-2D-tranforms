# Project Description
This project assignment consists of creation of two 2D objects, their transformation and user input for applied transformations.
# Project Technologies
- OpenGL 3.3
- GLFW
- GLAD
- GLM
- vectors
# Functionality
(Input will be listed as first key for the left object and second for the right object)
- Drag object using the mouse and left mouse button
- Move objects using "arrow keys" and "WASD"
- Rotate objects using "R" and "T"
- Scale objects using "<" ">" and "{" "}"
- Scale both objects using mouse wheel
## Python code
Python code was added to find the necessary coordinates of a decagon figure and normalize them to the [-1, 1] scale required by the shaders of OpenGL. This is a helper program, not a dependency.
## What is lacking
Line smoothing has not been implemented for the lack of fiesable methods of achieving interpolation without breaking the objects. Each tested functionality has been scraped for malfunctioning.
