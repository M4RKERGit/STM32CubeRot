#include <stdlib.h>
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_gyroscope.h"

#include "oglcube.h"
#include "framehandler.h"
#include "depth_sdram.h"
#include "Demo.h"
#include <stdio.h>
#include <math.h>

#define ROTANIM_DURATION_MS 1000
#define ROTANIM_CUBE_CNT 4

static Cube m_Cubes[ROTANIM_CUBE_CNT];
static const Camera* m_Camera;
static const vec3 x_turn = {1, 0, 0};
static const vec3 y_turn = {0, 1, 0};
static const vec3 z_turn = {0, 0, 1};
int xDiff;
int yDiff;
int zDiff;
int xOld;
int yOld;
int zOld;
uint32_t tickCounter;
int colorCounter;
static uint32_t m_lastTick;
static uint8_t m_isActive;
static int xAxis, yAxis, zAxis;
bool isShowing = 0;


static const uint32_t m_colors[CUBE_VERTEX_COUNT] =
{
		0xFFFF0000, // RED
		0xFF00FF00, // GREEN
		0xFF0000FF, // BLUE
		0xFF00FFFF, // CYAN
		0xFFFF00FF, // MAGENTA
		0xFFFFFF00, // YELLOW
		0xFFA52A2A, // BROWN
		0xFFFFA500  // ORANGE
};

static inline float GetRandomFloat()
{
	return ((float)rand()) / RAND_MAX;
}

static inline float GetRandomFloatRange(float a_min, float b_max)
{
	return a_min + (GetRandomFloat()) * (b_max - a_min);
}

static void GetRandomTranslation(vec3 tr)
{
	tr[0] = GetRandomFloatRange(-2.0f, 3.0f);
	tr[1] = GetRandomFloatRange(-1.5f, 0.0f);
	tr[2] = GetRandomFloatRange(-1.0f, 1.0f);
}

static void GetRandomRotation(quat rot)
{
	vec3 axis;
	axis[0] = GetRandomFloatRange(-1, 1);
	axis[1] = GetRandomFloatRange(-1, 1);
	axis[2] = GetRandomFloatRange(-1, 1);
	float angle_rads = GetRandomFloat() * M_PI;
	quat_rotate(rot, angle_rads, axis);
}

void CubeRotationAnim_Init(const Camera* camera)
{
	srand(1U);

	m_Camera = camera;
	m_isActive = 0;

	uint32_t i;

	for (i = 0; i < ROTANIM_CUBE_CNT; i++)
	{
		float scale = GetRandomFloatRange(0.1f, 0.2f);
		OpenGL_Cube_Init(&m_Cubes[i], scale);

		quat rot;
		GetRandomRotation(rot);
		OpenGL_Cube_RotateLocal(&m_Cubes[i], rot);

		vec3 tr;
		GetRandomTranslation(tr);
		OpenGL_Cube_TranslateVec3(&m_Cubes[i], tr);
	}

	m_lastTick = HAL_GetTick();
}

void CubeRotationAnim_Pause()
{
	m_isActive = 0;
}

char buf[64];
float data[3];
int x = 0;
int y = 0;
int z = 0;

