#include "PrefabGUI.hpp"
#include "Engine/Engine.hpp"
#include "ECS/Component/Basic.hpp"
#include "ECS/Component/Model.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <nlohmann/json.hpp>


namespace ImGui
{
    static auto vector_getter = [](void* vec, int idx, const char** out_text)
    {
        auto& vector = *static_cast<std::vector<std::string>*>(vec);
        if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
        *out_text = vector.at(idx).c_str();
        return true;
    };

    bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
    {
        if (values.empty()) { return false; }
        return Combo(label, currIndex, vector_getter,
                     static_cast<void*>(&values), values.size());
    }

    bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
    {
        if (values.empty()) { return false; }
        return ListBox(label, currIndex, vector_getter,
                       static_cast<void*>(&values), values.size());
    }

}

void PrefabGUI::CleanUp() {
    prefab_loaded_ = redengine::prefab();
    save_location = "";
    save_json = nlohmann::json();
    collider_index_ = 0;
}

PrefabGUI::PrefabGUI() {

}

void PrefabGUI::Draw(Shader *shader, const glm::mat4 &projection, const glm::mat4 &view) {
    main_menu_ ? MainMenu() : (void)0;
    create_new_prefab_ ? CreatePrefab() : (void)0;
    main_edit_menu_ ? MainEntityMenu() : (void)0;
    get_prefab_ ? LoadExistingPrefab() : (void)0;
    model_component_ ? ModelComponentMenu() : (void)0;
    transform_component_ ? TransformComponentMenu() : (void)0;
    physics_edit_menu ? PhysicsMainMenu() : (void)0;
    collider_edit_menu_ ? ColliderEditor() : (void)0;
    affordance_edit_menu ? AffordanceMenu() : (void)0;
    physics_menu_ ? PhysicsMainMenu() : (void)0;
    save_to ? SaveTo() : (void)0;


    if (prefab_loaded_.has_model) {
        glm::mat4 model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, prefab_loaded_.position_local);
        model_matrix = model_matrix * glm::mat4_cast(prefab_loaded_.rotation_local);
        model_matrix = glm::scale(model_matrix, prefab_loaded_.scale_local);
        shader->SetMat4("model", model_matrix);
        shader->SetBool("isAnimated", false);
        redengine::Engine::get().model_manager_.Draw(prefab_loaded_.model_id, shader, model_matrix);
    }
}

