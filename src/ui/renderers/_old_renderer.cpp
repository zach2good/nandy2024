#include "renderer.h"

#include <SDL.h>
#include <SDL_render.h>
#include <cassert>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <nlohmann/json.hpp>

#include "applog.h"
#include "logic_sim.h"

#include "ui/canvas_view_model.h"

Renderer::Renderer(std::string const& title, std::size_t width, std::size_t height)
: m_Width(width)
, m_Height(height)
{
    // Setup SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Setup SDL Window
    m_Window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN);
    if (m_Window == nullptr)
    {
        spdlog::error("Could not create window: {}", SDL_GetError());
        std::exit(-1);
    }

    // Setup SDL Renderer
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (m_Renderer == nullptr)
    {
        spdlog::error("Error creating SDL_Renderer!", SDL_GetError());
        std::exit(-1);
    }

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
    ImGui_ImplSDL2_InitForSDLRenderer(m_Window, m_Renderer);
    ImGui_ImplSDLRenderer2_Init(m_Renderer);

    m_CanvasTexture = SDL_CreateTexture(m_Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Config::kCanvasWidth, Config::kCanvasHeight);
}

Renderer::~Renderer()
{
    // Cleanup Dear ImGui
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    // Cleanup SDL
    SDL_DestroyRenderer(m_Renderer);
    SDL_DestroyWindow(m_Window);
    SDL_Quit();
}

void Renderer::handleInput(SDL_Event const& event)
{
    ImGui_ImplSDL2_ProcessEvent(&event);
}

void Renderer::clear()
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    SDL_RenderClear(m_Renderer);
    SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 255);
}

void Renderer::drawPrimitiveNAND(float x, float y, float w, float h, bool a, bool b, bool o)
{
    // Legs
    drawPrimitiveLine(x + w * 0.0f, y + h * 0.3f, x + w * 0.2f, y + h * 0.3f);
    drawPrimitiveLine(x + w * 0.0f, y + h * 0.7f, x + w * 0.2f, y + h * 0.7f);
    drawPrimitiveLine(x + w * 0.8f, y + h * 0.5f, x + w * 1.0f, y + h * 0.5f);

    // Body
    drawPrimitiveLine(x + w * 0.2f, y + h * 0.2f, x + w * 0.2f, y + h * 0.8f);
    drawPrimitiveLine(x + w * 0.2f, y + h * 0.2f, x + w * 0.5f, y + h * 0.2f);
    drawPrimitiveLine(x + w * 0.5f, y + h * 0.2f, x + w * 0.7f, y + h * 0.3f);
    drawPrimitiveLine(x + w * 0.7f, y + h * 0.3f, x + w * 0.8f, y + h * 0.5f);
    drawPrimitiveLine(x + w * 0.7f, y + h * 0.7f, x + w * 0.8f, y + h * 0.5f);
    drawPrimitiveLine(x + w * 0.5f, y + h * 0.8f, x + w * 0.7f, y + h * 0.7f);
    drawPrimitiveLine(x + w * 0.2f, y + h * 0.8f, x + w * 0.5f, y + h * 0.8f);

    // Draw circle
    drawPrimitiveCircle(x + w * 0.8f, y + h * 0.5f, w * 0.05f);
}

void Renderer::handleUIInput()
{
    switch (m_UIState)
    {
        case UIState::None:
        {
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && m_IsCanvasHovered)
            {
                m_UIState = UIState::CanvasPanning;
            }
        }
        break;
        case UIState::CanvasPanning:
        {
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                m_OffsetX += ImGui::GetIO().MouseDelta.x;
                m_OffsetY += ImGui::GetIO().MouseDelta.y;
            }
            else
            {
                if (ImGui::GetIO().MouseDelta.x < 0.01f && ImGui::GetIO().MouseDelta.y < 0.01f)
                {
                    // m_SelectedComponents.clear();
                }
                m_UIState = UIState::None;
            }
        }
    }
}