void CubeRotationAnim_Resume()
{
	if (!m_isActive)
	{
		FrameHandler_Reset();
		m_isActive = 1;
	}
	quat q;
	uint32_t curr = HAL_GetTick();

	if (isShowing == 1)
	{
		snprintf(buf, sizeof buf, "400dpi");
		BSP_LCD_DisplayStringAt(0, 300,(uint8_t *)buf,LEFT_MODE);
		snprintf(buf, sizeof buf, "800");
		BSP_LCD_DisplayStringAt(0, 117,(uint8_t *)buf,LEFT_MODE);
		snprintf(buf, sizeof buf, "1600");
		BSP_LCD_DisplayStringAt(0, 170,(uint8_t *)buf,LEFT_MODE);
		snprintf(buf, sizeof buf, "2400");
		BSP_LCD_DisplayStringAt(180, 220,(uint8_t *)buf,LEFT_MODE);

		snprintf(buf, sizeof buf, "X %d", xAxis);
		BSP_LCD_DisplayStringAt(0,30,(uint8_t *)buf,LEFT_MODE);
		snprintf(buf, sizeof buf, "Y %d", yAxis);
		BSP_LCD_DisplayStringAt(0,50,(uint8_t *)buf,LEFT_MODE);
		snprintf(buf, sizeof buf, "Z %d", zAxis);
		BSP_LCD_DisplayStringAt(0,70,(uint8_t *)buf,LEFT_MODE);

		snprintf(buf, sizeof buf, "X %d", xDiff);
		BSP_LCD_DisplayStringAt(100,30,(uint8_t *)buf,LEFT_MODE);
		snprintf(buf, sizeof buf, "Y %d", yDiff);
		BSP_LCD_DisplayStringAt(100,50,(uint8_t *)buf,LEFT_MODE);
		snprintf(buf, sizeof buf, "Z %d", zDiff);
		BSP_LCD_DisplayStringAt(100,70,(uint8_t *)buf,LEFT_MODE);
	}
	BSP_GYRO_GetXYZ(data);

	xAxis += (int)data[0]/10000;
	yAxis += (int)data[1]/10000;
	zAxis += (int)data[2]/10000;

	xDiff = xAxis - xOld;
	yDiff = yAxis - yOld;
	zDiff = zAxis - zOld;

	float turned;
	float angle_rads;




	turned = ((float)xDiff) / ROTANIM_DURATION_MS;
	angle_rads = turned * M_PI * 10;

	quat_rotate(q, angle_rads, x_turn);		//поворот по x
	OpenGL_Cube_RotateLocal(&m_Cubes[0], q);

	quat_rotate(q, 2.0f * angle_rads, x_turn);
	OpenGL_Cube_RotateLocal(&m_Cubes[1], q);

	quat_rotate(q, 4.0f * angle_rads, x_turn);
	OpenGL_Cube_RotateLocal(&m_Cubes[2], q);

	quat_rotate(q, 6.0f * angle_rads, x_turn);
	OpenGL_Cube_RotateLocal(&m_Cubes[3], q);




	turned = ((float)yDiff) / ROTANIM_DURATION_MS;
	angle_rads = turned * M_PI * 10;

	quat_rotate(q, angle_rads, y_turn);		//поворот по y
	OpenGL_Cube_RotateLocal(&m_Cubes[0], q);

	quat_rotate(q, 2.0f * angle_rads, y_turn);
	OpenGL_Cube_RotateLocal(&m_Cubes[1], q);

	quat_rotate(q, 4.0f * angle_rads, y_turn);
	OpenGL_Cube_RotateLocal(&m_Cubes[2], q);

	quat_rotate(q, 6.0f * angle_rads, y_turn);
	OpenGL_Cube_RotateLocal(&m_Cubes[3], q);




	turned = ((float)zDiff) / ROTANIM_DURATION_MS;
	angle_rads = turned * M_PI * 10;

	quat_rotate(q, angle_rads, z_turn);		//поворот по z
	OpenGL_Cube_RotateLocal(&m_Cubes[0], q);

	quat_rotate(q, 2.0f * angle_rads, z_turn);
	OpenGL_Cube_RotateLocal(&m_Cubes[1], q);

	quat_rotate(q, 4.0f * angle_rads, z_turn);
	OpenGL_Cube_RotateLocal(&m_Cubes[2], q);

	quat_rotate(q, 6.0f * angle_rads, z_turn);
	OpenGL_Cube_RotateLocal(&m_Cubes[3], q);

	xOld = xAxis;
	yOld = yAxis;
	zOld = zAxis;

	for (uint8_t i = 0; i < ROTANIM_CUBE_CNT; i++)
	{
		FrameHandler_DrawCube(m_Camera, &m_Cubes[i]);
	}
	FrameHandler_glFlush();
	m_lastTick = curr;
}

void CubeRotationAnim_SetNextColor()
{
	if (colorCounter == 8)
			{
			for (uint8_t j = 0; j < ROTANIM_CUBE_CNT; j++)
			{
				for (uint8_t i = 0; i < 7; i++)
			{
				m_Cubes[j].colors[i] = m_colors[i];
			}
			}
				colorCounter = 0;
				return;
			}
	for (uint8_t j = 0; j < ROTANIM_CUBE_CNT; j++)
	{
		if (j == 0)
		{
			uint8_t buff = colorCounter;
			if (buff > 8)	buff = 0;
			for (uint8_t i = 0; i < 7; i++)
			{
				m_Cubes[j].colors[i] = m_colors[buff];
			}
		}
		if (j == 1)
		{
			uint8_t buff = colorCounter + 1;
			if (buff > 8)	buff = 1;
			for (uint8_t i = 0; i < 7; i++)
			{
				m_Cubes[j].colors[i] = m_colors[buff];
			}
		}
		if (j == 2)
		{
			uint8_t buff = colorCounter + 2;
			if (buff > 8)	buff = 2;
			for (uint8_t i = 0; i < 7; i++)
			{
				m_Cubes[j].colors[i] = m_colors[buff];
			}
		}
		if (j == 3)
		{
			uint8_t buff = colorCounter + 3;
			if (buff > 8)	buff = 3;
			for (uint8_t i = 0; i < 7; i++)
			{
				m_Cubes[j].colors[i] = m_colors[buff];
			}
		}
	}
	colorCounter ++;
}
void CubeRotationAnim_ShowStat()
{
	isShowing = !isShowing;
}