void PrefabGUI::MainMenu() {
    ImGui::SetNextWindowPos(ImVec2(0.5, 0.5), ImGuiCond_Always, ImVec2(-0.5, -0.5));
    ImGui::SetNextWindowSize(ImVec2(250, 500), 1);
    ImGui::Begin("Main Menu", &main_menu_,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    if (ImGui::Button("New Prefab", button_size_)) {
        main_menu_ = false;
        create_new_prefab_ = true;
    }
    if (ImGui::Button("Loaded Prefab", button_size_)) {
        get_prefab_ = true;
        main_menu_ = false;
    }
    if (ImGui::Button("Exit", button_size_)) {
        redengine::Engine::get().game_stack_.popTop();
    }
    ImGui::End();
}

void PrefabGUI::CreatePrefab() {
    static char prefab_name[50] = {'\0'};
    ImGui::SetNextWindowPos(ImVec2(0.5, 0.5), ImGuiCond_Always, ImVec2(-0.5, -0.5));
    ImGui::SetNextWindowSize(ImVec2(250, 500), 1);
    ImGui::Begin("Create New Prefab", &create_new_prefab_,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::InputText("Prefab Name", prefab_name, IM_ARRAYSIZE(prefab_name));
    if (ImGui::Button("Submit", button_size_)) {
        prefab_loaded_ = redengine::prefab();
        prefab_loaded_.name = std::string(prefab_name);
        main_edit_menu_ = true;
        create_new_prefab_ = false;
    }
    if (ImGui::Button("Exit", button_size_)) {
        main_menu_ = true;
        create_new_prefab_ = false;
    }
    ImGui::End();
}

void PrefabGUI::LoadExistingPrefab() {
    static int listbox_item_current_ = 0;
    auto list = redengine::Engine::get().GetPrefabRepo().GetPrefabList();
    ImGui::SetNextWindowPos(ImVec2(0.5, 0.5), ImGuiCond_Always, ImVec2(-0.5, -0.5));
    ImGui::SetNextWindowSize(ImVec2(250, 500), 1);
    ImGui::Begin("Load Prefab", &get_prefab_,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::ListBox("Listbox\n(single select)", &listbox_item_current_, list);

    if (ImGui::Button("Get Entity", button_size_)) {
        get_prefab_ = false;
        main_edit_menu_ = true;
        prefab_loaded_ = redengine::Engine::get().GetPrefabRepo().GetPrefabMut(list.at(
                static_cast<unsigned long>(listbox_item_current_)));
    }
    if (ImGui::Button("Exit", button_size_)) {
        main_menu_ = true;
        get_prefab_ = false;
    }
    ImGui::End();
}

void PrefabGUI::MainEntityMenu() {
    ImGui::SetNextWindowPos(ImVec2(0.5, 0.5), ImGuiCond_Always, ImVec2(-0.5, -0.5));
    ImGui::SetNextWindowSize(ImVec2(250, 500), 1);
    ImGui::Begin("Load Prefab", &main_edit_menu_,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Editing: %s", prefab_loaded_.name.c_str());
    if (ImGui::Button("Model Component", button_size_)) {
        model_component_ = true;
    }
    if (ImGui::Button("Transform Component", button_size_)) {
        transform_component_ = true;
    }
    if (ImGui::Button("Physics Component", button_size_)) {
        physics_menu_ = true;
    }
    if (ImGui::Button("Save and Submit", button_size_)) {
        redengine::Engine::get().GetPrefabRepo().InsertPrefab(prefab_loaded_);
        redengine::prefab::to_json(save_json, prefab_loaded_);
        save_to = true;
        //main_menu_ = true;
        main_edit_menu_ = false;
        CleanUp();
    }
    if (ImGui::Button("Close, Dont Save", button_size_)) {
        main_menu_ = true;
        main_edit_menu_ = false;
        CleanUp();
    }
    ImGui::End();
}

void PrefabGUI::ModelComponentMenu() {
    static char prefab_name[100] = {'\0'};
    static bool animate = false;
    ImGui::SetNextWindowPos(ImVec2(0.5, 0.5), ImGuiCond_Always, ImVec2(-0.5, -0.5));
    ImGui::SetNextWindowSize(ImVec2(500, 500), 1);
    ImGui::Begin("Set Model", &model_component_,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Please enter the relative address from the res/model/ folder.");
    ImGui::Text("Input should look like this: Chair/Chair.fbx");
    ImGui::NewLine();
    ImGui::InputText("Model Location: res/model/...", prefab_name, IM_ARRAYSIZE(prefab_name));
    ImGui::NewLine();
    ImGui::Checkbox("Enable Animation", &animate);
    if (ImGui::Button("Save and Submit", button_size_)) {
        auto &engine = redengine::Engine::get();
        model_component_ = false;
        prefab_loaded_.model_dir = "res/model/" + std::string(prefab_name);
        auto full_path = engine.GetBasePath() / "res" / "model" / std::filesystem::path{prefab_name};
        prefab_loaded_.model_id = engine.model_manager_.GetModelID(full_path);
        if (animate && engine.model_manager_.getModel(prefab_loaded_.model_id)->is_animated_) {
            prefab_loaded_.has_animation = true;
        }
        prefab_loaded_.has_model = true;
    }
    if (ImGui::Button("Close, Dont Save", button_size_)) {
        model_component_ = false;
    }
    ImGui::End();
}

static inline void ThreeButtonMenu(const std::string& val, const std::string& type, float &ref) {
    if (ImGui::Button(std::string("<<<<##" + type + val).c_str())) {
        ref += -100.0f;
    }
    ImGui::SameLine();
    if (ImGui::Button(std::string("<<<##" + type + val).c_str())) {
        ref += -10.0f;
    }
    ImGui::SameLine();
    if (ImGui::Button(std::string("<<##" + type + val).c_str())) {
        ref += -1.0f;
    }
    ImGui::SameLine();
    if (ImGui::Button(std::string("<##" + type + val).c_str())) {
        ref += -0.1f;
    }
    ImGui::SameLine();
    ImGui::Text("%s", val.c_str());
    ImGui::SameLine();
    if (ImGui::Button(std::string(">##" + type + val).c_str())) {
        ref += 0.1f;
    }
    ImGui::SameLine();
    if (ImGui::Button(std::string(">>##" + type + val).c_str())) {
        ref += 1.0f;
    }
    ImGui::SameLine();
    if (ImGui::Button(std::string(">>>##" + type + val).c_str())) {
        ref += 10.0f;
    }
    ImGui::SameLine();
    if (ImGui::Button(std::string(">>>>##" + type + val).c_str())) {
        ref += 100.0f;
    }
}

void PrefabGUI::TransformComponentMenu() {
    ImGui::SetNextWindowPos(ImVec2(0.5, 0.5), ImGuiCond_Always, ImVec2(-0.5, -0.5));
    ImGui::SetNextWindowSize(ImVec2(350, 500), 1);
    ImGui::Begin("Transform Model", &transform_component_,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Position from origin");
    ThreeButtonMenu("X", "pos", prefab_loaded_.position_local.x);
    ThreeButtonMenu("Y", "pos", prefab_loaded_.position_local.y);
    ThreeButtonMenu("Z", "pos", prefab_loaded_.position_local.z);
    ImGui::Text("Scale");
    ThreeButtonMenu("X", "scale", prefab_loaded_.scale_local.x);
    ThreeButtonMenu("Y", "scale", prefab_loaded_.scale_local.y);
    ThreeButtonMenu("Z", "scale", prefab_loaded_.scale_local.z);
    //TODO: Implement this
    ImGui::Text("Rotation");
    static glm::vec3 eulerRot = glm::eulerAngles(prefab_loaded_.rotation_local);
    ThreeButtonMenu("X", "rotation", eulerRot.x);
    ThreeButtonMenu("Y", "rotation", eulerRot.y);
    ThreeButtonMenu("Z", "rotation", eulerRot.z);
    prefab_loaded_.rotation_local = glm::quat(eulerRot);
    ImGui::Text("To be implemented");
    if (ImGui::Button("Save and Submit", button_size_)) {
        auto &engine = redengine::Engine::get();
        transform_component_ = false;
        eulerRot = glm::vec3();
    }
    ImGui::End();
}
// TODO: Implement this
void PrefabGUI::PhysicsMainMenu() {
    static int listbox_item_current_ = 0;
    auto list = prefab_loaded_.GetColliderList();
    ImGui::Begin("Physics Menu", &physics_menu_,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Checkbox("Has physics: ", &prefab_loaded_.has_physics);
    ImGui::Checkbox("Is static: ", &prefab_loaded_.is_static);
    ImGui::NewLine();
    ImGui::ListBox("Collider to Edit", &listbox_item_current_, list);
    if (ImGui::Button("Edit Collider", button_size_)) {
        if (static_cast<size_t>(listbox_item_current_) < list.size()) {
            collider_index_ = static_cast<size_t>(listbox_item_current_);
            collider_ = prefab_loaded_.colliders_.at(collider_index_);
            collider_index_set_ = true;
            collider_edit_menu_ = true;
            //physics_menu_ = false;
        }
    }
    if (ImGui::Button("New Collider", button_size_)) {
            collider_index_set_ = false;
            collider_ = redengine::Collider();
            collider_edit_menu_ = true;
    }
    if (ImGui::Button("Save and Submit", button_size_)) {
        save_to = true;
        //main_menu_ = true;
        physics_menu_ = false;
    }
    if (ImGui::Button("Close, Dont Save", button_size_)) {
        main_menu_ = true;
        physics_menu_ = false;
    }
    ImGui::End();
}

void PrefabGUI::ColliderEditor() {
    static char prefab_name[50] = {'\0'};
    static glm::vec3 eulerRot;
    static std::vector<std::string> list = {"Box", "Sphere", "Capsule"};
    static int selection_choice = 0;
    //TODO: Make a vistor for collider.
    ImGui::Begin("Collider Edit Menu", &collider_edit_menu_,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::InputText("Prefab Name", prefab_name, IM_ARRAYSIZE(prefab_name));
    ImGui::ListBox("Select Collider: ", &selection_choice, list);
    ImGui::Text("Collider Position");
    ThreeButtonMenu("X", "pos-col", collider_.position_local.x);
    ThreeButtonMenu("Y", "pos-col", collider_.position_local.y);
    ThreeButtonMenu("Z", "pos-col", collider_.position_local.z);
    ImGui::Text("Collider Rotation");
    eulerRot = glm::eulerAngles(collider_.rotation_local);
    ThreeButtonMenu("X", "rotation-col", eulerRot.x);
    ThreeButtonMenu("Y", "rotation-col", eulerRot.y);
    ThreeButtonMenu("Z", "rotation-col", eulerRot.z);
    collider_.rotation_local = glm::quat(eulerRot);
    if (selection_choice == 0) {

    }
    if (selection_choice == 1) {

    }
    if (selection_choice == 2) {

    } else {

    }
    if (ImGui::Button("Save and Submit", button_size_)) {
        collider_edit_menu_ = false;
        eulerRot = glm::vec3();
    }
    ImGui::End();
}

// TODO: Implement this
void PrefabGUI::AffordanceMenu() {

}

void PrefabGUI::SaveTo() {
    static char location[100] = {'\0'};
    ImGui::SetNextWindowPos(ImVec2(0.5, 0.5), ImGuiCond_Always, ImVec2(-0.5, -0.5));
    ImGui::SetNextWindowSize(ImVec2(250, 500), 1);
    ImGui::Begin("Save to...", &save_to,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Path relative from prefab. Please include extension (.json)");
    ImGui::InputText("Location", location, IM_ARRAYSIZE(location));
    if (ImGui::Button("Save", button_size_)) {
        auto path = redengine::Engine::get().GetBasePath();
        path = path / "res" / "prefab" / location;
        std::ofstream myfile;
        myfile.open(path);
        myfile << save_json;
        myfile.close();
        save_to = false;
        main_menu_ = true;
    }
    if (ImGui::Button("Dont Save", button_size_)) {
        save_to = false;
        main_menu_ = true;
    }
    ImGui::End();
}

redengine::prefab PrefabGUI::GetPrefab() const {
    return prefab_loaded_;
}
