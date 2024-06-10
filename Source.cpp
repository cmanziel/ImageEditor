#include <stdio.h>
#include <stdlib.h>

#include "Window/Window.h"
#include "Renderer.h"

/* TODO: increase performance:
    * too many calls to gl functions
    * in PointBuffer constructor and Insert/Remove point don't call glBufferSubData for every point, instead every point in an array and do a single call to glBufferSubData with that array
    * less gl calls: problem in PointBuffer constructor
        - two different attributes are being updated through glBufferSubData so there are two ways of generating a single array for a single call of glBufferSubData
        1. save the offset for the point nearer to the buffer start, collect float values of positions and colors in the same array, then call glBufferSubData
        2. collect position and color data in two different arrays and then call glBufferSubdata on each of them
        In this case data in the buffer should be arranged and the vertex specification should be done differently:
        store all the positions' values first and then all the colors' values right after

        Doing first impl.
        call glBufferSubData for every row of points because they are stored consecutively in the gl buffer

    * work on window resizing and consequently grid resizing    
*/

/* TODO:
    * make the brush size a function of the velocity of its movement (cursor movement)
    * implement color choice for the brush
*/

/* TODO: respond to window resizing
    * change grid dimensions accordingly
    * change each drawn point's position according to the scaling of the window (if size [area] is increasing)
    * the buffer in gpu is allocated with the window's initial size, so allocate another buffer and refill it with the point's new data
*/

/* TODO:
    * in this case screen coords map 1:1 with pixels, but implement grid using framebuffer size anyway  
*/

int main(void)
{
    // open an image, get its width and height and pass them to the Window class constructor

    Window* window;
    GLFWwindow* GLFWwin;
    char path[100] = "pictures\\drawing_0.png";
     
    try
    {
        window = new Window(path);
        GLFWwin = window->GetGLFWwindow();
    }
    catch (std::exception& e)
    {
        printf("%s\n", e.what());
        return -1;
    }

    Renderer* renderer = new Renderer(window->GetPointBuffer());

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(GLFWwin))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_PROGRAM_POINT_SIZE);
        
        window->Update();

        renderer->Draw();

        /* Swap front and back buffers */
        glfwSwapBuffers(GLFWwin);

        /* Poll for and process events */
        glfwPollEvents();
    }

    delete window;
    delete renderer;

    glfwTerminate();
    return 0;
}