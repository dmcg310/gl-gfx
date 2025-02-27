#include "scene_system.h"

#include "renderer.h"

namespace SceneSystem {
    static std::unordered_map<std::string, Entity> m_entities;
    static std::vector<Entity *> m_entityPtrs;

    void Init() {
        m_entities.clear();
        m_entityPtrs.clear();
    }

    Entity *CreateEntity(const std::string &name, const glm::vec4 &color) {
        Entity entity;
        entity.name = name;
        entity.transform = TransformSystem::CreateTransform(name);
        entity.color = color;

        m_entities[name] = entity;
        m_entityPtrs.push_back(&m_entities[name]);

        return &m_entities[name];
    }

    Entity *GetEntity(const std::string &name) {
        const auto it = m_entities.find(name);
        return (it != m_entities.end()) ? &it->second : nullptr;
    }

    const std::vector<Entity *> &GetAllEntities() {
        return m_entityPtrs;
    }

    void DestroyEntity(const std::string &name) {
        const auto it = m_entities.find(name);
        if (it != m_entities.end()) {
            if (const auto vecIt =
                    std::ranges::find(m_entityPtrs, &it->second); vecIt != m_entityPtrs.end()) {
                m_entityPtrs.erase(vecIt);
            }

            m_entities.erase(it);
        }
    }

    void Update() {
        for (const auto *entity: m_entityPtrs) {
            if (entity->isActive) {
                Renderer::SubmitInstanced(
                    entity->mesh,
                    entity->material,
                    entity->texture,
                    TransformSystem::GetModelMatrix(entity->transform),
                    entity->color
                );
            }
        }
    }

    void CleanUp() {
        m_entities.clear();
        m_entityPtrs.clear();
    }
}
