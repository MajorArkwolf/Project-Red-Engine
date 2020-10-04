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
    player = ecs_.CreateEntity();
    player.AddComponent<component::Player>();
    auto playerComp = player.GetComponent<component::Player>();

    player.AddComponent<component::PhysicBody>();
    auto &phys = player.GetComponent<component::PhysicBody>();
    physics_world_.AddCollisionBody(player.GetID(), playerComp.camera.position_, glm::quat(glm::vec3(0, 0, 0)));
    auto &physics_engine = redengine::Engine::get().GetPhysicsEngine();
    auto playerShape = physics_engine.CreateCapsuleShape(50, 100);
    physics_world_.AddCollider(player.GetID(), playerShape, {0.f, 0.f, 0.f}, {1.0f, 0.f, 0.f, 0.f});

    player.GetComponent<component::Player>().camera.movement_speed_ = 0.15f;
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
    renderer.SetCameraOnRender(player.GetComponent<component::Player>().camera);
    ecs_.Draw(shader, projection, view);
}

void Demo::GUIStart() {
    auto &engine = redengine::Engine::get();
    GUIManager::startWindowFrame();
    engine.GetGuiManager().DisplayEscapeMenu();
    engine.GetGuiManager().DisplayConsoleLog();
    engine.GetGuiManager().DisplayDevScreen(player.GetComponent<component::Player>().camera);
    engine.GetGuiManager().DisplayInputRebindWindow();
    engine.GetGuiManager().DisplayQuitScreen();

}

void Demo::GUIEnd() {
    GUIManager::EndWindowFrame();
}

void Demo::Update(double t, double dt) {
    auto &renderer = redengine::Engine::get().renderer_;
    renderer.SetCameraOnRender(player.GetComponent<component::Player>().camera);
    ecs_.Update(t, dt);

    //TODO: fix this to use just the phyiscs world instead.
    auto &physics_engine = redengine::Engine::get().GetPhysicsEngine();
    physics_engine.Update(t, dt);
}

void Demo::FixedUpdate(double t, double dt) {
    camera.ProcessKeyboardInput(forward_, backward_, left_, right_, dt);
    player.GetComponent<component::Player>().camera.ProcessKeyboardInput(forward_, backward_, left_, right_, dt);
    player.GetComponent<component::Player>().Update(t, dt);
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
                               player.GetComponent<component::Player>().camera.ProcessMouseMovement(prev_x - x, prev_y - y);
                               handledMouse = true;
                               prev_x = x;
                               prev_y = y;
                           } break;
                           default:
                               break;
                       }
                   }},
               inputData.data);
    if (!handledMouse) {
        engine.mouse_ = {0.0f, 0.0f};
    }
}
