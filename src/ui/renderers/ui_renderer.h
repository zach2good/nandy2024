#pragma once

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

#include "window_renderer.h"

#include "applog_sink.h"

#include "ui/actions/action.h"
#include "ui/actions/ui_canvas_hovered_action.h"
#include "ui/actions/ui_close_requested_action.h"
#include "ui/actions/ui_drag_drop_action.h"
#include "ui/actions/ui_keypress_action.h"
#include "ui/actions/ui_mouse_down_action.h"
#include "ui/actions/ui_mouse_moved_action.h"
#include "ui/actions/ui_mouse_up_action.h"
#include "ui/actions/ui_mouse_wheel_action.h"
#include "ui/actions/ui_sim_control_action.h"

#include <vector>

class UIRenderer final
{
public:
    UIRenderer(WindowRenderer* windowRenderer);
    ~UIRenderer();

    void handleEvent(SDL_Event const& event);
    void clear();
    auto draw() -> std::vector<std::unique_ptr<Action>>;
    void present();

private:
    WindowRenderer* m_WindowRenderer;

    // State
    bool m_IsHoveringCanvas = false;
    bool m_IsMouseDown      = false;
    f32  m_MouseWheel       = 0.0f;
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

inline auto UIRenderer::draw() -> std::vector<std::unique_ptr<Action>>
{
    std::vector<std::unique_ptr<Action>> actions;

    auto& io            = ImGui::GetIO();
    io.WantCaptureMouse = true;

    const auto mousePos      = ImGui::GetMousePos();
    const auto mouseDelta    = io.MouseDelta;
    const bool validPosition = mousePos.x >= 0 && mousePos.y >= 0 && mousePos.x < Config::kScreenWidth && mousePos.y < Config::kScreenHeight;
    const bool hasMoved      = mouseDelta.x != 0 || mouseDelta.y != 0;

    f32 canvasX = 0.0f;
    f32 canvasY = 0.0f;

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
                    actions.push_back(std::make_unique<UICloseRequestedAction>());
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

            const auto defineDragNDropButtonFn = [&](const char* label, const char* payload)
            {
                ImGui::Button(label, ImVec2(50, 50));
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
                {
                    ImGui::SetDragDropPayload(payload, payload, sizeof(payload));

                    // Preview tooltip
                    ImGui::Text("Drag and drop me!");

                    ImGui::EndDragDropSource();
                }
            };

            defineDragNDropButtonFn("NAND", "NAND");
            // Break
            defineDragNDropButtonFn("Node", "NODE");
            // Break
            defineDragNDropButtonFn("CLK", "CLK");
            // Break
            ImGui::Separator();

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
                        actions.push_back(std::make_unique<UISimControlAction>(SimControl::Step));
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Run"))
                    {
                        actions.push_back(std::make_unique<UISimControlAction>(SimControl::Run));
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Stop"))
                    {
                        actions.push_back(std::make_unique<UISimControlAction>(SimControl::Stop));
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Zoom +"))
                    {
                        actions.push_back(std::make_unique<UIMouseWheelAction>(1.0f));
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Zoom -"))
                    {
                        actions.push_back(std::make_unique<UIMouseWheelAction>(-1.0f));
                    }
                    ImGui::SameLine();

                    /*
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
                ImGui::Image(m_WindowRenderer->sdlCanvasTexture(), ImVec2(Config::kCanvasWidth, Config::kCanvasHeight));
                canvasX = ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x - ImGui::GetScrollX();
                canvasY = Config::kCanvasHeight + ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y - ImGui::GetScrollY();
                {
                    bool isHoveringCanvas = ImGui::IsItemHovered();
                    if (!m_IsHoveringCanvas && isHoveringCanvas)
                    {
                        m_IsHoveringCanvas = true;
                        actions.push_back(std::make_unique<UICanvasHoveredAction>());
                    }
                    else if (m_IsHoveringCanvas && !isHoveringCanvas)
                    {
                        m_IsHoveringCanvas = false;
                        m_IsMouseDown      = false;
                        actions.push_back(std::make_unique<UICanvasHoveredAction>());
                    }

                    if (ImGui::BeginDragDropTarget())
                    {
                        // This is the only UI -> Canvas interaction!
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(nullptr))
                        {
                            actions.push_back(std::make_unique<UIDragDropAction>(canvasX, canvasY, (const char*)payload->Data));
                        }

                        ImGui::EndDragDropTarget();
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
            ImGui::EndGroup(); // "Middle"
        }

        // Right
        ImGui::SameLine();
        ImGui::BeginChild("Right", ImVec2(0, 0), ImGuiChildFlags_Border);
        {
            // TODO: Draw stats & tables for components, gates, nodes, selection, etc.
            ImGui::EndChild();
        }

        ImGui::End(); // "MainWindow"
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

    // Emit stateful canvas actions
    if (m_IsHoveringCanvas)
    {
        if (validPosition && hasMoved)
        {
            actions.push_back(std::make_unique<UIMouseMovedAction>(canvasX, canvasY, mouseDelta.x, mouseDelta.y));
        }

        const bool isMouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
        if (m_IsMouseDown && !isMouseDown)
        {
            m_IsMouseDown = false;
            actions.push_back(std::make_unique<UIMouseUpAction>());
        }
        else if (!m_IsMouseDown && isMouseDown)
        {
            m_IsMouseDown = true;
            actions.push_back(std::make_unique<UIMouseDownAction>());
        }

        // TODO: Why isn't this working?
        if (io.MouseWheel != 0.0f)
        {
            actions.push_back(std::make_unique<UIMouseWheelAction>(io.MouseWheel));
        }

        // TODO: After interacting with the Canvas this doesn't work reliably
        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            actions.push_back(std::make_unique<UICloseRequestedAction>());
        }

        if (ImGui::IsKeyPressed(ImGuiKey_R))
        {
            actions.push_back(std::make_unique<UIKeypressAction>('R'));
        }

        if (ImGui::IsKeyPressed(ImGuiKey_C))
        {
            actions.push_back(std::make_unique<UIKeypressAction>('C'));
        }
    }

    // TODO: Handle the mouse exiting the canvas during a drag - emitting a mouse-up action

    return actions;
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
