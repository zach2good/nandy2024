#include "ui.h"

#include <SDL.h>
#include <imgui.h>

#include "logic/logic_sim.h"
#include "ui/applog.h"

void UI::draw(LogicSim& logicSim, SDL_Texture* texture)
{
    // TODO: Instead of getting fresh information from logicSim every time there is a UI update, we should only do it 2-4 times per second.
    //       This includes generating the texture, and getting the stats.

    // TODO: Break out each UI section into its own function and make resizing reactive instead of hard-coding sizes.

    auto& io         = ImGui::GetIO();
    auto  mousePos   = ImGui::GetMousePos();
    auto  mouseDelta = io.MouseDelta;
    float canvasX    = 0.0f;
    float canvasY    = 0.0f;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(Config::kScreenWidth, Config::kScreenHeight));
    ImGui::Begin("MainWindow", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_MenuBar);
    {
        // Menu bar
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Close", "Ctrl+W"))
                {
                    spdlog::info("Close menu item pressed");
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Left
        ImGui::BeginChild("LeftPane", ImVec2(150, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX);
        {
            ImGui::Button("NAND\n(Drag)", ImVec2(50, 50));
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
            {
                ImGui::SetDragDropPayload("DND_DEMO", "DND_DEMO", sizeof("DND_DEMO"));

                // Preview tooltip
                ImGui::Text("Drag and drop me!");

                ImGui::EndDragDropSource();
            }
            ImGui::EndChild();
        }

        // Middle
        ImGui::SameLine();
        ImGui::BeginGroup();
        {
            ImGui::BeginChild("CanvasPane", ImVec2(Config::kCanvasWidth, Config::kCanvasHeight + 60), ImGuiChildFlags_Border);
            {
                ImGui::BeginChild("ButtonsPane", ImVec2(Config::kCanvasWidth - 15, 40), ImGuiChildFlags_Border);
                {
                    if (ImGui::Button("Step"))
                    {
                        spdlog::info("Step button pressed");
                        if (!logicSim.running())
                        {
                            logicSim.step();
                        }
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Run"))
                    {
                        spdlog::info("Run button pressed");
                        logicSim.run();
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Stop"))
                    {
                        spdlog::info("Stop button pressed");
                        logicSim.stop();
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Zoom +"))
                    {
                        spdlog::info("Zoom + button pressed");
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Zoom -"))
                    {
                        spdlog::info("Zoom - button pressed");
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("<"))
                    {
                        spdlog::info("< button pressed");
                    }
                    ImGui::SameLine();

                    if (ImGui::Button(">"))
                    {
                        spdlog::info("> button pressed");
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("^"))
                    {
                        spdlog::info("^ button pressed");
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("v"))
                    {
                        spdlog::info("v button pressed");
                    }

                    ImGui::EndChild(); // "ButtonsPane"
                }

                // Canvas Image w/ drag and drop
                // 0,0 is top left corner of the window
                {
                    ImGui::Image(texture, ImVec2(Config::kCanvasWidth, Config::kCanvasHeight));
                    canvasX = 0.0f;
                    canvasY = 0.0f;
                    if (ImGui::IsItemHovered())
                    {
                        canvasX = ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x - ImGui::GetScrollX();
                        canvasY = Config::kCanvasHeight + ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y - ImGui::GetScrollY();
                    }
                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO"))
                        {
                            spdlog::info("ImGui drag and drop accepted! Cursor: {}, {}", canvasX, canvasY);
                        }
                        ImGui::EndDragDropTarget();
                    }
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                    {
                        spdlog::info("x: {}, y: {}, delta: {}, {}", canvasX, canvasY, ImGui::GetMouseDragDelta().x, ImGui::GetMouseDragDelta().y);
                    }
                }
                ImGui::EndChild(); // "CanvasPane"
            }

            // Logging/stats
            ImGui::BeginChild("LoggingPane", ImVec2(Config::kCanvasWidth, 0), ImGuiChildFlags_Border);
            {
                AppLog::get().Draw("LoggingPane");
                ImGui::EndChild(); // "LoggingPane"
            }
            ImGui::EndGroup();
        }

        // Right
        ImGui::SameLine();
        ImGui::BeginChild("Right", ImVec2(0, 0), ImGuiChildFlags_Border);
        {
            ImGui::Text(fmt::format("Gates count: {}", logicSim.gates.size()).c_str());
            ImGui::Text(fmt::format("Nodes count: {}", logicSim.nodes.size()).c_str());
            ImGui::Text(fmt::format("Step count: {}", logicSim.stepCount()).c_str());
            ImGui::Text(fmt::format("Step time: {:L}us", logicSim.stepTime()).c_str());
            ImGui::Text(fmt::format("Hz: {}", 1000000.0f / logicSim.stepTime()).c_str());
            ImGui::Text(fmt::format("Ops total: {}", logicSim.opsTotalCount()).c_str());
            ImGui::Text(fmt::format("Ops per step: {}", logicSim.opsPerStep()).c_str());

            ImGui::Separator();

            ImGui::Text(fmt::format("Mouse pos: {}, {}", mousePos.x, mousePos.y).c_str());
            ImGui::Text(fmt::format("Mouse delta: {}, {}", mouseDelta.x, mouseDelta.y).c_str());
            if (canvasX == 0.0f && canvasY == 0.0f)
            {
                ImGui::Text("Canvas pos: None");
            }
            else
            {
                ImGui::Text(fmt::format("Canvas pos: {}, {}", canvasX, canvasY).c_str());
            }

            ImGui::Separator();

            /*
            for (auto* node : logicSim.inputNodes)
            {
                ImGui::Text(fmt::format("Input Node: {}", node->value).c_str());
            }

            for (auto* node : logicSim.outputNodes)
            {
                ImGui::Text(fmt::format("Output Node: {}", node->value).c_str());
            }

            for (auto* node : logicSim.clockNodes)
            {
                ImGui::Text(fmt::format("Clock Node: {}", node->value).c_str());
            }
            */

            ImGui::EndChild();
        }

        ImGui::End(); // "MainWindow"
    }
}