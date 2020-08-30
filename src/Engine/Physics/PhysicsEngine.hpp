#pragma once
#include "Engine/Physics/CollisionDetection.hpp"
#include "Engine/Physics/CollisionResolution.hpp"

class ECS;

class PhysicsEngine {
public:
    PhysicsEngine() = default;
    ~PhysicsEngine() = default;
    PhysicsEngine(PhysicsEngine& p) = delete;
    PhysicsEngine(PhysicsEngine&& p) = delete;
    PhysicsEngine& operator= (PhysicsEngine p) = delete;
    PhysicsEngine& operator= (PhysicsEngine&& p) = delete;

    void FixedUpdate(double t, double dt);
    void Update(double t, double dt);
    void Draw(const glm::mat4& projection, const glm::mat4& view, const glm::dvec2& cameraPos);
    void ToggleRenderer();
    bool GetRendererStatus();
    void SetECS(ECS * ecs);

private:
    ECS* ecs_ = nullptr;
    CollisionDetection collision_detection_ = {};
    CollisionResolution collision_resolution_ = {};
};

