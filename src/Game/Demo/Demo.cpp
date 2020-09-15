#include "Demo.hpp"

#include "ECS/Component/Basic.hpp"
#include "ECS/Component/Model.hpp"
#include "ECS/Component/Player.hpp"
#include "Engine/Engine.hpp"
#include "Engine/SubModules/JsonLoader.hpp"

static inline void ToggleRenderer(physics::PhysicsWorld &pe, bool val) {
    if (pe.GetRendererStatus() != val) {
        pe.ToggleRenderer();
    }
}

template<class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};
template<class... Ts>
overload(Ts...)->overload<Ts...>;

Demo::Demo() {
    physics_world_.SetECS(&ecs_);
    camera = engine::Camera();
    camera.position_ = glm::vec3(0.0f, 10.0f, 0.0f);
    relativeMouse = true;
    auto &engine = redengine::Engine::get();
    engine.GetMouseMode(relativeMouse);
    std::filesystem::path path = "";
    path.append("Demo");
    path.append("Scene.json");
    JSONLoader::LoadScene(path, &ecs_, &physics_world_);

    /// This is messy, this instantiates the two main players for our scene.
    auto big_player = ecs_.CreateEntity();
    big_player.AddComponent<component::Player>();
    auto filepath = redengine::Engine::get().GetBasePath() / "res" / "model" / "ClothedMan.gltf";
    auto &b_model = big_player.AddComponent<component::Model>(filepath);
    b_model.draw_model = false;
    auto &trans = big_player.AddComponent<component::Transform>();
    trans.pos = glm::vec3{0.f, 0.f, 0.f};
    auto &playerComp = big_player.GetComponent<component::Player>();
    big_player.AddComponent<component::PhysicBody>();
    auto &phys = big_player.GetComponent<component::PhysicBody>();
    physics_world_.AddCollisionBody(big_player.GetID(), playerComp.camera_.position_, glm::quat(glm::vec3(0, 0, 0)));
    auto &physics_engine = redengine::Engine::get().GetPhysicsEngine();
    auto playerShape = physics_engine.CreateCapsuleShape(50, 100);
    physics_world_.AddCollider(big_player.GetID(), playerShape, {0.f, 0.f, 0.f}, {1.0f, 0.f, 0.f, 0.f});
    trans.scale = glm::vec3 {24.0f, 24.0f, 24.0f};
    big_player.GetComponent<component::Player>().camera_.movement_speed_ = 0.15f;
    big_player.GetComponent<component::Player>().camera_.position_ = glm::vec3{0.f, 0.f, 0.f};;
    auto &anim =
            big_player.AddComponent<component::Animation>(
                    big_player.GetComponent<component::Model>().id_);
    auto idle = "IDLE";
    anim.animator_.LoadAnimation(idle);
    player_.SetBigPlayer(big_player);

    auto little_player = ecs_.CreateEntity();
    auto &little_p = little_player.AddComponent<component::Player>();
    auto &little_model = little_player.AddComponent<component::Model>(filepath);
    auto &little_tran = little_player.AddComponent<component::Transform>();
    little_tran.pos = glm::vec3{-465.0f, 80.0f, 330.0f};
    little_tran.scale = glm::vec3 {0.5, 0.5, 0.5};
    little_p.camera_.position_ = glm::vec3{-465.0f, 82.8f, 330.0f};
    little_p.height_ = 83.0f;
    auto &little_anim =
            little_player.AddComponent<component::Animation>(
                    little_player.GetComponent<component::Model>().id_);
    little_anim.animator_.LoadAnimation(idle);
    player_.SetLittlePlayer(little_player);
    /// Player instantiation completed

}

void Demo::Init() {
}

void Demo::UnInit() {
    auto &renderer = redengine::Engine::get().renderer_;
    renderer.ClearCamera();
}

