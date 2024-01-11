#include "pch.hpp"

#include "application.hpp"

aito::Application::Application()
{
	// Setup global ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
}

aito::Application::~Application()
{
	// Tear down ImGui context
	ImGui_ImplGlfw_InitForVulkan(window_.get_glfw_window(), true);
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
