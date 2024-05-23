#include <stdio.h>
#include <stdlib.h>

#include "Window/Window.h"
#include "Renderer.h"

/* TODO: increase performance:
    * work on window resizing and consequently grid resizing
    
    * maybe use a list for a one dimensional collection of points
    * better sorting algorithm for checking if point has already been drawn
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
    const char path[100] = "pictures\\drawing_0.png";

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