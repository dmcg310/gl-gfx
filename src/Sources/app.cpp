#include "app.h"

#include "scene_system.h"

namespace App {
	void Run() {
		Backend::Init();
		Renderer::Init();
		SceneSystem::Init();

		auto *cube1 = SceneSystem::CreateEntity("cube1");
		cube1->mesh = Renderer::GetDefaultCubeMesh();
		cube1->material = MaterialSystem::CreateDefaultMaterial("cube1_material");
		cube1->texture = Renderer::GetDefaultTexture();
		TransformSystem::SetPosition(cube1->transform, glm::vec3(0.0f, 0.0f, -3.0f));

		auto *cube2 = SceneSystem::CreateEntity("cube2");
		cube2->mesh = Renderer::GetDefaultCubeMesh();
		cube2->material = MaterialSystem::CreateDefaultMaterial("cube2_material");
		cube2->texture = Renderer::GetDefaultTexture();
		TransformSystem::SetPosition(cube2->transform, glm::vec3(2.0f, 1.0f, -5.0f));

		auto *cube3 = SceneSystem::CreateEntity("cube3");
		cube3->mesh = Renderer::GetDefaultCubeMesh();
		cube3->material = MaterialSystem::CreateDefaultMaterial("cube3_material");
		cube3->texture = Renderer::GetDefaultTexture();
		TransformSystem::SetPosition(cube3->transform, glm::vec3(4.0f, 0.0f, -3.0f));

		while (Backend::WindowIsOpen()) {
			Backend::BeginFrame();

			Backend::Update();
			SceneSystem::Update();

			Renderer::Render();

			Backend::EndFrame();
		}

		SceneSystem::CleanUp();
		Renderer::CleanUp();
		Backend::CleanUp();
	}
}
