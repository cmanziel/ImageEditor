#include "PointBuffer.h"

// PointBuffer receives the image's scanlines (with filter byte at the start of every row) sotred in a single array
// give every grid point the image's pixel's rgb values, unless image_data == NULL, then just have a white background
PointBuffer::PointBuffer(Brush* brush, int width, int height, unsigned char* image_data)
	: m_Brush(brush), m_WinWidth(width), m_WinHeight(height)
{
	glGenBuffers(1, &m_ID);
	Bind();

	m_ImageData = image_data;
	m_Grid = (point**)malloc(sizeof(point*) * m_WinHeight);

	if (m_Grid == NULL)
	{
		printf("points grid not allocated\n");
		return;
	}

	bool default_background = false; // if the image_data is NULL (problem opening, corrupted comprressed data during decompression or other errors) then sset default_baackground to true
	if (image_data == NULL)
		default_background = true;

	//unsigned int bufSize = (m_WinWidth / m_PointSize) * (m_WinHeight / m_PointSize) * (COORDS_PER_POINT * sizeof(float) + sizeof(GLboolean)); // one bool for the flag to decide if point should be drawn
	// bufSize = m_WinHeight * m_WinWidth * (2 values for position + 3 values for rgb values + 1 value for drawn flag)
	unsigned int bufSize = m_WinHeight * m_WinWidth * (VALUES_PER_POINT * sizeof(float));

	glBufferData(GL_ARRAY_BUFFER, bufSize, NULL, GL_DYNAMIC_DRAW);

	// a grid cell now is 1px x 1px, so there's no need to calcualte it's centre and area to check if the cursor's inside of it. just use the integer screen coordinates returned from the glfwGetCursorPos to represent grid cell coordinates
	for (int y = 0; y < m_WinHeight; y++)
	{
		m_Grid[y] = (point*)malloc(sizeof(point) * m_WinWidth);

		if (m_Grid[y] == NULL)
		{
			printf("row not allocated\n");
			return;
		}

		for (int x = 0; x < m_WinWidth; x++)
		{
			point* p = &m_Grid[y][x];

			float glX, glY;

			glX = (2.0 * x / m_WinWidth) - 1.0; // 2.0 is represented as a floating point value otherwise the division would be integer and always truncated to 0 for values of x and y < m_WinWidth and m_WinHeight
			glY = (2.0 * y / m_WinHeight) - 1.0;

			//flip the y axis
			glY = -glY;

			p->x = glX;
			p->y = glY;
			p->drawn = false;

			color pix_color;
			if (default_background)
			{
				pix_color.r = 255;
				pix_color.g = 255;
				pix_color.b = 255;
			}
			else
			{
				// retrieve the pixel data at this point in the grid from the pixel data array. the first y + 1 = 1 byte filter * current row number
				uLong index = (y + 1) + y * (m_WinWidth * CHANNELS_PER_PIXEL) + (x * CHANNELS_PER_PIXEL);
				pix_color.r = image_data[index];
				pix_color.g = image_data[index + 1];
				pix_color.b = image_data[index + 2];
			}

			p->p_color = pix_color;

			//p->offset = (y * grid_columns + x) * (COORDS_PER_POINT * sizeof(float) + sizeof(GLboolean));
			p->offset = (y * m_WinWidth + x) * (VALUES_PER_POINT * sizeof(float));

			float pointData[VALUES_PER_POINT];

			pointData[0] = p->x;
			pointData[1] = p->y;

			pointData[2] = (float)p->p_color.r;
			pointData[3] = (float)p->p_color.g;
			pointData[4] = (float)p->p_color.b;

			glBufferSubData(GL_ARRAY_BUFFER, p->offset, VALUES_PER_POINT * sizeof(float), pointData); // buffer every point to the gpu
		}
	}
}

PointBuffer::~PointBuffer()
{
	for (int y = 0; y < m_WinHeight; y++)
	{
		// free the rows
		free(m_Grid[y]);
	}

	// free the "column of pointers"
	free(m_Grid);

	if(m_ImageData != NULL) // m_ImageData is null if no image was provided or wasn't opened, in this case default white background was used
		free(m_ImageData); // m_ImageData points to data allocated in the Window constructor where the image's opened

	glDeleteBuffers(1, &m_ID);
}

unsigned int PointBuffer::GetID() {
	return m_ID;
}

int PointBuffer::GetWinWidth()
{
	return m_WinWidth;
}

int PointBuffer::GetWinHeight()
{
	return m_WinHeight;
}

void PointBuffer::SetWindowDims(int width, int height)
{
	m_WinWidth = width;
	m_WinHeight = height;
}

void PointBuffer::Bind() {
	glBindBuffer(GL_ARRAY_BUFFER, m_ID);
}

