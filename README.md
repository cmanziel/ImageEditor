# ImageEditor

The aim of this project is to try to make two separate projects work together: one is an application for drawing on a window with the mouse cursor as a brush using opengl and the other is a static library which functions are wrappers around the zlib compression library functions.
Through these functions valid datasreams for displaying PNG images can be generated.

Main classes:
- Window: it takes care of the glfw window creation and image decompression
- PointBuffer: this is the class that allocates a grid of points that is accessed to modify the color of the window's pixel through functions like InsertPoint and RemovePoint
- Brush: this class keeps track of the cursor position and implements how points around it are drawn. The cursor is implemented as a circular brush whose radius grows as the cursor's speed increases. The brush class also keeps track of the state, which determines if the brush is drawing or erasing points.

## Image decompression
First of all the PNG image to be edited should be decompressed to get the raw image data, that is the 8 bit rgb values for every pixel.
This is done in the Window class' constructor by calling the pnglib's static library function *decomp* which returns the array of raw uncompressed image data.

## Window creation
In the Window class' constructor the image's width and height are retrieved by the pnglib's functions and are used to create a window of the same dimensions. Then each pixel of the window is given its image counterpart's color value, making the window display the image as its "canvas" onto which the cursor can draw.

This is done by passing the raw image data to the PointBuffer class' constructor. In here a bidimensional array of *point* structures is allocated and a single opengl buffer object is created to hold every value for all the points in the grid.

```
*point* structure:
point {
	float x;
	float y;
	unsigned int offset; // value used for overwirting the data for the specific point when calling glBufferSubData
	bool drawn;
	color p_color; // color of the point in question: it is the color of the image's pixel or of the default background, unless the point is drawn then it is the color of the brush
};
```

## Drawing and erasing:
The brush can have three different states:

- STATE_INACTIVE: the state for when the cursor is outside the window

- STATE_DRAW: every frame the PointBuffer class' InsertPoint method is called:
First of all are defined the x and y boundaries of a grid of points around the cursor position
Then the points in this grid are iterated through:
Firstly is checked if the current point (*m_Grid[y][x]*) is already drawn, if it is then skip to the next one. If the point is inside the brush area and is not already drawn then its flag field *drawn* and its *p_color* field are updated, and their values update the buffer object's data store at the offset given by the *offset* field

- STATE_ERASE: every frame the PointBuffer class' RemovePoint method is called:
As in the InsertPoint function the point is checked if inside the brush area boundaries, then if it is already not drawn.
If both conditions are satisfied the point color is reset to the one of the image pixel at that position.
The *drawn* flag is set to 0.0 again. The data is finally written to the buffer object's data store at the particular point offset

## OpenGL side:
- Vertex shader: takes in a vec2 attribute for the position of the point being drawn, a float value flag to determine if the point should be drawn, a vec3 for the point's color, a uniform to set the point's size

- Fragment Shader: just outputs the color passed as an input by the vertex shader

## Some images from the development:
Brush not yet implemented. the cursor draws just the point at its coordinates relative to the window
![Just cursor](pictures/drawing.png)

Using a square brush, its dimension is modified by key input:
![Square brush](pictures/drawing_0.png)

Circular brush whose dimension depends on the cursor's speed, default white background:
![Circular brush, default background](pictures/edited_0.png)

Drawing on an image previously generated with this application:
![Drawing on image](pictures/collage.png)
<!-- ![Drawing on image](pictures/drawing_0_edited.png) -->