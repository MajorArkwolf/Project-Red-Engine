#pragma once

#include "Engine/Physics/CollisionDetection.hpp"
#include "Engine/Physics/CollisionResolution.hpp"


namespace physics {

    class PhysicsEngine {
    public:
        /**
         * Default constructor.
         */
        PhysicsEngine() = default;

        /**
         * Default destructor.
         */
        ~PhysicsEngine() = default;

        /**
         * Deleted copy constructor.
         * @param p
         */
        PhysicsEngine(PhysicsEngine &p) = delete;

        /**
         * Deleted move constructor
         * @param p
         */
        PhysicsEngine(PhysicsEngine &&p) = delete;

        /**
         * Deleted copy assignment.
         * @param p
         * @return
         */
        PhysicsEngine &operator=(PhysicsEngine p) = delete;

        /**
         * Deleted move assignment.
         * @param p
         * @return
         */
        PhysicsEngine &operator=(PhysicsEngine &&p) = delete;

        /**
         * Fixed Update, commonly used to update physics based components.
         * @param t time since engine started.
         * @param dt time since last frame aka delta time.
         */
        void FixedUpdate(double t, double dt);

        /**
         * This is used for non physics related tasks such as setting up the display renderer for the draw call.
         * @param t time since engine started.
         * @param dt time since last frame aka delta time.
         */
        void Update(double t, double dt);


        /**
         * Draws the physics world over the real world.
         * @param projection projection matrix of the camera.
         * @param view the view matrix of the camera.
         */
        void Draw(const glm::mat4 &projection, const glm::mat4 &view);


        /**
         * Toggles the debug renderer on or off.
         */
        void ToggleRenderer();

        /**
         * Returns if the renderer is on.
         * @return true if renderer is on.
         */
        bool GetRendererStatus();

        unsigned int AddCollider(const entt::entity &entity_id, PhysicsShape &shape, glm::vec3 relative_position,
                        glm::quat rotation);

        PhysicsShape CreateBoxShape(glm::vec3 extents);

        PhysicsShape CreateCapsuleShape(double radius, double height);

        PhysicsShape CreateSphereShape(double radius);

        reactphysics3d::PhysicsWorld *CreatePhysicsWorld();

        void DestroyPhysicsWorld(reactphysics3d::PhysicsWorld *);

        void Init();

    private:
        CollisionDetection collision_detection_ = {};
        CollisionResolution collision_resolution_ = {};
    };
}
