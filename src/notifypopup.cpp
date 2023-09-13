#include "pch.h"
#include "notifypopup.h"

void NotifiyPopup::AddToQueue(std::string&& message) {
    m_Messages.push_back({std::move(message), 0});
}

bool NotifiyPopup::IsMessagesInQueue() {
    return m_Messages.size() != 0;
}

size_t NotifiyPopup::GetQueueSize() {
    return m_Messages.size();
}

void NotifiyPopup::Draw() {
    static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                                           ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
    static ImVec2 pivot{ 0.0f, 0.0f };

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 pos{ m_fOffset, m_fOffset };
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, pivot);
    ImGui::SetNextWindowBgAlpha(0.75f);

    if (IsMessagesInQueue()) {
        if (ImGui::Begin("Notification", nullptr, window_flags)) {
            ImGui::Text("Notifications");
            ImGui::Separator();
            ImGui::Spacing();
            double curTimer = ImGui::GetTime();

            size_t popCount = 0;
            for (size_t i = 0; i < m_nMaxDisplayCount; ++i) {
                if (i >= m_Messages.size()) {
                    break;
                }

                if (m_Messages[i].timer == 0) {
                    m_Messages[i].timer = curTimer;
                }

                ImGui::TextUnformatted(m_Messages[i].message.c_str());

                if ((curTimer - m_Messages[i].timer) > m_fDuration) {
                    ++popCount;
                }
            }

            for (size_t i = 0; i != popCount; ++i) {
                m_Messages.pop_front();
            }
            ImGui::End();
        }
    }
}