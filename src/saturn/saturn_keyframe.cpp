#include "saturn_keyframe.h"

#include "libs/imgui/imgui.h"
#include "libs/imgui/imgui_internal.h"
#include "libs/imgui/imgui_neo_sequencer.h"

#include <algorithm>
#include <unordered_set>
#include <math.h>

std::map<std::string, Timeline> timelines = {};
int timeline_position;
bool timeline_is_playing;
static int prev_position;

bool TimelineButton(std::string name, float* ptr) {
    return TimelineButton<float>(name, ptr,
        [](float* out, float* a, float* b, float x) { *out = (*b - *a) * x + *a; },
        [](float* value) { ImGui::BeginTooltip(); ImGui::Text("%.3f", *value); ImGui::EndTooltip(); }
    );
}

bool TimelineButton(std::string name, int* ptr) {
    return TimelineButton<int>(name, ptr,
        [](int* out, int* a, int* b, float x) { *out = (*b - *a) * x + *a; },
        [](int* value) { ImGui::BeginTooltip(); ImGui::Text("%d", *value); ImGui::EndTooltip(); }
    );
}

bool TimelineButton(std::string name, bool* ptr) {
    return TimelineButton<bool>(name, ptr,
        [](bool* out, bool* a, bool* b, float x) { *out = x < 1 ? *a : *b; },
        [](bool* value) { ImGui::BeginTooltip(); ImGui::Checkbox("##preview_checkbox", value); ImGui::EndTooltip(); }
    );
}

bool TimelineButton(std::string name, Timeline timeline) {
    auto it = timelines.find(name);
    bool clicked = ImGui::Button(it == timelines.end() ? "~" : "X");

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        if (it == timelines.end()) ImGui::Text("Automate");
        else ImGui::Text("Remove Timeline");
        ImGui::EndTooltip();
    }
    
    if (!clicked) return false;

    if (it == timelines.end()) {
        // add a starting keyframe
        Keyframe start = Keyframe();
        memcpy(start.get(timeline.size), timeline.ptr, timeline.size); // use the current value
        timeline.keyframes.push_back(start);
        timelines.insert({ name, timeline });
    }
    else timelines.erase(it);

    return true;
}

static float PowerTransform(float x, float power, InterpolationType type) {
    switch (type) {
        case In: x = powf(x, power); break;
        case Out: x = 1 - powf(1 - x, power); break;
        case InOut: x = x < 0.5
            ? powf(2, power - 1) * powf(x, power)
            : 1 - powf(-2 * x + 2, power) / 2; break;
        case Unknown: assert(false);
    }
    return x;
}

static Keyframe* RunTimeline(Timeline& timeline, void* out) {
    // find the keyframes between the current timeline position
    Keyframe* curr_kf = &timeline.keyframes[0];
    Keyframe* next_kf = &timeline.keyframes[0];
    for (Keyframe& kf : timeline.keyframes) {
        next_kf = &kf;
        if (kf.position > timeline_position) break;
        curr_kf = &kf;
    }

    // get linear interpolation position
    float pos = next_kf->position == curr_kf->position
        ? 0
        : PowerTransform(
            (timeline_position - curr_kf->position) / (float)(next_kf->position - curr_kf->position),
            curr_kf->interp_power, curr_kf->interp_type
        );
    pos = PowerTransform(pos, curr_kf->interp_power, curr_kf->interp_type);


    // interpolate and apply
    void* a = curr_kf->get(timeline.size);
    void* b = next_kf->get(timeline.size);
    timeline.interpolate(out, a, b, pos);

    return curr_kf;
}

static void SortKeyframes(std::vector<Keyframe>& keyframes) {
    std::sort(keyframes.begin(), keyframes.end(), [](Keyframe& a, Keyframe& b) {
        return a.position < b.position; 
    });
}

void UpdateTimelines() {
    for (auto& tl_pair : timelines) {
        Timeline& timeline = tl_pair.second;

        char out[timeline.size];
        Keyframe* kf = RunTimeline(timeline, out);

        if (prev_position != timeline_position)
            // playhead moved; apply interpolated value
            memcpy(timeline.ptr, out, timeline.size);

        else if (!timeline.compare(timeline.ptr, out)) {
            // value is different than what we expect
            if (kf->position == timeline_position)
                // a keyframe exists at the current timeline position; overwrite its value
                memcpy(kf->get(timeline.size), timeline.ptr, timeline.size);
            else {
                // create a new keyframe with the value
                timeline.keyframes.push_back(Keyframe());
                kf = &timeline.keyframes.back();
                kf->position = timeline_position;
                memcpy(kf->get(timeline.size), timeline.ptr, timeline.size);

                SortKeyframes(timeline.keyframes);
            }
        }
    }

    prev_position = timeline_position;
    if (timeline_is_playing) timeline_position++;
}