void Renderer::draw(LogicSim& logicSim)
{
    handleUIInput();
    auto model = CanvasViewModel::create(logicSim);
    drawCanvas(model, logicSim);
    drawUI(logicSim);
    /*
    for (auto& node : logicSim.nodes)
    {
        auto& component = logicSim.components[node.componentId.value];
        if (node.value)
        {
            // Yellow
            SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
        }
        else
        {
            // Grey
            SDL_SetRenderDrawColor(m_Renderer, 128, 128, 128, 255);
        }
        drawPrimitiveRectangle(m_OffsetX + component.x * m_Zoom, m_OffsetY + component.y * m_Zoom, component.w * m_Zoom, component.h * m_Zoom);

        // TODO: This is kind of nasty, do I want an abstraction for "wire"?
        for (auto& otherNodeId : logicSim.drivingNodes[node.id.value])
        {
            auto& otherNode = logicSim.nodes[otherNodeId.value];

            auto  a  = logicSim.components[node.componentId.value];
            auto  b  = logicSim.components[otherNode.componentId.value];
            auto  aX = m_OffsetX + (a.x + 5.0f) * m_Zoom;
            auto  aY = m_OffsetY + (a.y + 5.0f) * m_Zoom;
            auto  bX = m_OffsetX + (b.x + 5.0f) * m_Zoom;
            auto  bY = m_OffsetY + (b.y + 5.0f) * m_Zoom;

            if (node.value)
            {
                // Yellow
                SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
            }
            else
            {
                // Grey
                SDL_SetRenderDrawColor(m_Renderer, 128, 128, 128, 255);
            }

            drawPrimitiveLine(aX, aY, bX, bY);
        }

        if (!m_ComponentUnderMouse &&
            m_CursorX > m_OffsetX + component.x * m_Zoom &&
            m_CursorX < m_OffsetX + component.x * m_Zoom + component.w * m_Zoom &&
            m_CursorY > m_OffsetY + component.y * m_Zoom &&
            m_CursorY < m_OffsetY + component.y * m_Zoom + component.h * m_Zoom)
        {
            m_ComponentUnderMouse = component;
        }
    }
    */

    /*
    for (auto& gate : logicSim.gates)
    {
        auto& component = logicSim.components[gate.componentId.value];

        // White
        SDL_SetRenderDrawColor(m_Renderer, 255, 255, 255, 255);
        drawPrimitiveNAND(
            m_OffsetX + component.x * m_Zoom,
            m_OffsetY + component.y * m_Zoom,
            component.w * m_Zoom,
            component.h * m_Zoom, false, false, false);

        if (!m_ComponentUnderMouse &&
            m_CursorX > m_OffsetX + component.x * m_Zoom &&
            m_CursorX < m_OffsetX + component.x * m_Zoom + component.w * m_Zoom &&
            m_CursorY > m_OffsetY + component.y * m_Zoom &&
            m_CursorY < m_OffsetY + component.y * m_Zoom + component.h * m_Zoom)
        {
            m_ComponentUnderMouse = component;
        }
    }
    */

    /*
    if (!m_SelectedComponents.empty())
    {
        float minX = std::numeric_limits<float>::infinity();
        float minY = std::numeric_limits<float>::infinity();
        float maxX = -std::numeric_limits<float>::infinity();
        float maxY = -std::numeric_limits<float>::infinity();

        for (auto& componentId : m_SelectedComponents)
        {
            auto& component = logicSim.components[componentId.value];
            minX = std::min(minX, component.x);
            minY = std::min(minY, component.y);
            maxX = std::max(maxX, component.x + component.w);
            maxY = std::max(maxY, component.y + component.h);
        }

        // Yellow
        SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
        drawPrimitiveRectangle(m_OffsetX + minX * m_Zoom, m_OffsetY + minY * m_Zoom, (maxX - minX) * m_Zoom, (maxY - minY) * m_Zoom);
    }
    */

    switch (m_UIState)
    {
        case UIState::None:
        {
            /*
            // Highlight components under the mouse by redrawing them
            if (m_ComponentUnderMouse)
            {
                auto component = (*m_ComponentUnderMouse).get();
                if (component.type == NAND) // TODO: Get rid of .type and use std::optional or something with .getGate(component)
                {
                    // Yellow
                    SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
                    drawPrimitiveNAND(m_OffsetX + component.x * m_Zoom, m_OffsetY + component.y * m_Zoom, component.w * m_Zoom, component.h * m_Zoom, false, false, false);
                }
                else if (component.type == NODE)
                {
                    // Yellow
                    SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
                    drawPrimitiveRectangle(m_OffsetX + component.x * m_Zoom, m_OffsetY + component.y * m_Zoom, component.w * m_Zoom, component.h * m_Zoom);
                }
            }

            if (m_ComponentUnderMouse && (*m_ComponentUnderMouse).get().type == NAND && ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                m_UIState = UIState::CanvasMovingComponent;
                m_MouseDragStartX = m_CursorX;
                m_MouseDragStartY = m_CursorY;
                m_ComponentBeingMoved = m_ComponentUnderMouse;
            }
            else if (m_ComponentUnderMouse && (*m_ComponentUnderMouse).get().type == NODE && ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                m_UIState = UIState::CanvasConnectingComponents;
                m_MouseDragStartX = m_CursorX;
                m_MouseDragStartY = m_CursorY;
                m_ComponentConnectionSource = m_ComponentUnderMouse;
            }
            // else if (m_ComponentUnderMouse && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            // {
            //     auto& component = (*m_ComponentUnderMouse).get();
            //     logicSim.removeComponent(component.id);
            //     m_ComponentUnderMouse = std::nullopt;
            // }
            else if (m_ComponentUnderMouse && (*m_ComponentUnderMouse).get().type == NODE && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                auto& component = (*m_ComponentUnderMouse).get();
                component.dirty = true;
                Node& node      = logicSim.getNode(component);
                node.value      = !node.value;
            }
            else if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsKeyDown(ImGuiKey_LeftShift) && m_IsCanvasHovered)
            {
                m_MouseDragStartX = m_CursorX;
                m_MouseDragStartY = m_CursorY;
                m_UIState = UIState::CanvasSelecting;
            }
            else if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && m_IsCanvasHovered)
            {
                m_UIState = UIState::CanvasPanning;
            }

            if (ImGui::IsKeyDown(ImGuiKey_Delete) && !m_SelectedComponents.empty())
            {
                // TODO: This is very not safe
                logicSim.stop();
                for (auto& componentId : m_SelectedComponents)
                {
                    logicSim.removeComponent(componentId);
                }
                m_SelectedComponents.clear();
            }
            */
        }
        break;
        case UIState::CanvasPanning:
        {
            /*
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                m_OffsetX += ImGui::GetIO().MouseDelta.x;
                m_OffsetY += ImGui::GetIO().MouseDelta.y;
            }
            else
            {
                if (ImGui::GetIO().MouseDelta.x < 0.01f && ImGui::GetIO().MouseDelta.y < 0.01f)
                {
                    // m_SelectedComponents.clear();
                }
                m_UIState = UIState::None;
            }
            */
        }
        break;
        case UIState::CanvasMovingComponent:
        {
            /*
            if (m_ComponentBeingMoved && ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                // We know the component is a NAND if we're here
                auto& component = (*m_ComponentBeingMoved).get();
                auto& gate      = logicSim.getGate(component);

                // Redraw the component in yellow before we apply any transformations, otherwise there will be ghosting
                SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
                drawPrimitiveNAND(
                    m_OffsetX + component.x * m_Zoom,
                    m_OffsetY + component.y * m_Zoom,
                    component.w * m_Zoom,
                    component.h * m_Zoom, false, false, false);

                // TODO: Gross, make this a method?
                component.x += ImGui::GetIO().MouseDelta.x / m_Zoom;
                component.y += ImGui::GetIO().MouseDelta.y / m_Zoom;

                auto snapToGrid = [](float n, float grid) { return grid * round(n / grid); };
                component.x = snapToGrid(component.x, 20.0f);
                component.y = snapToGrid(component.y, 20.0f);

                // Find and fixup all connected nodes
                // TODO: This is gross, make helpers
                auto& i0c = logicSim.components[logicSim.getNode(gate.input0Id).componentId.value];
                auto& i1c = logicSim.components[logicSim.getNode(gate.input1Id).componentId.value];
                auto& o0c = logicSim.components[logicSim.getNode(gate.outputId).componentId.value];

                // TODO: Don't hardcode sizes here?
                i0c.x = (component.x + component.w * 0.0f) - 5.0f;
                i0c.y = (component.y + component.h * 0.3f) - 5.0f;
                i1c.x = (component.x + component.w * 0.0f) - 5.0f;
                i1c.y = (component.y + component.h * 0.7f) - 5.0f;
                o0c.x = (component.x + component.w * 1.0f) - 5.0f;
                o0c.y = (component.y + component.h * 0.5f) - 5.0f;
            }
            else
            {
                m_ComponentBeingMoved = std::nullopt;
                m_UIState = UIState::None;
            }
            */
        }
        break;
        case UIState::CanvasConnectingComponents:
        {
            /*
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && m_ComponentConnectionSource)
            {
                // Draw a line from the start of the drag to the current mouse position
                SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
                drawPrimitiveLine(m_MouseDragStartX, m_MouseDragStartY, m_CursorX, m_CursorY);

                auto& source = (*m_ComponentConnectionSource).get();

                // Keep source highlighted in yellow
                SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
                drawPrimitiveRectangle(m_OffsetX + source.x * m_Zoom, m_OffsetY + source.y * m_Zoom, source.w * m_Zoom, source.h * m_Zoom);

                m_ComponentConnectionTarget = std::nullopt;
                if (m_ComponentUnderMouse)
                {
                    if (m_ComponentUnderMouse)
                    {
                        auto& target = (*m_ComponentUnderMouse).get();

                        // Check is valid
                        bool isSameNode = source.id.value == target.id.value;
                        // bool isSiblingNode = source->parent && source->parent == target->parent; // TODO: Reinstate me

                        if (!isSameNode) // && !isSiblingNode)
                        {
                            // Highlight potential target in yellow
                            SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
                            drawPrimitiveRectangle(m_OffsetX + target.x * m_Zoom, m_OffsetY + target.y * m_Zoom, target.w * m_Zoom, target.h * m_Zoom);

                            m_ComponentConnectionTarget = m_ComponentUnderMouse;
                        }
                        // else if (isSiblingNode)
                        // {
                        //     // Draw everything in red to indicate invalid connection
                        //     SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 255);
                        //     drawPrimitiveRectangle(m_OffsetX + source->x * m_Zoom, m_OffsetY + source->y * m_Zoom, source->w * m_Zoom, source->h * m_Zoom);
                        //     drawPrimitiveRectangle(m_OffsetX + target->x * m_Zoom, m_OffsetY + target->y * m_Zoom, target->w * m_Zoom, target->h * m_Zoom);
                        //     drawPrimitiveLine(m_MouseDragStartX, m_MouseDragStartY, m_CursorX, m_CursorY);
                        // }
                    }
                }
            }
            else
            {
                if (m_ComponentConnectionSource && m_ComponentConnectionTarget)
                {
                    auto& source = (*m_ComponentConnectionSource).get();
                    auto& target = (*m_ComponentConnectionTarget).get();
                    if (source.type == NODE && target.type == NODE)
                    {
                        auto& sourceNode = logicSim.getNode(source);
                        auto& targetNode = logicSim.getNode(target);
                        logicSim.connect(sourceNode.id, targetNode.id);
                    }
                }

                // Clear the state
                m_ComponentConnectionSource = std::nullopt;
                m_ComponentConnectionTarget = std::nullopt;
                m_UIState = UIState::None;
            }
            */
        }
        break;
        case UIState::CanvasSelecting:
        {
            /*
            auto doRectanglesIntersect = [](float rect1X, float rect1Y, float rect1W, float rect1H,
                                                      float rect2X, float rect2Y, float rect2W, float rect2H) -> bool
            {
                // Check if one rectangle is to the left of the other
                if (rect1X + rect1W < rect2X || rect2X + rect2W < rect1X)
                {
                    return false;
                }
                if (rect1Y + rect1H < rect2Y || rect2Y + rect2H < rect1Y)
                {
                    return false;
                }
                return true;
            };

            // TODO: Make it select components when their bounding box is intersected by the selection rectangle
            // Not just contained within it

            // x, y, w, h of the selection rectangle, in screen space
            float canvasX = m_MouseDragStartX;
            float canvasY = m_MouseDragStartY;
            float canvasW = m_CursorX - m_MouseDragStartX;
            float canvasH = m_CursorY - m_MouseDragStartY;

            // Account for the selection being dragged in the opposite direction
            if (canvasW < 0)
            {
                canvasX += canvasW;
                canvasW = -canvasW;
            }

            if (canvasH < 0)
            {
                canvasY += canvasH;
                canvasH = -canvasH;
            }

            std::unordered_set<Component*> selectedComponents;
            for (auto& node : logicSim.nodes)
            {
               // Get component bounds out of node and check, adjusting for zoom
                auto& component = logicSim.components[node.componentId.value];

                // Convert to screen space
                float componentScreenX = m_OffsetX + component.x * m_Zoom;
                float componentScreenY = m_OffsetY + component.y * m_Zoom;
                float componentScreenW = component.w * m_Zoom;
                float componentScreenH = component.h * m_Zoom;

                if (doRectanglesIntersect(
                    canvasX, canvasY, canvasW, canvasH,
                    componentScreenX, componentScreenY, componentScreenW, componentScreenH))
                {
                    selectedComponents.insert(&component);
                }
            }
            for (auto& gate : logicSim.gates)
            {
                // Get component bounds out of gate and check, adjusting for zoom
                auto& component = logicSim.components[gate.componentId.value];

                // Convert to screen space
                float componentScreenX = m_OffsetX + component.x * m_Zoom;
                float componentScreenY = m_OffsetY + component.y * m_Zoom;
                float componentScreenW = component.w * m_Zoom;
                float componentScreenH = component.h * m_Zoom;

                if (doRectanglesIntersect(canvasX, canvasY, canvasW, canvasH,
                    componentScreenX, componentScreenY, componentScreenW, componentScreenH))
                {
                    selectedComponents.insert(&component);

                    selectedComponents.insert(&component);
                    auto& i0 = logicSim.components[logicSim.getNode(gate.input0Id).componentId.value];
                    auto& i1 = logicSim.components[logicSim.getNode(gate.input1Id).componentId.value];
                    auto& o0 = logicSim.components[logicSim.getNode(gate.outputId).componentId.value];
                    selectedComponents.insert(&i0);
                    selectedComponents.insert(&i1);
                    selectedComponents.insert(&o0);
                }
            }

            float minX = std::numeric_limits<float>::infinity();
            float minY = std::numeric_limits<float>::infinity();
            float maxX = -std::numeric_limits<float>::infinity();
            float maxY = -std::numeric_limits<float>::infinity();

            for (auto& component : selectedComponents)
            {
                minX = std::min(minX, component->x);
                minY = std::min(minY, component->y);
                maxX = std::max(maxX, component->x + component->w);
                maxY = std::max(maxY, component->y + component->h);
            }

            SDL_SetRenderDrawColor(m_Renderer, 64, 64, 64, 255);
            drawPrimitiveRectangle(m_OffsetX + minX * m_Zoom, m_OffsetY + minY * m_Zoom, (maxX - minX) * m_Zoom, (maxY - minY) * m_Zoom);

            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                // Draw a rectangle from the start of the drag to the current mouse position
                SDL_SetRenderDrawColor(m_Renderer, 255, 255, 255, 255);
                drawPrimitiveRectangle(m_MouseDragStartX, m_MouseDragStartY, m_CursorX - m_MouseDragStartX, m_CursorY - m_MouseDragStartY);
            }
            else
            {
                // TODO: Output selected components to the console as JSON
                spdlog::info("Selected {} components", selectedComponents.size());
                for (auto component : selectedComponents)
                {
                    spdlog::info("{}", component->id.value);
                    m_SelectedComponents.push_back(component->id);
                }

                m_UIState = UIState::None;
            }
            */
        }
        break;
        case UIState::UIDragging:
        {
            // TODO:
            // Do nothing
        }
    }
}