void Demo::Display(Shader *shader, const glm::mat4 &projection, const glm::mat4 &view) {
    auto &renderer = redengine::Engine::get().renderer_;
    auto &engine = redengine::Engine::get();
    auto &gui_manager = engine.GetGuiManager();
    ToggleRenderer(physics_world_, gui_manager.renderer_);
    renderer.SetCameraOnRender(player_.GetActiveCamera());
    ecs_.Draw(shader, projection, view);
}

void Demo::GUIStart() {
    auto &engine = redengine::Engine::get();
    GUIManager::startWindowFrame();
    engine.GetGuiManager().DisplayEscapeMenu();
    engine.GetGuiManager().DisplayConsoleLog();
    engine.GetGuiManager().DisplayDevScreen(player_.GetActiveCamera());
    engine.GetGuiManager().DisplayInputRebindWindow();
    engine.GetGuiManager().DisplayQuitScreen();

}

void Demo::GUIEnd() {
    GUIManager::EndWindowFrame();
}

void Demo::Update(double t, double dt) {
    auto &renderer = redengine::Engine::get().renderer_;
    renderer.SetCameraOnRender(player_.GetActiveCamera());
    ecs_.Update(t, dt);
    player_.ProcessKeyboardInput(forward_, backward_, left_, right_, dt);
    player_.Update(t, dt);
}

void Demo::FixedUpdate(double t, double dt) {
    ecs_.FixedUpdate(t, dt);
}

void Demo::HandleInputData(input::InputEvent inputData, double deltaTime) {
    using namespace input;
    auto &engine = redengine::Engine::get();
    auto &gui_manager = engine.GetGuiManager();
    auto handledMouse = false;
    std::visit(overload{
                   [&](std::monostate) {

                   },
                   [&](InputEvent::MouseEvent mouse) {

                   },
                   [&](InputEvent::KeyboardEvent keyboard) {
                       switch (inputData.type) {
                           case input::InputType::kKeyPressed: {
                               switch (keyboard.key) {
                                   case input::VirtualKey::W: {
                                       forward_ = true;
                                   } break;
                                   case input::VirtualKey::A: {
                                       left_ = true;
                                   } break;
                                   case input::VirtualKey::S: {
                                       backward_ = true;
                                   } break;
                                   case input::VirtualKey::D: {
                                       right_ = true;
                                   } break;
                                   case input::VirtualKey::X: {
                                       gui_manager.ToggleWindow("quitScreen");
                                   } break;
                               }
                           } break;
                           case input::InputType::kKeyReleased: {
                               switch (keyboard.key) {
                                   case input::VirtualKey::W: {
                                       forward_ = false;
                                   } break;
                                   case input::VirtualKey::A: {
                                       left_ = false;
                                   } break;
                                   case input::VirtualKey::S: {
                                       backward_ = false;
                                   } break;
                                   case input::VirtualKey::D: {
                                       right_ = false;
                                   } break;
                                   case input::VirtualKey::E: {
                                       player_.TogglePlayer();
                                   } break;
                                   case input::VirtualKey::kEscape:
                                       gui_manager.ToggleWindow("escapeMenu");
                               }
                               break;
                               default:
                                   break;
                           } break;
                       }
                   },
                   [&](InputEvent::dVector2 vec) {

                   },
                   [&](InputEvent::iVector2 vec) {
                       switch (inputData.type) {
                           case input::InputType::kCursorMoved: {
                               static int prev_x = 0;
                               static int prev_y = 0;

                               auto x = static_cast<double>(vec.x);
                               auto y = static_cast<double>(vec.y);
                               x = x * -1.0;
                               player_.GetActiveCamera().ProcessMouseMovement(prev_x - x, prev_y - y);
                               handledMouse = true;
                               prev_x = x;
                               prev_y = y;
                           } break;
                           case input::InputType::kMouseScrolled: {
                               double amountScrolledY = static_cast<double>(vec.y);
                               player_.GetActiveCamera().ProcessMouseScroll(amountScrolledY);
                           }
                           default:
                               break;
                       }
                   }},
               inputData.data);
    if (!handledMouse) {
        engine.mouse_ = {0.0f, 0.0f};
    }
}
