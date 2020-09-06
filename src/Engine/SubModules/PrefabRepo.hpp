#pragma once

#include <glm/vec3.hpp>
#include <string>
#include <vector>
#include <glm/gtc/quaternion.hpp>
#include <unordered_map>
#include <Engine/Renderer/Shader.hpp>
#include <Engine/Physics/CollisionShapes.hpp>

namespace redengine{
    struct prefab {
        std::string name;
        bool has_model = false;
        size_t model_id = 0;
        std::shared_ptr<Shader> model_shader;
        glm::vec3 position_local = {0.0f, 0.0f, 0.0f};
        glm::quat rotation_local = {1.0f, 0.0f, 0.0f, 0.0f};
        glm::vec3 scale_local = {1.0f, 1.0f, 1.0f};
        bool has_animation = false;
        bool has_physics = false;
        bool is_static = false;
        std::vector<CollisionShape> collision_shapes = {};
    };

    class PrefabRepo {
    public:
        PrefabRepo();
        ~PrefabRepo() = default;
        PrefabRepo(PrefabRepo& prefabRepo) = delete;
        PrefabRepo(PrefabRepo&& prefabRepo) = default;
        PrefabRepo& operator=(PrefabRepo& prefabRepo) = delete;
        PrefabRepo& operator=(PrefabRepo&& prefabRepo) = default;
        void Init();
        prefab& AddNewPrefab(const std::string& key);
        bool FindPrefab(const std::string& key);
        const prefab &GetPrefab(const std::string &key) const;
    private:
        std::unordered_map<std::string,prefab> prefabMap_ = {};
    };
}

