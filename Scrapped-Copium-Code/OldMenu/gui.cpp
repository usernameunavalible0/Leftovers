#include "gui.h"
#include "../Vars.h"


constexpr int MENU_KEY = VK_INSERT;

int unuPrimary = 0;
int unuSecondary = 0;

/* The main Menu */
void COldMenu::DrawMenu()
{
	ImGui::SetNextWindowSize(ImVec2(700, 700), ImGuiCond_FirstUseEver);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 700, 500 });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);

	if (ImGui::Begin("Copium", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar))
	{
		const auto drawList = ImGui::GetWindowDrawList();
		const auto windowSize = ImGui::GetWindowSize();
		const auto windowPos = ImGui::GetWindowPos();

		// Prepare Gradiant Colors
		{
			// nothing to see here ;)		
		}

		//Gradiant Line
		
		ImGui::Dummy({ 0, 2 });

		//Title Tex
		{
			ImGui::PushFont(TitleFont);
			const auto titleWidth = ImGui::CalcTextSize("Copium").x;
			drawList->AddText(TitleFont, TitleFont->FontSize, { windowPos.x + (windowPos.x / 2) - (titleWidth / 2), windowPos.y }, Accent, "Copium");
			ImGui::PopFont();
		}

		//Icons

	}
}

void COldMenu::Render(IDirect3DDevice9* pDevice)
{
	static bool initialized = false;
	if (!ConfigLoaded) { return; }

	if (!initialized)
	{
		Init(pDevice);
		initialized = true;
	}

	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

	// Toggle menu
	if (GetAsyncKeyState(MENU_KEY) & 1)
	{
		//F::Menu.IsOpen = !F::Menu.IsOpen;
		//I::EngineTool->ShowCursor(F::Menu.IsOpen);
	}

	// Begin current frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::PushFont(Verdana);

	// Window that should always be visible

	if (IsOpen)
	{
		DrawMenu();
	}

	// End frame and render
	ImGui::PopFont();
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
}

void COldMenu::Init(IDirect3DDevice9* pDevice)
{
	// Initialize ImGui and device
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(FindWindowA(nullptr, "Team Fortress 2"));
	ImGui_ImplDX9_Init(pDevice);

	// Fonts
	{
		const auto& io = ImGui::GetIO();

		auto fontConfig = ImFontConfig();
		fontConfig.OversampleH = 2;

		constexpr ImWchar fontRange[]{ 0x0020, 0x00FF,0x0400, 0x044F, 0 }; // Basic Latin, Latin Supplement and Cyrillic

		VerdanaSmall = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\verdana.ttf", 12.0f, &fontConfig, fontRange);
		Verdana = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\verdana.ttf", 14.0f, &fontConfig, fontRange);
		VerdanaBold = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\verdanab.ttf", 18.0f, &fontConfig, fontRange);

		SectionFont = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\verdana.ttf", 16.0f, &fontConfig, fontRange);
		TitleFont = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\verdanab.ttf", 20.0f, &fontConfig, fontRange);


		io.Fonts->Build();
	}


}