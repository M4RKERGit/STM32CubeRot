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
#define ROTANIM_CUBE_CNT 1

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
static uint32_t m_lastTick;
static uint8_t m_isActive;
static int xAxis, yAxis, zAxis;

static inline float GetRandomFloat() {
	return ((float)rand()) / RAND_MAX;
}

static inline float GetRandomFloatRange(float a_min, float b_max) {
	return a_min + (GetRandomFloat()) * (b_max - a_min);
}

static void GetRandomTranslation(vec3 tr) {
	tr[0] = GetRandomFloatRange(0, 0);
	tr[1] = GetRandomFloatRange(-0.5f, 0.5f);
	tr[2] = GetRandomFloatRange(0, 0);
}

static void GetRandomRotation(quat rot) {
	vec3 axis;
	axis[0] = GetRandomFloatRange(-1, 1);
	axis[1] = GetRandomFloatRange(-1, 1);
	axis[2] = GetRandomFloatRange(-1, 1);
	float angle_rads = GetRandomFloat() * M_PI;
	quat_rotate(rot, angle_rads, axis);
}

void CubeRotationAnim_Init(const Camera* camera) {
	srand(1U);

	m_Camera = camera;
	m_isActive = 0;

	uint32_t i;
	for (i = 0; i < ROTANIM_CUBE_CNT; ++i) {
		float scale = GetRandomFloatRange(0.1f, 0.8f);
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

void CubeRotationAnim_Pause() {
	m_isActive = 0;
}
char buf[64];
char buf1[64];
char buf2[64];
float data[3];
int x = 0;
int y = 0;
int z = 0;


void CubeRotationAnim_Resume() {
	if (!m_isActive) {
		FrameHandler_Reset();
		m_isActive = 1;
	}
	quat q;
	uint32_t curr = HAL_GetTick();

	BSP_GYRO_GetXYZ(data);

	xAxis += (int)data[0]/10000;
	yAxis += (int)data[1]/10000;
	zAxis += (int)data[2]/10000;

	snprintf(buf, sizeof buf, "X %d", xAxis);
    snprintf(buf1, sizeof buf1, "Y %d", yAxis);
	snprintf(buf2, sizeof buf2, "Z %d", zAxis);

	BSP_LCD_DisplayStringAt(0,30,(uint8_t *)buf,LEFT_MODE);
	BSP_LCD_DisplayStringAt(0,50,(uint8_t *)buf1,LEFT_MODE);
	BSP_LCD_DisplayStringAt(0,70,(uint8_t *)buf2,LEFT_MODE);

	xDiff = xAxis - xOld;
	yDiff = yAxis - yOld;
	zDiff = zAxis - zOld;

	snprintf(buf, sizeof buf, "X %d", xDiff);
    snprintf(buf1, sizeof buf1, "Y %d", yDiff);
	snprintf(buf2, sizeof buf2, "Z %d", zDiff);

	BSP_LCD_DisplayStringAt(100,30,(uint8_t *)buf,LEFT_MODE);
	BSP_LCD_DisplayStringAt(100,50,(uint8_t *)buf1,LEFT_MODE);
	BSP_LCD_DisplayStringAt(100,70,(uint8_t *)buf2,LEFT_MODE);

	float turned;
	float angle_rads;


	if (xDiff > 1 | xDiff < 1)
	{
		turned = ((float)xDiff) / ROTANIM_DURATION_MS;
		angle_rads = turned * M_PI * 10;
		quat_rotate(q, angle_rads, x_turn);

		OpenGL_Cube_RotateLocal(&m_Cubes[0], q);
	}


	if (yDiff  > 1 | yDiff < 1)
	{
		turned = ((float)yDiff) / ROTANIM_DURATION_MS;
		angle_rads = turned * M_PI * 10;
		quat_rotate(q, angle_rads, y_turn);

		OpenGL_Cube_RotateLocal(&m_Cubes[0], q);
	}

	if (zDiff  > 1 | zDiff < 1)
	{
		turned = ((float)zDiff) / ROTANIM_DURATION_MS;
		angle_rads = turned * M_PI * 10;
		quat_rotate(q, angle_rads, z_turn);

		OpenGL_Cube_RotateLocal(&m_Cubes[0], q);
	}


	FrameHandler_DrawCube(m_Camera, &m_Cubes[0]);
	FrameHandler_glFlush();
	m_lastTick = curr;
	xOld = xAxis;
	yOld = yAxis;
	zOld = zAxis;
}
