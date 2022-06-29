#include "Menu.h"

#include "InputHelper/InputHelper.h"
//#include "ConfigManager/ConfigManager.h"

void CMenu::DrawTooltip()
{
	if (m_szCurTip.empty() || !Vars::Menu::ToolTips.m_Var)
		return;

	int w, h;
	I::VGuiSurface->GetTextSize(G::Draw.m_Fonts.Find(EFonts::MENU_TAHOMA), m_szCurTip.data(), w, h);

	G::Draw.OutlinedRect(g_InputHelper.m_nMouseX + 13, g_InputHelper.m_nMouseY + 13, w + 4, h + 4, { 0, 0, 0, 255 });
	G::Draw.Rect(g_InputHelper.m_nMouseX + 14, g_InputHelper.m_nMouseY + 14, w + 2, h + 2, Vars::Menu::Colors::WindowBackground);
	G::Draw.String(EFonts::MENU_TAHOMA, g_InputHelper.m_nMouseX + 15, g_InputHelper.m_nMouseY + 15, Vars::Menu::Colors::Text, TXT_CENTERXY, m_szCurTip.data());
}

void CMenu::Separator()
{
	int x = m_LastWidget.x + m_LastWidget.width + Vars::Menu::SpacingX;
	int y = Vars::Menu::Position.y;
	int w = 0;
	int h = 0;

	G::Draw.Line(x, y, x, (y + Vars::Menu::Position.height - 1), Vars::Menu::Colors::OutlineMenu);

	m_LastWidget.x = x + Vars::Menu::SpacingX;
	m_LastWidget.y = y;
	m_LastWidget.width = w;
	m_LastWidget.height = h;
}

