#pragma once

#include <stdint.h>
#include <math.h>
#include <stdio.h>

#define CHANNELS_PER_PIXEL 3

typedef struct
{
	double x;
	double y;
} cursor;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} color;

enum brush_state
{
	STATE_DRAW, STATE_ERASE, STATE_INACTIVE
};

#define DELTA_AREA_RADIUS 75
// could also be static class
class Brush
{
public:
	Brush(double cursor_x, double cursor_y, long time);
	~Brush();

	cursor GetPosition();
	void SetCursorPos(double xPos, double yPos, long time);

	void ChangeState(uint8_t state);
	uint8_t GetState();

	int GetRadius();
	//void SetRadius(int mod);

	// delta milliseconds as argument
	void SetRadius();

	color GetColor();

	bool IsPointInside(int grid_point_x, int grid_point_y);

private:
	int m_Radius;

	cursor m_Cursor;

	cursor m_DeltaAreaCenter;
	int m_DeltaAreaRadius;
	long m_DeltaStartTime; // time in milliseconds when the DeltaArea was escaped

	//cursor m_Velocity;
	//cursor m_LastVelocity;

	float m_Velocity;
	float m_LastVelocity;

	uint8_t m_State;
	color m_Color;
};

