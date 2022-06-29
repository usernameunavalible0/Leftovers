#include "Menu.h"
#include "../Vars.h"

#define item_h 15;

void c_menu::initialize()
{
	add_item(reinterpret_cast<float*>(Vars::Trigger::Active), "triggerbot_enabled", 0.0f, 1.0f, 1.0f);
	add_item(reinterpret_cast<float*>(Vars::Trigger::Hitbox), "triggerbot_hitbox", 0.0f, 1.0f, 0.0f);
	add_item(reinterpret_cast<float*>(Vars::Trigger::AutoBackstab), "Autobackstab", 0.0f, 1.0f, 1.0f);

	add_item(reinterpret_cast<float*>(Vars::Misc::AutoStrafe), "Autostrafe", 0.0f, 1.0f, 1.0f);
	add_item(reinterpret_cast<float*>(Vars::Misc::Bunnyhop), "Bunnyhop", 0.0f, 1.0f, 1.0f);
	add_item(reinterpret_cast<float*>(Vars::Misc::BypassPure), "Bypass Pure", 0.0f, 1.0f, 1.0f);

}

void c_menu::draw_fps()
{
	static bool menu_open = false;

	if ((GetAsyncKeyState(VK_INSERT) & 0x01))
	{
		menu_open = !menu_open;
	}

	if (menu_open)
	{
		static const int total_items = (static_cast<int>(m_items.size() - 1));

		m_font = EFonts::MENU_TAHOMA;
		m_draw_y = 280.0f;

		G::Draw.Rect(280.0f, 280.0f, 165.0f, 5.0f, { 225, 225, 225, 225 });

		for (const auto& item : m_items)
		{
			Color clr = (item->get_index() == m_cur_item) ? Color(1.0f, 0.3f, 0.0f, 1.0f) : Color(1.0f, 1.0f, 1.0f, 1.0f);

		}

		if ((GetAsyncKeyState(VK_UP) & 0x01) && (m_cur_item > 0))
			m_cur_item--;

		if ((GetAsyncKeyState(VK_DOWN) & 0x01) && (m_cur_item < total_items))
			m_cur_item++;

		if ((GetAsyncKeyState(VK_LEFT) & 0x01))
			m_items[m_cur_item]->move_left();

		if ((GetAsyncKeyState(VK_RIGHT) & 0x01))
			m_items[m_cur_item]->move_right();
	}
}

void c_menu::add_item(float* var, const char* const name, const float min, const float max, const float step)
{
	static int index = 0;

	m_items.push_back(std::make_unique<c_item>(var, name, min, max, step, index));

	index++;
}