bool CMenu::CheckBox(CVar<bool>& Var, const wchar_t* const szToolTip)
{
	bool callback = false;

	int x = m_LastWidget.x;
	int y = m_LastWidget.y + m_LastWidget.height + Vars::Menu::SpacingY;
	int w = Vars::Menu::CheckBoxW;
	int h = Vars::Menu::CheckBoxH;

	if (g_InputHelper.m_nMouseX > x && g_InputHelper.m_nMouseX < x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
	{
		if (g_InputHelper.IsPressed(VK_LBUTTON)) {
			callback = true;
			Var.m_Var = !Var.m_Var;
		}

		G::Draw.Rect(x, y, w, h, Vars::Menu::Colors::WidgetActive);
		m_szCurTip += szToolTip;
	}

	if (Var.m_Var)
	{
		G::Draw.GradientRect(
			x,
			y,
			x + w,
			y + h,
			Vars::Menu::Colors::Widget, Vars::Menu::Colors::WidgetActive,
			false);
	}

	G::Draw.OutlinedRect(x, y, w, h, Vars::Menu::Colors::OutlineMenu);
	G::Draw.String(EFonts::MENU_TAHOMA, x + w + Vars::Menu::SpacingText, y + (h / 2), Vars::Menu::Colors::Text, TXT_CENTERY, Var.m_szDisplayName);

	m_LastWidget.x = x;
	m_LastWidget.y = y;
	m_LastWidget.width = w;
	m_LastWidget.height = h;

	return callback;
}

bool CMenu::Button(const wchar_t* Label, bool Active, int WidthOverride, int HeightOverride)
{
	bool callback = false;

	int x = m_LastWidget.x;
	int y = m_LastWidget.y + m_LastWidget.height + Vars::Menu::SpacingY;
	int w = WidthOverride ? WidthOverride : Vars::Menu::ButtonW;
	int h = HeightOverride ? HeightOverride : Vars::Menu::ButtonH;

	if (g_InputHelper.m_nMouseX > x && g_InputHelper.m_nMouseX < x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
	{
		if (g_InputHelper.IsPressed(VK_LBUTTON))
			callback = true;

		G::Draw.Rect(x, y, w, h, Vars::Menu::Colors::WidgetActive);
	}

	if (Active)
		G::Draw.GradientRect(x, y, x + w, y + h, Vars::Menu::Colors::Widget, Vars::Menu::Colors::WidgetActive, false);

	G::Draw.OutlinedRect(x, y, w, h, Vars::Menu::Colors::OutlineMenu);
	G::Draw.String(EFonts::MENU_TAHOMA, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, TXT_CENTERXY, Label);

	m_LastWidget.x = x;
	m_LastWidget.y = y;
	m_LastWidget.width = w;
	m_LastWidget.height = h;

	if (callback)
		m_bReopened = true;

	return callback;
}

bool CMenu::ComboBox(CVar<int>& Var, const std::vector<CVar<int>>& List)
{
	auto FindCurItemName = [&]() -> const wchar_t*
	{
		for (const auto& Item : List)
		{
			if (Item.m_Var == Var.m_Var)
				return Item.m_szDisplayName;
		}

		return _(L"UNKNOWN_ITEM");
	};

	auto FindCurItemIndex = [&]() -> int
	{
		for (size_t n = 0; n < List.size(); n++)
		{
			if (List[n].m_Var == Var.m_Var)
				return n;
		}

		return 0;
	};

	bool callback = false;

	int x = m_LastWidget.x;
	int y = m_LastWidget.y + m_LastWidget.height + Vars::Menu::SpacingY;
	int w = Vars::Menu::ComboBoxW;
	int h = Vars::Menu::ComboBoxH;

	static std::map<CVar<int>*, int> indexes = {};
	static std::map<CVar<int>*, bool> inits = {};

	if (!inits[&Var] || m_bReopened) {
		indexes[&Var] = FindCurItemIndex();
		inits[&Var] = true;
	}

	if (g_InputHelper.m_nMouseX > x && g_InputHelper.m_nMouseX < x + (w / 2) && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
	{
		if (indexes[&Var] > 0)
		{
			if (g_InputHelper.IsPressed(VK_LBUTTON)) {
				Var.m_Var = List[--indexes[&Var]].m_Var;
				callback = true;
			}

			G::Draw.GradientRect(x, y, x + (w / 2), y + h, Vars::Menu::Colors::WidgetActive, Vars::Menu::Colors::Widget, true);
		}
	}

	else if (g_InputHelper.m_nMouseX > x + (w / 2) && g_InputHelper.m_nMouseX < x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
	{
		if (indexes[&Var] < int(List.size() - 1))
		{
			if (g_InputHelper.IsPressed(VK_LBUTTON)) {
				Var.m_Var = List[++indexes[&Var]].m_Var;
				callback = true;
			}

			G::Draw.GradientRect(x + (w / 2), y, x + w, y + h, Vars::Menu::Colors::Widget, Vars::Menu::Colors::WidgetActive, true);
		}
	}

	G::Draw.OutlinedRect(x, y, w, h, Vars::Menu::Colors::OutlineMenu);
	G::Draw.String(EFonts::MENU_TAHOMA, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, TXT_CENTERXY, FindCurItemName());
	G::Draw.String(EFonts::MENU_TAHOMA, x + w + Vars::Menu::SpacingText, y + (h / 2), Vars::Menu::Colors::Text, TXT_CENTERY, Var.m_szDisplayName);

	m_LastWidget.x = x;
	m_LastWidget.y = y;
	m_LastWidget.width = w;
	m_LastWidget.height = h;

	return callback;
}

bool CMenu::InputFloat(CVar<float>& Var, float Min, float Max, float Step, const wchar_t* Fmt)
{
	bool callback = false;

	int x = m_LastWidget.x;
	int y = m_LastWidget.y + m_LastWidget.height + Vars::Menu::SpacingY;
	int w = Vars::Menu::InputBoxW;
	int h = Vars::Menu::InputBoxH;

	if (Var.m_Var != Min)
	{
		if (g_InputHelper.m_nMouseX > x && g_InputHelper.m_nMouseX < x + (w / 2) && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
		{
			if (g_InputHelper.IsPressedAndHeld(VK_LBUTTON)) {
				Var.m_Var -= Step;
				Var.m_Var = std::clamp(Var.m_Var, Min, Max);
				callback = true;
			}

			G::Draw.GradientRect(x, y, x + (w / 2), y + h, Vars::Menu::Colors::WidgetActive, Vars::Menu::Colors::Widget, true);
		}
	}

	if (Var.m_Var != Max)
	{
		if (g_InputHelper.m_nMouseX > x + (w / 2) && g_InputHelper.m_nMouseX < x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
		{
			if (g_InputHelper.IsPressedAndHeld(VK_LBUTTON)) {
				Var.m_Var += Step;
				Var.m_Var = std::clamp(Var.m_Var, Min, Max);
				callback = true;
			}

			G::Draw.GradientRect(x + (w / 2), y, x + w, y + h, Vars::Menu::Colors::Widget, Vars::Menu::Colors::WidgetActive, true);
		}
	}

	G::Draw.OutlinedRect(x, y, w, h, Vars::Menu::Colors::OutlineMenu);
	G::Draw.String(EFonts::MENU_TAHOMA, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, TXT_CENTERXY, Fmt, Var.m_Var);
	G::Draw.String(EFonts::MENU_TAHOMA, x + w + Vars::Menu::SpacingText, y + (h / 2), Vars::Menu::Colors::Text, TXT_CENTERY, Var.m_szDisplayName);

	m_LastWidget.x = x;
	m_LastWidget.y = y;
	m_LastWidget.width = w;
	m_LastWidget.height = h;

	return callback;
}

bool CMenu::InputInt(CVar<int>& Var, int Min, int Max, int Step)
{
	bool callback = false;

	int x = m_LastWidget.x;
	int y = m_LastWidget.y + m_LastWidget.height + Vars::Menu::SpacingY;
	int w = Vars::Menu::InputBoxW;
	int h = Vars::Menu::InputBoxH;

	if (Var.m_Var != Min)
	{
		if (g_InputHelper.m_nMouseX > x && g_InputHelper.m_nMouseX < x + (w / 2) && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
		{
			if (g_InputHelper.IsPressedAndHeld(VK_LBUTTON)) {
				Var.m_Var -= Step;
				Var.m_Var = std::clamp(Var.m_Var, Min, Max);
				callback = true;
			}

			G::Draw.GradientRect(x, y, x + (w / 2), y + h, Vars::Menu::Colors::WidgetActive, Vars::Menu::Colors::Widget, true);
		}
	}

	if (Var.m_Var != Max)
	{
		if (g_InputHelper.m_nMouseX > x + (w / 2) && g_InputHelper.m_nMouseX < x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
		{
			if (g_InputHelper.IsPressedAndHeld(VK_LBUTTON)) {
				Var.m_Var += Step;
				Var.m_Var = std::clamp(Var.m_Var, Min, Max);
				callback = true;
			}

			G::Draw.GradientRect(x + (w / 2), y, x + w, y + h, Vars::Menu::Colors::Widget, Vars::Menu::Colors::WidgetActive, true);
		}
	}

	G::Draw.OutlinedRect(x, y, w, h, Vars::Menu::Colors::OutlineMenu);
	G::Draw.String(EFonts::MENU_TAHOMA, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, TXT_CENTERXY, "%d", Var.m_Var);
	G::Draw.String(EFonts::MENU_TAHOMA, x + w + Vars::Menu::SpacingText, y + (h / 2), Vars::Menu::Colors::Text, TXT_CENTERY, Var.m_szDisplayName);

	m_LastWidget.x = x;
	m_LastWidget.y = y;
	m_LastWidget.width = w;
	m_LastWidget.height = h;

	return callback;
}

bool CMenu::InputString(const wchar_t* Label, std::wstring& output)
{
	bool callback = false;

	int x = m_LastWidget.x;
	int y = m_LastWidget.y + m_LastWidget.height + Vars::Menu::SpacingY;
	int w = Vars::Menu::InputBoxW * 2;
	int h = Vars::Menu::ButtonH;

	static bool active = false;
	static std::wstring active_str = {};

	if (g_InputHelper.IsPressed(VK_LBUTTON))
	{
		if (g_InputHelper.m_nMouseX > x && g_InputHelper.m_nMouseX < x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h)
			active = !active;

		else active = false;
	}

	if (active)
	{
		m_bTyping = true;

		if (g_InputHelper.IsPressed(VK_INSERT)) {
			active = false;
			return false;
		}

		if (active_str.length() < 21)
		{
			if (g_InputHelper.IsPressed(VK_SPACE))
				active_str += char(VK_SPACE);

			for (int16_t key = L'A'; key < L'Z' + 1; key++)
			{
				if (g_InputHelper.IsPressed(key))
					active_str += wchar_t(key);
			}
		}

		if (g_InputHelper.IsPressedAndHeld(VK_BACK) && !active_str.empty())
			active_str.erase(active_str.end() - 1);

		if (g_InputHelper.IsPressed(VK_RETURN))
		{
			active = false;

			if (!active_str.empty())
			{
				output = active_str;
				callback = true;
			}
		}

		G::Draw.String(EFonts::MENU_TAHOMA, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, TXT_CENTERXY, "%ws", active_str.empty() ? _(L"Enter a Name") : active_str.c_str());
	}

	else
	{
		active_str = {};
		G::Draw.String(EFonts::MENU_TAHOMA, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, TXT_CENTERXY, "%ws", Label);
	}

	G::Draw.OutlinedRect(x, y, w, h, Vars::Menu::Colors::OutlineMenu);

	m_LastWidget.x = x;
	m_LastWidget.y = y;
	m_LastWidget.width = w;
	m_LastWidget.height = h;

	return callback;
}

bool CMenu::InputKey(CVar<int>& output, bool bAllowNone)
{
	auto VK2STR = [&](const short key) -> std::wstring
	{
		switch (key) {
		case VK_LBUTTON: return _(L"LButton");
		case VK_RBUTTON: return _(L"RButton");
		case VK_MBUTTON: return _(L"MButton");
		case VK_XBUTTON1: return _(L"XButton1");
		case VK_XBUTTON2: return _(L"XButton2");
		case 0x0: return _(L"None");
		default: break;
		}

		WCHAR output[16] = { L"\0" };

		if (const int result = GetKeyNameTextW(MapVirtualKeyW(key, MAPVK_VK_TO_VSC) << 16, output, 16))
			return output;

		return _(L"VK2STR_FAILED");
	};

	bool callback = false;

	int x = m_LastWidget.x;
	int y = m_LastWidget.y + m_LastWidget.height + Vars::Menu::SpacingY;
	int w = Vars::Menu::InputBoxW;
	int h = Vars::Menu::InputBoxH;

	static bool active = false, old_active = active;
	static float time = I::EngineClient->Time();
	float elapsed = I::EngineClient->Time() - time;

	if (old_active != active) {
		time = I::EngineClient->Time();
		old_active = active;
	}

	if (!active && elapsed > 0.1f && g_InputHelper.IsPressed(VK_LBUTTON))
	{
		if (g_InputHelper.m_nMouseX > x && g_InputHelper.m_nMouseX < x + w && g_InputHelper.m_nMouseY > y && g_InputHelper.m_nMouseY < y + h) {
			active = true;
			g_InputHelper.NullKey(VK_LBUTTON);
		}
	}

	static float time_notactive = 0.0f;

	if (active)
	{
		m_bTyping = true;

		if (I::EngineClient->Time() - time_notactive > 0.1f)
		{
			for (short n = 0; n < 256; n++)
			{
				if ((n > 0x0 && n < 0x7)
					|| (n > L'A' - 1 && n < L'Z' + 1)
					|| n == VK_LSHIFT || n == VK_RSHIFT || n == VK_SHIFT
					|| n == VK_ESCAPE || n == VK_INSERT)
				{
					if (g_InputHelper.IsPressed(n))
					{
						if (n == VK_INSERT) {
							active = false;
							break;
						}

						if (n == VK_ESCAPE && bAllowNone) {
							output.m_Var = 0x0;
							active = false;
							break;
						}

						output.m_Var = n;
						active = false;
						break;
					}
				}
			}
		}

		G::Draw.String(EFonts::MENU_TAHOMA, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, TXT_CENTERXY, "%ws", _(L"Press a Key"));
	}

	else
	{
		time_notactive = I::EngineClient->Time();
		G::Draw.String(EFonts::MENU_TAHOMA, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, TXT_CENTERXY, "%ws", VK2STR(output.m_Var).c_str());
	}

	G::Draw.String(EFonts::MENU_TAHOMA, x + w + Vars::Menu::SpacingText, y + (h / 2), Vars::Menu::Colors::Text, TXT_CENTERY, output.m_szDisplayName);
	G::Draw.OutlinedRect(x, y, w, h, Vars::Menu::Colors::OutlineMenu);

	m_LastWidget.x = x;
	m_LastWidget.y = y;
	m_LastWidget.width = w;
	m_LastWidget.height = h;

	return callback;
}

void CMenu::GroupBoxStart()
{
	m_LastGroupBox.x = m_LastWidget.x;
	m_LastGroupBox.y = m_LastWidget.y + m_LastWidget.height + Vars::Menu::SpacingY;

	m_LastWidget.x += Vars::Menu::SpacingX;
	m_LastWidget.y += Vars::Menu::SpacingY * 2;
}

void CMenu::GroupBoxEnd(const wchar_t* Label, int Width)
{
	int h = m_LastWidget.y - m_LastGroupBox.y + m_LastWidget.height + Vars::Menu::SpacingY;

	int label_w, label_h;
	I::VGuiSurface->GetTextSize(G::Draw.m_Fonts.Find(EFonts::MENU_TAHOMA), Label, label_w, label_h);

	int label_x = m_LastGroupBox.x + Vars::Menu::SpacingText;
	int label_y = m_LastGroupBox.y - (label_h / 2);

	G::Draw.Line(m_LastGroupBox.x, m_LastGroupBox.y, label_x, m_LastGroupBox.y, Vars::Menu::Colors::OutlineMenu);
	G::Draw.Line(label_x + label_w, m_LastGroupBox.y, m_LastGroupBox.x + Width, m_LastGroupBox.y, Vars::Menu::Colors::OutlineMenu);
	G::Draw.Line(m_LastGroupBox.x + Width, m_LastGroupBox.y, m_LastGroupBox.x + Width, m_LastGroupBox.y + h, Vars::Menu::Colors::OutlineMenu);
	G::Draw.Line(m_LastGroupBox.x + Width, m_LastGroupBox.y + h, m_LastGroupBox.x, m_LastGroupBox.y + h, Vars::Menu::Colors::OutlineMenu);
	G::Draw.Line(m_LastGroupBox.x, m_LastGroupBox.y + h, m_LastGroupBox.x, m_LastGroupBox.y, Vars::Menu::Colors::OutlineMenu);

	G::Draw.String(EFonts::MENU_TAHOMA, label_x, label_y, Vars::Menu::Colors::Text, TXT_CENTERXY, Label);

	m_LastWidget.x -= Vars::Menu::SpacingX;
	m_LastWidget.y += Vars::Menu::SpacingY * 2;
	m_LastGroupBox.height = h;
}

void CMenu::Run()
{
	m_bReopened = false;
	m_bTyping = false;

	static bool bOldOpen = m_bOpen;

	if (bOldOpen != m_bOpen)
	{
		bOldOpen = m_bOpen;

		if (m_bOpen)
			m_bReopened = true;
	}

	if (I::EngineClient->IsDrawingLoadingImage()) {
		m_bOpen = false;
		return;
	}

	static float flTimeOnChange = 0.0f;

	if (GetAsyncKeyState(VK_INSERT) & 1) {
		I::VGuiSurface->UnlockCursor();
		I::VGuiSurface->SetCursorAlwaysVisible(m_bOpen = !m_bOpen);
		flTimeOnChange = I::EngineClient->Time();
	}

	m_flFadeElapsed = I::EngineClient->Time() - flTimeOnChange;

	if (m_flFadeElapsed < m_flFadeDuration) {
		m_flFadeAlpha = RemapValClamped(m_flFadeElapsed, 0.0f, m_flFadeDuration, !m_bOpen ? 1.0f : 0.0f, m_bOpen ? 1.0f : 0.0f);
		I::VGuiSurface->DrawSetAlphaMultiplier(m_flFadeAlpha);
	}

	if (m_bOpen || m_flFadeElapsed < m_flFadeDuration)
	{
		m_szCurTip = L"";
		g_InputHelper.Update();

		//Do the Watermark

		//Do the Window
		{
			g_InputHelper.Drag(
				g_InputHelper.m_nMouseX,
				g_InputHelper.m_nMouseY,
				Vars::Menu::Position.x,
				Vars::Menu::Position.y,
				Vars::Menu::Position.width,
				Vars::Menu::TitleBarH,
				Vars::Menu::TitleBarH);

			G::Draw.Rect(
				Vars::Menu::Position.x,
				Vars::Menu::Position.y,
				Vars::Menu::Position.width,
				Vars::Menu::Position.height,
				Vars::Menu::Colors::WindowBackground);

			G::Draw.Rect(
				Vars::Menu::Position.x,
				Vars::Menu::Position.y - Vars::Menu::TitleBarH,
				Vars::Menu::Position.width,
				Vars::Menu::TitleBarH,
				Vars::Menu::Colors::TitleBar);

			G::Draw.String(EFonts::MENU_TAHOMA,
				Vars::Menu::Position.x + (Vars::Menu::Position.width / 2),
				Vars::Menu::Position.y - (Vars::Menu::TitleBarH / 2),
				Vars::Menu::Colors::Text,
				TXT_CENTERXY,
				"%ls", _(L"Team Fortress 2"));
		}

		//Do the Widgets
		{
			enum struct EMainTabs { TAB_AIM, TAB_VISUALS, TAB_MISC, TAB_CONFIGS };
			enum struct EAimTabs { TAB_AIMBOT, TAB_TRIGGERBOT, TAB_OTHER };
			enum struct EVisualsTabs { TAB_ESP, TAB_RADAR, TAB_CHAMS, TAB_GLOW, TAB_OTHER, TAB_SKINS, TAB_COLORS };
			enum struct EMiscTabs { TAB_HVH, TAB_MAIN };

			m_LastWidget = { Vars::Menu::Position.x + Vars::Menu::SpacingX, Vars::Menu::Position.y, 0, 0 };

			static EMainTabs Tab = EMainTabs::TAB_AIM;
			{
				if (Button(_(L"Aim"), Tab == EMainTabs::TAB_AIM))
					Tab = EMainTabs::TAB_AIM;

				if (Button(_(L"Visuals"), Tab == EMainTabs::TAB_VISUALS))
					Tab = EMainTabs::TAB_VISUALS;

				if (Button(_(L"Misc"), Tab == EMainTabs::TAB_MISC))
					Tab = EMainTabs::TAB_MISC;

				m_LastWidget = {
					Vars::Menu::Position.x + Vars::Menu::SpacingX,
					Vars::Menu::Position.y + Vars::Menu::Position.height - (Vars::Menu::ButtonH + (Vars::Menu::SpacingY * 2)),
					0, 0 };

				if (Button(_(L"Configs"), Tab == EMainTabs::TAB_CONFIGS))
					Tab = EMainTabs::TAB_CONFIGS;
			}

			Separator();

			switch (Tab)
			{
			case EMainTabs::TAB_AIM:
			{
				static EAimTabs Tab = EAimTabs::TAB_AIMBOT;
				{
					Rect_t checkpoint_line = m_LastWidget;
					checkpoint_line.x -= Vars::Menu::SpacingX;
					checkpoint_line.y += Vars::Menu::ButtonHSmall + (Vars::Menu::SpacingY * 2);
					Rect_t checkpoint_move = m_LastWidget;

					if (Button(_(L"Aimbot"), Tab == EAimTabs::TAB_AIMBOT, Vars::Menu::ButtonWSmall, Vars::Menu::ButtonHSmall))
						Tab = EAimTabs::TAB_AIMBOT;

					checkpoint_move.x += Vars::Menu::ButtonWSmall + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint_move;

					if (Button(_(L"Triggerbot"), Tab == EAimTabs::TAB_TRIGGERBOT, Vars::Menu::ButtonWSmall, Vars::Menu::ButtonHSmall))
						Tab = EAimTabs::TAB_TRIGGERBOT;

					checkpoint_move.x += Vars::Menu::ButtonWSmall + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint_move;

					if (Button(_(L"Other"), Tab == EAimTabs::TAB_OTHER, Vars::Menu::ButtonWSmall, Vars::Menu::ButtonHSmall))
						Tab = EAimTabs::TAB_OTHER;

					m_LastWidget = checkpoint_line;
					G::Draw.Line(checkpoint_line.x, checkpoint_line.y, Vars::Menu::Position.x + Vars::Menu::Position.width - 1, checkpoint_line.y, Vars::Menu::Colors::OutlineMenu);
					checkpoint_line.x += Vars::Menu::SpacingX;
					checkpoint_line.y += Vars::Menu::SpacingY;
					m_LastWidget = checkpoint_line;
				}


				break;
			}

			case EMainTabs::TAB_VISUALS:
			{
				static EVisualsTabs Tab = EVisualsTabs::TAB_ESP;
				{
					Rect_t checkpoint_line = m_LastWidget;
					checkpoint_line.x -= Vars::Menu::SpacingX;
					checkpoint_line.y += Vars::Menu::ButtonHSmall + (Vars::Menu::SpacingY * 2);
					Rect_t checkpoint_move = m_LastWidget;

					if (Button(_(L"ESP"), Tab == EVisualsTabs::TAB_ESP, Vars::Menu::ButtonWSmall, Vars::Menu::ButtonHSmall))
						Tab = EVisualsTabs::TAB_ESP;

					checkpoint_move.x += Vars::Menu::ButtonWSmall + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint_move;

					if (Button(_(L"Radar"), Tab == EVisualsTabs::TAB_RADAR, Vars::Menu::ButtonWSmall, Vars::Menu::ButtonHSmall))
						Tab = EVisualsTabs::TAB_RADAR;

					checkpoint_move.x += Vars::Menu::ButtonWSmall + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint_move;

					if (Button(_(L"Chams"), Tab == EVisualsTabs::TAB_CHAMS, Vars::Menu::ButtonWSmall, Vars::Menu::ButtonHSmall))
						Tab = EVisualsTabs::TAB_CHAMS;

					checkpoint_move.x += Vars::Menu::ButtonWSmall + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint_move;

					if (Button(_(L"Glow"), Tab == EVisualsTabs::TAB_GLOW, Vars::Menu::ButtonWSmall, Vars::Menu::ButtonHSmall))
						Tab = EVisualsTabs::TAB_GLOW;

					checkpoint_move.x += Vars::Menu::ButtonWSmall + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint_move;

					if (Button(_(L"Other"), Tab == EVisualsTabs::TAB_OTHER, Vars::Menu::ButtonWSmall, Vars::Menu::ButtonHSmall))
						Tab = EVisualsTabs::TAB_OTHER;

					checkpoint_move.x += Vars::Menu::ButtonWSmall + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint_move;

					if (Button(_(L"Colors"), Tab == EVisualsTabs::TAB_COLORS, Vars::Menu::ButtonWSmall, Vars::Menu::ButtonHSmall))
						Tab = EVisualsTabs::TAB_COLORS;

					m_LastWidget = checkpoint_line;
					G::Draw.Line(checkpoint_line.x, checkpoint_line.y, Vars::Menu::Position.x + Vars::Menu::Position.width - 1, checkpoint_line.y, Vars::Menu::Colors::OutlineMenu);
					checkpoint_line.x += Vars::Menu::SpacingX;
					checkpoint_line.y += Vars::Menu::SpacingY;
					m_LastWidget = checkpoint_line;
				}

				switch (Tab)
				{
				case EVisualsTabs::TAB_ESP:
				{
					Rect_t checkpoint = m_LastWidget;


					break;
				}

				case EVisualsTabs::TAB_OTHER:
				{
					Rect_t checkpoint = m_LastWidget;

					GroupBoxStart();
					{
						CheckBox(Vars::Visual::RemoveScope, _(L"Remove sniper's scope overlay"));
						CheckBox(Vars::Visual::RemoveVisualRecoil, _(L"Remove visual punch/recoil"));
					}
					GroupBoxEnd(_(L"Local"), 190);

					checkpoint.x += 190 + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint;

					GroupBoxStart();
					{
						CheckBox(Vars::Menu::ToolTips, _(L"This is an example tooltip"));
					}
					GroupBoxEnd(_(L"Other"), 200);

					break;
				}
				}

				break;
			}

			case EMainTabs::TAB_MISC:
			{
				static EMiscTabs Tab = EMiscTabs::TAB_MAIN;
				{
					Rect_t checkpoint_line = m_LastWidget;
					checkpoint_line.x -= Vars::Menu::SpacingX;
					checkpoint_line.y += Vars::Menu::ButtonHSmall + (Vars::Menu::SpacingY * 2);
					Rect_t checkpoint_move = m_LastWidget;

					if (Button(_(L"Main"), Tab == EMiscTabs::TAB_MAIN, Vars::Menu::ButtonWSmall, Vars::Menu::ButtonHSmall))
						Tab = EMiscTabs::TAB_MAIN;

					checkpoint_move.x += Vars::Menu::ButtonWSmall + Vars::Menu::SpacingX;
					m_LastWidget = checkpoint_move;

					if (Button(_(L"HvH"), Tab == EMiscTabs::TAB_HVH, Vars::Menu::ButtonWSmall, Vars::Menu::ButtonHSmall))
						Tab = EMiscTabs::TAB_HVH;

					m_LastWidget = checkpoint_line;
					G::Draw.Line(checkpoint_line.x, checkpoint_line.y, Vars::Menu::Position.x + Vars::Menu::Position.width - 1, checkpoint_line.y, Vars::Menu::Colors::OutlineMenu);
					checkpoint_line.x += Vars::Menu::SpacingX;
					checkpoint_line.y += Vars::Menu::SpacingY;
					m_LastWidget = checkpoint_line;
				}

				switch (Tab)
				{
				case EMiscTabs::TAB_MAIN:
				{
					GroupBoxStart();
					{
						CheckBox(Vars::Misc::Bunnyhop, _(L"Automatically bunnyhop"));
						CheckBox(Vars::Misc::AutoStrafe, _(L"Automatically strafe"));
						CheckBox(Vars::Misc::BypassPure, _(L"Bypass sv_pure"));
					}
					GroupBoxEnd(_(L"Main"), 210);

					break;
				}
				}

				break;
			}

			default: break;
			}
		}

		DrawTooltip();
	}

	I::VGuiSurface->DrawSetAlphaMultiplier(1.0f);
}