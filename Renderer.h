#pragma once
#include "PointBuffer.h"
#include "Shader.h"

class Renderer {
public:
	Renderer(PointBuffer* pb);
	~Renderer();

	void Draw();

private:
	Shader* m_Shader;
	PointBuffer* m_PointBuffer;
};