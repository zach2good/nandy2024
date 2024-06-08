#pragma once

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

#include "window_renderer.h"

#include "applog_sink.h"

class UIRenderer
{
public:
    UIRenderer(WindowRenderer* windowRenderer);
    ~UIRenderer();

    void handleEvent(SDL_Event const& event);
    void clear();
    void draw();
    void present();

private:
    WindowRenderer* m_WindowRenderer;
};

inline UIRenderer::UIRenderer(WindowRenderer* windowRenderer)
: m_WindowRenderer(windowRenderer)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // Disable .ini file
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(m_WindowRenderer->sdlWindow(), m_WindowRenderer->sdlRenderer());
    ImGui_ImplSDLRenderer2_Init(m_WindowRenderer->sdlRenderer());
}

inline UIRenderer::~UIRenderer()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

inline void UIRenderer::handleEvent(SDL_Event const& event)
{
    ImGui_ImplSDL2_ProcessEvent(&event);
}

inline void UIRenderer::clear()
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

inline void UIRenderer::draw()
{
    auto& io         = ImGui::GetIO();
    auto  mousePos   = ImGui::GetMousePos();
    auto  mouseDelta = io.MouseDelta;

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
            ImGui::Text("Drag n' Drop");

            ImGui::Separator();

            ImGui::Button("NAND", ImVec2(50, 50));
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
            {
                // m_UIState = UIState::UIDragging;
                ImGui::SetDragDropPayload("NAND", "NAND", sizeof("NAND"));

                // Preview tooltip
                ImGui::Text("Drag and drop me!");

                ImGui::EndDragDropSource();
            }

            ImGui::SameLine();

            ImGui::Button("Node", ImVec2(50, 50));
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
            {
                // m_UIState = UIState::UIDragging;
                ImGui::SetDragDropPayload("NODE", "NODE", sizeof("NODE"));

                // Preview tooltip
                ImGui::Text("Drag and drop me!");

                ImGui::EndDragDropSource();
            }

            // Break

            ImGui::Button("CLK", ImVec2(50, 50));
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
            {
                // m_UIState = UIState::UIDragging;
                ImGui::SetDragDropPayload("CLK", "CLK", sizeof("CLK"));

                // Preview tooltip
                ImGui::Text("Drag and drop me!");

                ImGui::EndDragDropSource();
            }

            // Break
            ImGui::Separator();

            ImGui::Text("User Components");

            // Break
            ImGui::Separator();

            ImGui::Text("Non-functional Components");

            ImGui::Button("Text", ImVec2(50, 50));
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
            {
                // m_UIState = UIState::UIDragging;
                ImGui::SetDragDropPayload("TEXT", "TEXT", sizeof("TEXT"));

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
                    /*
                    if (ImGui::Button("Step"))
                    {
                        spdlog::info("Step button pressed");
                        if (!logicSim.running())
                        {
                            logicSim.step();
                        }
                    }
                    ImGui::SameLine();

                    if (!logicSim.running())
                    {
                        if (ImGui::Button("Run"))
                        {
                            spdlog::info("Run button pressed");
                            logicSim.run();
                        }
                    }
                    else
                    {
                        if (ImGui::Button("Stop"))
                        {
                            spdlog::info("Stop button pressed");
                            // logicSim.stop();
                        }
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Zoom +"))
                    {
                        spdlog::info("Zoom + button pressed: {}", m_Zoom);
                        m_Zoom += 0.1f;
                        m_Zoom = std::clamp(m_Zoom, 0.025f, 3.0f);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Zoom -"))
                    {
                        spdlog::info("Zoom - button pressed: {}", m_Zoom);
                        m_Zoom -= 0.1f;
                        m_Zoom = std::clamp(m_Zoom, 0.025f, 3.0f);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("<"))
                    {
                        spdlog::info("< button pressed");
                        m_OffsetX -= 100.0f * m_Zoom;
                    }
                    ImGui::SameLine();

                    if (ImGui::Button(">"))
                    {
                        spdlog::info("> button pressed");
                        m_OffsetX += 100.0f * m_Zoom;
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("^"))
                    {
                        spdlog::info("^ button pressed");
                        m_OffsetY -= 100.0f * m_Zoom;
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("v"))
                    {
                        spdlog::info("v button pressed");
                        m_OffsetY += 100.0f * m_Zoom;
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Reset"))
                    {
                        spdlog::info("Reset button pressed");
                        m_OffsetX = 0.0f;
                        m_OffsetY = 0.0f;
                        m_Zoom    = 1.0f;
                        logicSim.reset();
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Print Json"))
                    {
                        spdlog::info("Print Json button pressed");
                        spdlog::info(logicSim.toJson());
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Save & Rebuild from Json"))
                    {
                        spdlog::info("Rebuild from Json button pressed");
                        logicSim.saveToFile("circuit.json");
                        logicSim.loadFromFile("circuit.json");
                    }
                    */
                    ImGui::EndChild(); // "ButtonsPane"
                }

                // Canvas Image w/ drag and drop
                // 0,0 is top left corner of the window
                // m_CursorX = 0.0f;
                // m_CursorY = 0.0f;
                ImGui::Image(m_WindowRenderer->sdlCanvasTexture(), ImVec2(Config::kCanvasWidth, Config::kCanvasHeight));
                /*
                {
                    auto rect         = ImGui::GetItemRectMin();
                    m_IsCanvasHovered = ImGui::IsItemHovered();
                    if (m_IsCanvasHovered)
                    {
                        // Start building viewmodel information
                        m_CursorX = ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x - ImGui::GetScrollX();
                        m_CursorY = Config::kCanvasHeight + ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y - ImGui::GetScrollY();
                        m_Zoom    = std::clamp(m_Zoom - io.MouseWheel * 0.1f, 0.025f, 3.0f);
                    }
                    if (ImGui::BeginDragDropTarget())
                    {
                        // This is the only UI -> Canvas interaction!
                        auto x = (m_CursorX - m_OffsetX) / m_Zoom - 50.0f; // TODO: Handle this offset more gracefully
                        auto y = (m_CursorY - m_OffsetY) / m_Zoom - 50.0f; // TODO: Handle this offset more gracefully
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NAND"))
                        {
                            logicSim.addGate(x, y);
                            m_UIState = UIState::None;
                        }
                        else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE"))
                        {
                            logicSim.addNode(x + 50.0f, y + 50.0f); // TODO
                            m_UIState = UIState::None;
                        }
                        else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CLK"))
                        {
                            logicSim.addClockNode(x + 50.0f, y + 50.0f); // TODO
                            m_UIState = UIState::None;
                        }
                        else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXT"))
                        {
                            // logicSim.addTextNote(x + 50.0f, y + 50.0f);
                            m_UIState = UIState::None;
                        }

                        ImGui::EndDragDropTarget();
                    }
                }
                */
                ImGui::EndChild(); // "CanvasPane"
            }

            // Logging/stats
            ImGui::BeginChild("LoggingPane", ImVec2(Config::kCanvasWidth, 0), ImGuiChildFlags_Border);
            {
                AppLog::get().Draw("LoggingPane");
                ImGui::EndChild(); // "LoggingPane"
            }
            ImGui::EndGroup(); // "Middle"
        }

        // Right
        ImGui::SameLine();
        ImGui::BeginChild("Right", ImVec2(0, 0), ImGuiChildFlags_Border);
        {
            /*
            ImGui::Text("%s", fmt::format("Components count: {}", logicSim.components.size()).c_str());
            ImGui::Text("%s", fmt::format("Gates count: {}", logicSim.gates.size()).c_str());
            ImGui::Text("%s", fmt::format("Nodes count: {}", logicSim.nodes.size()).c_str());
            ImGui::Text("%s", fmt::format("Step count: {}", logicSim.stepCount()).c_str());

            auto ns = logicSim.stepTime();
            if (ns > 1000000)
            {
                ImGui::Text("%s", fmt::format("Step time: {:.2f}ms", ns / 1000000.0f).c_str());
            }
            else if (ns > 1000)
            {
                ImGui::Text("%s", fmt::format("Step time: {:.2f}us", ns / 1000.0f).c_str());
            }
            else
            {
                ImGui::Text("%s", fmt::format("Step time: {:.2f}ns", ns).c_str());
            }

            auto Hz = 1000000000.0f / logicSim.stepTime(); // From nano to Hz
            if (Hz > 1000000.0f)
            {
                ImGui::Text("%s", fmt::format("Hz: {:.2f}MHz", Hz / 1000000.0f).c_str());
            }
            else if (Hz > 1000.0f)
            {
                ImGui::Text("%s", fmt::format("Hz: {:.2f}kHz", Hz / 1000.0f).c_str());
            }
            else
            {
                ImGui::Text("%s", fmt::format("Hz: {:.2f}Hz", Hz).c_str());
            }

            ImGui::Text("%s", fmt::format("Ops total: {}", logicSim.opsTotalCount()).c_str());
            ImGui::Text("%s", fmt::format("Ops per step: {}", logicSim.opsPerStep()).c_str());

            ImGui::Separator();

            ImGui::Text("%s", fmt::format("Mouse pos: {}, {}", mousePos.x, mousePos.y).c_str());
            ImGui::Text("%s", fmt::format("Mouse delta: {}, {}", mouseDelta.x, mouseDelta.y).c_str());
            if (m_CursorX == 0.0f && m_CursorY == 0.0f)
            {
                ImGui::Text("Canvas pos: None");
            }
            else
            {
                ImGui::Text("%s", fmt::format("Canvas pos: {}, {}", m_CursorX, m_CursorY).c_str());
            }

            ImGui::Separator();

            ImGui::Text("%s", fmt::format("UI State: {}", UIStateToString(m_UIState)).c_str());

            ImGui::Separator();
            */

            ImGui::EndChild();
        }

        ImGui::End(); // "MainWindow"
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}

inline void UIRenderer::present()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Render();
    SDL_RenderSetScale(m_WindowRenderer->sdlRenderer(), io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    SDL_SetRenderDrawColor(m_WindowRenderer->sdlRenderer(), (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
    SDL_RenderSetScale(m_WindowRenderer->sdlRenderer(), io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}