void PointBuffer::Unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

point** PointBuffer::GetGrid() {
	return m_Grid;
}

Brush* PointBuffer::GetBrush()
{
	return m_Brush;
}

void PointBuffer::InsertPoint()
{
	// if these two variables' type is unsigned int then when grid_cell_y - BRUSH_RADIUS is < 0 the number can't be represented by unsigned int
	// either store grid_cell_y and grid_cell_x as ints or check if grid_cell_y is between 0 and BRUSH_RADIUS
	int grid_cell_y = m_Brush->GetPosition().y;
	int grid_cell_x = m_Brush->GetPosition().x;

	// boundary for the brush grid around the cursor, 
	int x_left_boundary, x_right_boundary, y_top_boundary, y_bottom_boundary;
	int brush_radius = m_Brush->GetRadius();

	x_left_boundary = grid_cell_x < brush_radius ? 0 : grid_cell_x - brush_radius;
	x_right_boundary = grid_cell_x >= m_WinWidth - brush_radius ? m_WinWidth - 1 : grid_cell_x + brush_radius;

	y_top_boundary = grid_cell_y < brush_radius ? 0 : grid_cell_y - brush_radius;
	y_bottom_boundary = grid_cell_y >= m_WinHeight - brush_radius ? m_WinHeight - 1 : grid_cell_y + brush_radius;

	for (int y = y_top_boundary; y <= y_bottom_boundary; y++)
	{
		for (int x = x_left_boundary; x <= x_right_boundary; x++)
		{
			if (IsPointDrawn(x, y))
				continue; // or do the following things under a not condition

			// check if the brush's grid's point inside the circle of brush_radius around the cursor
			if (!m_Brush->IsPointInside(x, y))
				continue;

			point* p = &m_Grid[y][x];

			p->p_color = m_Brush->GetColor();

			float col[CHANNELS_PER_PIXEL];

			col[0] = (float)p->p_color.r;
			col[1] = (float)p->p_color.g;
			col[2] = (float)p->p_color.b;

			p->drawn = true;

			glBufferSubData(GL_ARRAY_BUFFER, p->offset + COORDS_PER_POINT * sizeof(float), CHANNELS_PER_PIXEL * sizeof(float), (void*)col);
		}
	}
}

void PointBuffer::RemovePoint()
{
	int grid_cell_y = m_Brush->GetPosition().y;
	int grid_cell_x = m_Brush->GetPosition().x;

	// calculate the coordinates for the brush's grid edges
	int x_left_boundary, x_right_boundary, y_top_boundary, y_bottom_boundary;
	int brush_radius = m_Brush->GetRadius();

	x_left_boundary = grid_cell_x < brush_radius ? 0 : grid_cell_x - brush_radius;
	x_right_boundary = grid_cell_x >= m_WinWidth - brush_radius ? m_WinWidth - 1 : grid_cell_x + brush_radius;

	y_top_boundary = grid_cell_y < brush_radius ? 0 : grid_cell_y - brush_radius;
	y_bottom_boundary = grid_cell_y >= m_WinHeight - brush_radius ? m_WinHeight - 1 : grid_cell_y + brush_radius;

	// loop through the points aorund the brush, the brush grid is the collection of winwow grid points around the cursor
	for (int y = y_top_boundary; y <= y_bottom_boundary; y++)
	{
		for (int x = x_left_boundary; x <= x_right_boundary; x++)
		{
			if (!IsPointDrawn(x, y))
				continue; // or do the following things under a not condition

			if (!m_Brush->IsPointInside(x, y))
				continue;

			point* p = &m_Grid[y][x];

			// calculate index in the m_ImageData buffer, set p->p_color to the color of the image's pixel, when a point is removed
			uLong index = (y + 1) + y * (m_WinWidth * CHANNELS_PER_PIXEL) + (x * CHANNELS_PER_PIXEL);

			p->p_color.r = m_ImageData != NULL ? m_ImageData[index] : 255;
			p->p_color.g = m_ImageData != NULL ? m_ImageData[index + 1] : 255;
			p->p_color.b = m_ImageData != NULL ? m_ImageData[index + 2] : 255;

			float col[CHANNELS_PER_PIXEL];
			col[0] = p->p_color.r;
			col[1] = p->p_color.g;
			col[2] = p->p_color.b;

			p->drawn = false;

			glBufferSubData(GL_ARRAY_BUFFER, p->offset + COORDS_PER_POINT * sizeof(float), CHANNELS_PER_PIXEL * sizeof(float), (void*)col);
		}
	}
}

bool PointBuffer::IsPointDrawn(int grid_cell_x, int grid_cell_y)
{
	return m_Grid[grid_cell_y][grid_cell_x].drawn;
}

void PointBuffer::Reset()
{

}