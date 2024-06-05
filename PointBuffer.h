#pragma once

#include <vector>
//#include <windows.h>
//#include <winuser.h>
#include <GL/glew.h>

#include "Brush/Brush.h"
#include "pnglib.h"


#define VALUES_PER_POINT 5 // 2 values for coordinates, 3 for color values
#define COORDS_PER_POINT 2
#define GRID_CELL_SIZE 1

typedef struct point {
	float x;
	float y;
	bool drawn;
	unsigned int offset; // offset in the buffer allocated in the gpu
	color p_color;
};

class PointBuffer
{
private:
	//std::vector<coord*> m_Points;
	point** m_Grid; // bidimensional array
	unsigned char* m_ImageData;

	Brush* m_Brush;
	unsigned int m_ID;
	unsigned int m_WinWidth;
	unsigned int m_WinHeight;

public:
	PointBuffer(Brush* brush, int width, int height, unsigned char* image_data);
	~PointBuffer();

	unsigned int GetID();
	void Bind();
	void Unbind();

	point** GetGrid();
	Brush* GetBrush();

	int GetWinWidth();
	int GetWinHeight();
	void SetWindowDims(int width, int height);

	void InsertPoint();
	void RemovePoint();
	
	bool IsPointDrawn(int cell_x, int cell_y);
	void Reset();
};