#include "Brush.h"

float vector_length(cursor v);

Brush::Brush(double cursor_x, double cursor_y, long time)
	: m_State(STATE_INACTIVE)
{
	m_Cursor.x = cursor_x;
	m_Cursor.y = cursor_y;

	m_DeltaAreaCenter = m_Cursor;
	m_DeltaAreaRadius = DELTA_AREA_RADIUS;
	m_DeltaStartTime = time;

	m_Velocity = 0;

	m_LastVelocity = m_Velocity;

	m_Radius = 0;

	//start with red color
	m_Color.r = 0;
	m_Color.g = 0;
	m_Color.b = 255;
}

Brush::~Brush()
{

}

cursor Brush::GetPosition()
{
	return m_Cursor;
}

void Brush::SetCursorPos(double xPos, double yPos, long time)
{
	m_Cursor.x = xPos;
	m_Cursor.y = yPos;

	cursor deltaPos;
	deltaPos.x = m_Cursor.x - m_DeltaAreaCenter.x;
	deltaPos.y = m_Cursor.y - m_DeltaAreaCenter.y;

	if(vector_length(deltaPos) > m_DeltaAreaRadius)
	{
		// set new delta area center, calculate velocity
		long timeElapsed = time - m_DeltaStartTime;

		m_Velocity = vector_length(deltaPos) / timeElapsed;

		SetRadius();

		m_DeltaAreaCenter = m_Cursor;
		m_DeltaStartTime = time;
		m_LastVelocity = m_Velocity;
	}
}

int Brush::GetRadius()
{
	return m_Radius;
}

//void Brush::SetRadius(int mod)
//{
//	m_Radius = m_Radius == 0 && mod < 0 ? m_Radius : m_Radius + mod;
//}

void Brush::SetRadius()
{
	//float delta_velocity = vector_length(m_Velocity) - vector_length(m_LastVelocity);
	//float delta_velocity = m_Velocity - m_LastVelocity;

	//int scaling_factor = delta_velocity * 5 > 50 ? 50 : delta_velocity * 5;

	//m_Radius = m_Radius + scaling_factor < 0 ? 0 : m_Radius + scaling_factor;

	m_Radius = m_Velocity * 20 > 150 ? 150 : m_Velocity * 20;
}

color Brush::GetColor()
{
	return m_Color;
}

bool Brush::IsPointInside(int grid_point_x, int grid_point_y)
{
	// the cursor is the brush's grid's center, the argument is a point of the grid that needs to be evaluated if it's inside the brush's radius
	// cursor position in the window grid coordinates
	int grid_centre_y = m_Cursor.y;
	int grid_centre_x = m_Cursor.x;

	// point coordinates relative to the brush's position
	float brush_grid_x = grid_point_x - grid_centre_x;
	float brush_grid_y = grid_point_y - grid_centre_y;

	if (sqrt(brush_grid_x * brush_grid_x + brush_grid_y * brush_grid_y) <= m_Radius)
		return true;

	return false;
}

void Brush::ChangeState(uint8_t state)
{
	// to this function are passed either STATE_DRAW or STATE_ERASE, if the current state is equal to the argument then the action was to stop drawing or erasing
	if (state == m_State)
		m_State = STATE_INACTIVE;
	else
		m_State = state;
}

uint8_t Brush::GetState()
{
	return m_State;
}

float vector_length(cursor v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}