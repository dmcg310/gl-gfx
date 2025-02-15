#include "app.h"

void App::Run() {
	Backend::Init();
	Renderer::Init();

	while (Backend::WindowIsOpen()) {
		Backend::BeginFrame();
		Backend::Update();

		Renderer::Render();

		Backend::EndFrame();
	}

	Renderer::CleanUp();
	Backend::CleanUp();
}
