#include "Renderer.h"
#include "PointBuffer.h"

Renderer::Renderer(PointBuffer* pb)
	: m_PointBuffer(pb)
{
	m_Shader = new Shader("shaders/shader.vs", "shaders/shader.fs");
	m_Shader->CreateProgram();
}

void Renderer::Draw()
{
	m_Shader->use();

	GLint pos_index = glGetAttribLocation(m_Shader->get_id(), "pos");
	glEnableVertexAttribArray(pos_index);

	// pass pairs of float values to the shaders
	glVertexAttribPointer(pos_index, COORDS_PER_POINT, GL_FLOAT, GL_FALSE, VALUES_PER_POINT * sizeof(float), (void*)0);

	GLint color_index = glGetAttribLocation(m_Shader->get_id(), "point_color");
	glEnableVertexAttribArray(color_index);
	// offset (last argument): COORDS_PER_POINT + 1 value for drawn flag, multiplied for the sizeof(float)
	glVertexAttribPointer(color_index, CHANNELS_PER_PIXEL, GL_FLOAT, GL_FALSE, VALUES_PER_POINT * sizeof(float), (void*)(COORDS_PER_POINT * sizeof(float)));

	int location = glGetUniformLocation(m_Shader->get_id(), "point_size");
	glUniform1f(location, GRID_CELL_SIZE);

	color brush_color = m_PointBuffer->GetBrush()->GetColor();
	location = glGetUniformLocation(m_Shader->get_id(), "point_color");
	glUniform3f(location, brush_color.r, brush_color.g, brush_color.b);

	glDrawArrays(GL_POINTS, 0, m_PointBuffer->GetWinWidth() * m_PointBuffer->GetWinHeight());
}

Renderer::~Renderer() {
	delete m_Shader;
}