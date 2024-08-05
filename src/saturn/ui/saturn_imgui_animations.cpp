#include "saturn_imgui_animations.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "saturn/saturn.h"
#include "saturn/saturn_colors.h"
#include "saturn/saturn_models.h"
#include "saturn/saturn_textures.h"
#include "saturn/saturn_animations.h"
#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"
#include "saturn/libs/imgui/imgui_impl_opengl3_loader.h"
extern "C" {
    #include "pc/pc_main.h"
    #include "pc/gfx/gfx_pc.h"
    #include "pc/djui/djui.h"
    #include "pc/djui/djui_chat_box.h"
    #include "pc/djui/djui_console.h"
    #include "pc/djui/djui_interactable.h"
    #include "pc/network/network_player.h"
    #include "game/object_collision.h"
    #include "game/camera.h"
    #include "game/mario.h"
    #include "engine/math_util.h"
    #include "engine/behavior_script.h"
}

#include <SDL2/SDL.h>

static char animSearchTerm[128];
PlutoAnim current_panim;

void OpenAnimationsMenu() {
    if (ImGui::BeginTabBar("###animation_tab_bar")) {
        if (ImGui::BeginTabItem("Vanilla")) {
            enable_custom_anim = false;
            ImGui::SetNextItemWidth(200);
            if (ImGui::BeginCombo("###v_anim_combo", saturn_animations[selected_anim_index], ImGuiComboFlags_None)) {
                ImGui::InputTextWithHint("###anim_search", "Search...", animSearchTerm, IM_ARRAYSIZE(animSearchTerm), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsUppercase);
                ImGui::Separator();
                for (int n = 0; n < 209; n++) {
                    const bool is_selected = (selected_anim_index == n);

                    if (std::string(saturn_animations[n]).find(animSearchTerm) == std::string::npos &&
                        animSearchTerm != "")
                        continue;

                    if (ImGui::Selectable(saturn_animations[n], is_selected))
                        selected_anim_index = n;

                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("PAnim")) {
            enable_custom_anim = true;
            if (current_panim.Length == -1) {
                current_panim = LoadPAnim("dynos/anims/" + pluto_animations_list[0]);
                loop_anim = current_panim.Looping;
            }

            ImGui::BeginChild("###p_anim_select", ImVec2(200, 125), ImGuiChildFlags_Border);
            ImGui::SetNextItemWidth(200);
            ImGui::InputTextWithHint("###anim_search", "Search...", animSearchTerm, IM_ARRAYSIZE(animSearchTerm), ImGuiInputTextFlags_AutoSelectAll);
            ImGui::Separator();
            for (int n = 0; n < pluto_animations_list.size(); n++) {
                const bool is_selected = (selected_panim_index == n);

                if (pluto_animations_list[n].find(animSearchTerm) == std::string::npos &&
                    animSearchTerm != "")
                    continue;

                if (ImGui::Selectable(pluto_animations_list[n].c_str(), is_selected)) {
                    selected_panim_index = n;
                    current_panim = LoadPAnim("dynos/anims/" + pluto_animations_list[n]);
                    loop_anim = current_panim.Looping;
                }

                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndChild();

            ImGui::BeginChild("###p_metadata", ImVec2(200, 48), ImGuiChildFlags_Border);
            ImGui::Text("%s", current_panim.Name.c_str());
            ImGui::TextDisabled("@ %s", current_panim.Author.c_str());
            ImGui::EndChild();

            ImGui::EndTabItem();
        }
        if (ImGui::IsItemClicked())
            pluto_animations_list = GetPAnimList("dynos/anims");

        if (ImGui::Checkbox("Override Animation", &override_anim))
            if (!override_anim) set_character_animation(&gMarioStates[0], CHAR_ANIM_IDLE_HEAD_LEFT);
        ImGui::Separator();

        ImGui::BeginDisabled(!override_anim);
        if (enable_custom_anim && override_anim) ImGui::Text("Now Playing: %s", current_panim.Name.c_str());
        else ImGui::Text("Now Playing: %s", saturn_animations[gMarioStates[0].marioObj->header.gfx.animInfo.animID]);
        ImGui::BeginDisabled(!pause_anim);
        ImGui::SliderInt("###animation_frame", &paused_frame, gMarioStates[0].marioObj->header.gfx.animInfo.curAnim->loopStart, gMarioStates[0].marioObj->header.gfx.animInfo.curAnim->loopEnd-1, "frame %d", ImGuiSliderFlags_AlwaysClamp);
        ImGui::EndDisabled();
        ImGui::Checkbox("Paused", &pause_anim);
        ImGui::EndDisabled();
        ImGui::SameLine(); ImGui::Checkbox("Hang", &hang_anim);
        ImGui::BeginDisabled(hang_anim);
        ImGui::SameLine(); ImGui::Checkbox("Loop", &loop_anim);
        ImGui::EndDisabled();
        ImGui::EndTabBar();
    }
}