void RenderTimelineWidget() {
    static int start = 0, end = 60;
    static std::vector<Keyframe*> selected_keyframes;
    static std::vector<Keyframe>* keyframe_vector;
    bool open_popup = false;

    if (ImGui::BeginPopup("##keyframe_menu")) {
        // find common interpolation power and 
        bool unk_power = false;
        float* power = &selected_keyframes[0]->interp_power;
        InterpolationType interp = selected_keyframes[0]->interp_type;
        for (Keyframe* kf : selected_keyframes) {
            if (kf->interp_power != *power) unk_power = true;
            if (kf->interp_type != interp) interp = Unknown;
        }

        bool modified = false;
        auto modify = [&](std::function<void(Keyframe*)> func) {
            modified = true;
            for (Keyframe* kf : selected_keyframes)
                func(kf);
        };

        if (ImGui::MenuItem("Delete")) {
            for (Keyframe* kf : selected_keyframes)
                kf->position = -1;
            keyframe_vector->erase(std::remove_if(
                keyframe_vector->begin(), keyframe_vector->end(),
                [](Keyframe& x) { return x.position == -1; }
            ), keyframe_vector->end());
        }
        ImGui::MenuItem("Close");
        ImGui::SeparatorText("Interpolation");
        ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
        if (ImGui::MenuItem("In", nullptr, interp == In)) modify([&](Keyframe* kf) { interp = kf->interp_type = In; });
        if (ImGui::MenuItem("Out", nullptr, interp == Out)) modify([&](Keyframe* kf) { interp = kf->interp_type = Out; });
        if (ImGui::MenuItem("InOut", nullptr, interp == InOut)) modify([&](Keyframe* kf) { interp = kf->interp_type = InOut; });
        ImGui::PopItemFlag();
        ImGui::Separator();
        if (ImGui::SliderFloat("Power", power, 0, 4, unk_power ? "???" : "%.3f"))
            modify([&](Keyframe* kf) { unk_power = false; kf->interp_power = *power; });

        // draw the interpolation graph
        float values[101];
        for (int i = 0; i <= 100; i++) {
            values[i] = unk_power || interp == Unknown ? 0 : PowerTransform(i / 100.f, *power, interp);
        }
        ImGui::PlotLines("##keyframe_graph", values, 101, 0, nullptr, 0, 1, { 0, 100 });

        // if modified, set prev_position to -1 so that the value gets updated rather than a new keyframe being created
        if (modified) prev_position = -1;

        ImGui::EndPopup();
    }
    
    if (ImGui::BeginNeoSequencer("Sequencer", &timeline_position, &start, &end, ImGui::GetWindowSize(),
        ImGuiNeoSequencerFlags_EnableSelection |
        ImGuiNeoSequencerFlags_AllowLengthChanging |
        ImGuiNeoSequencerFlags_Selection_EnableDragging |
        ImGuiNeoSequencerFlags_Selection_EnableDeletion
    )) {
        for (auto& tl_pair : timelines) {
            if (!ImGui::BeginNeoTimelineEx(tl_pair.first.c_str())) continue;

            Timeline& timeline = tl_pair.second;
            bool updated = false;
            for (auto& kf : timeline.keyframes) {
                int prev_kfpos = kf.position;
                ImGui::NeoKeyframe(&kf.position);

                // keyframe move checks
                if (prev_kfpos == 0) kf.position = 0; // make the initial keyframe not be able to be moved
                else if (kf.position < 1) kf.position = 1; // a keyframe position cannot be <1 besides the initial one
                if (prev_kfpos != kf.position) updated = true;

                if (ImGui::IsNeoKeyframeRightClicked()) {
                    keyframe_vector = &timeline.keyframes;
                    selected_keyframes.clear();
                    selected_keyframes.push_back(&kf);
                    ImGui::FrameIndexType selected[ImGui::GetNeoKeyframeSelectionSize()];
                    ImGui::GetNeoKeyframeSelection(selected);
                    for (ImGui::FrameIndexType index : selected) 
                        if (&timeline.keyframes[index] != &kf)
                            selected_keyframes.push_back(&*std::find_if(
                                timeline.keyframes.begin(), timeline.keyframes.end(),
                                [&](Keyframe& kf){ return kf.position == index; }
                            ));
                    open_popup = true;
                }
                if (ImGui::IsNeoKeyframeHovered()) {
                    if (timeline.on_hover) timeline.on_hover(kf.get(timeline.size));
                }
            }

            if (updated) {
                // update value and sort keyframes
                prev_position = -1;
                SortKeyframes(timeline.keyframes);
            }
            
            ImGui::EndNeoTimeLine();
        }
        ImGui::EndNeoSequencer();
    }

    if (open_popup) ImGui::OpenPopup("##keyframe_menu");
}
