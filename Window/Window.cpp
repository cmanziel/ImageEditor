#include "Window.h"

Window::Window(char* path)
    : m_fps(120.0), m_Path(path), m_State(STATE_INACTIVE)
{
    // open the image file at "path", extract the compressed data from the idat chunk
// pass the idat_data to the decompression function, pass the decomoreesed data to PointBuffer

    m_Image = fopen(path, "rb");
    unsigned char* idat_data, * image_pixel_data;

    if (m_Image == NULL)
    {
        idat_data = NULL;
        image_pixel_data = NULL;
        m_Width = 2000;
        m_Height = 1000;
    }
    else
    {
        m_Width = image_get_width(m_Image);
        m_Height = image_get_height(m_Image);

        image_pixel_data = decompress_image(m_Image);
    }

    /* Initialize the library */
    if (!glfwInit())
        throw std::runtime_error("error initializing glfw");

    /* Create a windowed mode window and its OpenGL context */
    //window = glfwCreateWindow(640, 480, "Mouse Tracker", NULL, NULL);
    m_GLFWwindow = glfwCreateWindow(m_Width, m_Height, "Mouse Tracker", NULL, NULL);
    if (!m_GLFWwindow)
    {
        glfwTerminate();
        throw std::runtime_error("error creating window\n");
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(m_GLFWwindow);

    glfwSetWindowUserPointer(m_GLFWwindow, this);
    glfwSetKeyCallback(m_GLFWwindow, key_callback);
    //glfwSetInputMode(m_GLFWwindow, GLFW_STICKY_KEYS, GLFW_TRUE);

    if (glewInit() != GLEW_OK)
    {
        throw std::runtime_error("error initializing glew\n");
    }

    double xPos, yPos;
    glfwGetCursorPos(m_GLFWwindow, &xPos, &yPos);

    // pass to the PointBuffer the pixel_data_size that was just uncompressed
    // if the decomp function returns null then just have a white background

    m_Brush = new Brush(xPos, yPos, m_Timer.GetTimeMilliseconds());
    m_PointBuffer = new PointBuffer(m_Brush, m_Width, m_Height, image_pixel_data);

    if(m_Image != NULL)
        fclose(m_Image);
}

Window::~Window()
{
    delete m_Brush;
    delete m_PointBuffer;

    if(m_Image != NULL)
        fclose(m_Image);
}

void Window::Update()
{
    // update only every 1.0 / 60.0 seconds
    m_Timer.Update();

    long time = m_Timer.GetTimeMilliseconds();

    // lock window input handling and point insertion/removal to 60fps
    //if (time - m_Timer.m_LastTime < 1000.0 / m_fps)
    //    return;

    WindowResize();
    CursorMovement();
    //KeyInput();

    if (m_State == STATE_CURSOR_OUTSIDE)
        return;

    switch (m_Brush->GetState())
    {
        case STATE_DRAW:
        {
            m_PointBuffer->InsertPoint();
        } break;
    
        case STATE_ERASE:
        {
            m_PointBuffer->RemovePoint();
        } break;
    }

    printf("fps: %f\n", 1000.0 / (time - m_Timer.m_LastTime));

    m_Timer.m_LastTime = time;
}

//void Window::KeyInput()
//{
//    int key_D = glfwGetKey(m_GLFWwindow, GLFW_KEY_D);
//
//    if (key_D == GLFW_PRESS)
//        m_Brush->ChangeState(STATE_DRAW);
//
//    int key_E = glfwGetKey(m_GLFWwindow, GLFW_KEY_E);
//
//    if (key_E == GLFW_PRESS)
//        m_Brush->ChangeState(STATE_ERASE);
//
//    /*
//    if (glfwGetKey(m_GLFWwindow, GLFW_KEY_P) == GLFW_PRESS)
//        m_BrushState = STATE_SNAPSHOT;
//        */
//}

void Window::KeyCallback(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_D)
            m_Brush->ChangeState(STATE_DRAW);
        else if (key == GLFW_KEY_E)
            m_Brush->ChangeState(STATE_ERASE);
        else if (key == GLFW_KEY_UP)
            m_Brush->SetRadius(+1);
        else if (key == GLFW_KEY_DOWN)
            m_Brush->SetRadius(-1);
        else if (key == GLFW_KEY_P)
            TakeSnapshot();
        else if (key == GLFW_KEY_V)
            m_Brush->ToggleSpeedMode();
    }
}

void Window::WindowResize()
{
    glfwGetWindowSize(m_GLFWwindow, (int*)&m_Width, (int*)&m_Height);

    if (m_Height != m_PointBuffer->GetWinHeight() || m_Width != m_PointBuffer->GetWinWidth())
        m_PointBuffer->SetWindowDims(m_Width, m_Height);
}

void Window::CursorMovement()
{
    long time = m_Timer.GetTimeMilliseconds();

    double xPos, yPos;
    glfwGetCursorPos(m_GLFWwindow, &xPos, &yPos);

    m_Brush->SetCursorPos(xPos, yPos, time);

    if (xPos < 0 || yPos < 0 || (int)xPos >= m_Width || (int)yPos >= m_Height)
        m_State = STATE_CURSOR_OUTSIDE;
    else
        m_State = STATE_CURSOR_INSIDE;
}


/* long ticks = m_Timer->GetPerformaceCounter();
* long frequency = m_Timer->GetPerformanceFrequency();
ResizeBrush(m_Brush->GetPosition(), ticks, frequency);
*/

/* void ResizeBrush(cursor pos)
* {
*   unsigned int time = m_Timer.GetPerformanceCounter() / m_Timer.GetPerformanceFrequency();
* 
*   
*   m_Brush->SetCursorPos(pox.x, pos.y, time);
* }
*/


GLFWwindow* Window::GetGLFWwindow()
{
    return m_GLFWwindow;
}

unsigned int Window::GetWidth()
{
	return m_Width;
}

unsigned int Window::GetHeight()
{
	return m_Height;
}

PointBuffer* Window::GetPointBuffer()
{
    return m_PointBuffer;
}

// see PathTracer project for explanation
void Window::TakeSnapshot()
{
    unsigned int row_size = CHANNELS_PER_PIXEL * m_Width + 1;
    unsigned int pixel_data_size = row_size * m_Height * sizeof(unsigned char);

    unsigned char* pixel_data = (unsigned char*)malloc(pixel_data_size);

    if (pixel_data == NULL)
    {
        printf("image's pixel_data not allocated");
        return;
    }

    for (unsigned int y = 0; y < m_Height; y++)
    {
        unsigned int row_start = y * row_size;

        unsigned char filter_method = 0x00;

        pixel_data[row_start] = filter_method;

        for (unsigned int x = 0; x < m_Width; x++)
        {
            point p = m_PointBuffer->GetGrid()[y][x];

            unsigned int index = row_start + CHANNELS_PER_PIXEL * x + 1;

            pixel_data[index] = p.p_color.r; index++;
            pixel_data[index] = p.p_color.g; index++;
            pixel_data[index] = p.p_color.b; index++;
        }
    }

    // edit image path appending _edited to its name
    strcpy(m_Path + strlen(m_Path) - strlen(".png"), "\0");

    const char* path = strcat(m_Path, "_edited.png");

    create_image(pixel_data, path, m_Width, m_Height);

    free(pixel_data);
}