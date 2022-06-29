#pragma once

#include "../../SDK/SDK.h"

class c_item
{
public:
	c_item(float* var, const char* const name, const float min, const float max, const float step, const int index)
	{
		m_value = var;
		m_name = name;
		m_min = min;
		m_max = max;
		m_step = step;
		m_index = index;
	}
public:
	__inline void move_left()
	{
		if (*m_value > m_min)
			*m_value -= m_step;
	}

	__inline void move_right()
	{
		if (*m_value < m_max)
			*m_value += m_step;
	}

	__inline const char* get_name() const { return m_name; }
	__inline float get_value()      const { return *m_value; }
	__inline int get_index()        const { return m_index; }
private:
	float* m_value;

	float m_step;

	float m_min;
	float m_max;

	int m_index;

	const char* m_name;
};

class c_menu
{
public:
	void initialize();
	void draw_fps();

private:
	void add_item(float* var, const char* const name, const float min, const float max, const float step);

private:

	EFonts m_font = EFonts::MENU_TAHOMA;

	float m_draw_y = 0;

	float m_draw_x = 0;

	int m_cur_item = 0;

	std::vector<std::unique_ptr<c_item>> m_items = { };
};

namespace F { inline c_menu menu; }