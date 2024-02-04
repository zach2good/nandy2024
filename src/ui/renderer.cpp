#include "renderer.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>
#include <SDL_render.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "applog.h"
#include "logic_sim.h"

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
    ImGuiIO& io = ImGui::GetIO(); (void)io;
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

// https://stackoverflow.com/questions/71398406/generate-the-vertices-of-a-circle-in-sdl2-c
void GenerateCircleVerts(std::vector<SDL_Vertex> &outCircleVertices, std::vector<int> &outIndices, std::size_t vertexCount, int centerX, int centerY, int radius)
{
    // Size our vector so it can hold all the requested vertices plus our center one
    outCircleVertices.resize(vertexCount + 1);

    // Calculate the angle we'll need to rotate by for each iteration (* (PI / 180) to convert it into radians)
    double segRotationAngle = (360.0 / vertexCount) * (3.14159265 / 180);

    // We need an initial vertex in the center as a point for all of the triangles we'll generate
    outCircleVertices[0].position.x = centerX;
    outCircleVertices[0].position.y = centerY;

    // Set the colour of the center point
    outCircleVertices[0].color.r = 255;
    outCircleVertices[0].color.g = 255;
    outCircleVertices[0].color.b = 255;
    outCircleVertices[0].color.a = 255;

    // Set the starting point for the initial generated vertex. We'll be rotating this point around the origin in the loop
    double startX = 0.0 - radius;
    double startY = 0.0;

    for (int i = 1; i < vertexCount + 1; i++)
    {
        // Calculate the angle to rotate the starting point around the origin
        double finalSegRotationAngle = (i * segRotationAngle);

        // Rotate the start point around the origin (0, 0) by the finalSegRotationAngle (see https://en.wikipedia.org/wiki/Rotation_(mathematics) section on two dimensional rotation)
        outCircleVertices[i].position.x = cos(finalSegRotationAngle) * startX - sin(finalSegRotationAngle) * startY;
        outCircleVertices[i].position.y = cos(finalSegRotationAngle) * startY + sin(finalSegRotationAngle) * startX;

        // Set the point relative to our defined center (in this case the center of the screen)
        outCircleVertices[i].position.x += centerX;
        outCircleVertices[i].position.y += centerY;

        // Set the colour for the vertex
        outCircleVertices[i].color.r = 255;
        outCircleVertices[i].color.g = 255;
        outCircleVertices[i].color.b = 255;
        outCircleVertices[i].color.a = 255;

        // Add centre point index
        outIndices.push_back(0);

        // Add generated point index
        outIndices.push_back(i);

        // Add next point index (with logic to wrap around when we reach the start)
        int index = (i + 1) % vertexCount;
        if (index == 0)
        {
            index = vertexCount;
        }
        outIndices.push_back(index);
    }
}

void Renderer::drawPrimitiveLine(float x1, float y1, float x2, float y2)
{
    SDL_RenderDrawLine(m_Renderer, x1, y1, x2, y2);
}

void Renderer::drawPrimitiveRectangle(float x, float y, float w, float h)
{
    drawPrimitiveLine(x, y, x + w, y);
    drawPrimitiveLine(x, y, x, y + h);
    drawPrimitiveLine(x + w, y, x + w, y + h);
    drawPrimitiveLine(x, y + h, x + w, y + h);
}

void Renderer::drawPrimitiveCircle(int centerX, int centerY, int radius)
{
    std::vector<SDL_Vertex> circleVertices;
    std::vector<int> indices;
    GenerateCircleVerts(circleVertices, indices, 8, centerX, centerY, radius);
    SDL_RenderGeometry(m_Renderer, nullptr, circleVertices.data(), circleVertices.size(), indices.data(), indices.size());
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

void Renderer::draw(LogicSim& logicSim)
{
    // Render to a texture
    SDL_SetRenderTarget(m_Renderer, m_CanvasTexture);
    SDL_RenderClear(m_Renderer); // Clear the texture

    // TODO: The UI should own the renderer and the logic sim
    // Draw_Primitive_NAND(canvas_x, canvas_y, sz * zoom, sz * zoom, a, b, o);

    // TODO: Loop through the gates only once and build a viewmodel that can be drawn efficiently

    // Whatever state we're in, draw all gates and nodes (TODO: subject to view culling)
    // While we're doing this, we'll extract the id of the component under the cursor
    m_ComponentUnderMouse = std::nullopt;
    for (auto& node : logicSim.nodes)
    {
        if (node->getValue())
        {
            // Yellow
            SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
        }
        else
        {
            // Grey
            SDL_SetRenderDrawColor(m_Renderer, 128, 128, 128, 255);
        }
        drawPrimitiveRectangle(m_OffsetX + node->x * m_Zoom, m_OffsetY + node->y * m_Zoom, node->w * m_Zoom, node->h * m_Zoom);

        // TODO: This is kind of nasty, do I want an abstraction for "wire"?
        for (auto& other : node->driving)
        {
            if (other->type != NODE)
            {
                continue;
            }

            auto a  = node;
            auto b  = other;
            auto aX = m_OffsetX + (a->x + 5.0f) * m_Zoom;
            auto aY = m_OffsetY + (a->y + 5.0f) * m_Zoom;
            auto bX = m_OffsetX + (b->x + 5.0f) * m_Zoom;
            auto bY = m_OffsetY + (b->y + 5.0f) * m_Zoom;

            if (a->getValue())
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

        if (!m_ComponentUnderMouse && m_CursorX > m_OffsetX + node->x * m_Zoom && m_CursorX < m_OffsetX + node->x * m_Zoom + node->w * m_Zoom &&
            m_CursorY > m_OffsetY + node->y * m_Zoom && m_CursorY < m_OffsetY + node->y * m_Zoom + node->h * m_Zoom)
        {
            m_ComponentUnderMouse = node;
        }
    }
    for (auto& gate : logicSim.gates)
    {
        // White
        SDL_SetRenderDrawColor(m_Renderer, 255, 255, 255, 255);
        drawPrimitiveNAND(m_OffsetX + gate->x * m_Zoom, m_OffsetY + gate->y * m_Zoom, gate->w * m_Zoom, gate->h * m_Zoom, false, false, false);

        if (!m_ComponentUnderMouse && m_CursorX > m_OffsetX + gate->x * m_Zoom && m_CursorX < m_OffsetX + gate->x * m_Zoom + gate->w * m_Zoom &&
            m_CursorY > m_OffsetY + gate->y * m_Zoom && m_CursorY < m_OffsetY + gate->y * m_Zoom + gate->h * m_Zoom)
        {
            m_ComponentUnderMouse = gate;
        }
    }

    switch (m_UIState)
    {
        case UIState::None:
        {
            // Highlight components under the mouse by redrawing them
            if (m_ComponentUnderMouse)
            {
                if (auto gate = std::dynamic_pointer_cast<NandGate>(*m_ComponentUnderMouse); gate != nullptr)
                {
                    // Yellow
                    SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
                    drawPrimitiveRectangle(m_OffsetX + gate->x * m_Zoom, m_OffsetY + gate->y * m_Zoom, gate->w * m_Zoom, gate->h * m_Zoom);
                }
                else if (auto node = std::dynamic_pointer_cast<Node>(*m_ComponentUnderMouse); node != nullptr)
                {
                    // Yellow
                    SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
                    drawPrimitiveRectangle(m_OffsetX + node->x * m_Zoom, m_OffsetY + node->y * m_Zoom, node->w * m_Zoom, node->h * m_Zoom);
                }
            }

            if (m_ComponentUnderMouse && (*m_ComponentUnderMouse)->type == NAND && ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                m_UIState = UIState::CanvasMovingComponent;
                m_MouseDragStartX = m_CursorX;
                m_MouseDragStartY = m_CursorY;
                m_ComponentBeingMoved = m_ComponentUnderMouse;
            }
            else if (m_ComponentUnderMouse && (*m_ComponentUnderMouse)->type == NODE && ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                m_UIState = UIState::CanvasConnectingComponents;
                m_MouseDragStartX = m_CursorX;
                m_MouseDragStartY = m_CursorY;
                m_ComponentConnectionSource = m_ComponentUnderMouse;
            }
            else if (m_ComponentUnderMouse && (*m_ComponentUnderMouse)->type == NODE && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                auto node = std::dynamic_pointer_cast<Node>(*m_ComponentUnderMouse);
                node->setValue(!node->getValue());
            }
            else if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && m_IsCanvasHovered)
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
                m_UIState = UIState::None;
            }
        }
        break;
        case UIState::CanvasMovingComponent:
        {
            if (m_ComponentBeingMoved && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
            {
                // We know the component is a NAND if we're here
                auto gate = std::dynamic_pointer_cast<NandGate>(*m_ComponentBeingMoved);

                // Redraw the component in yellow before we apply any transformations, otherwise there will be ghosting
                SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
                drawPrimitiveNAND(m_OffsetX + gate->x * m_Zoom, m_OffsetY + gate->y * m_Zoom, gate->w * m_Zoom, gate->h * m_Zoom, false, false, false);

                // TODO: Gross, make this a method?
                gate->x += ImGui::GetIO().MouseDelta.x / m_Zoom;
                gate->y += ImGui::GetIO().MouseDelta.y / m_Zoom;

                // TODO: Don't hardcode sizes here?
                gate->getInputNode1()->x = (gate->x + gate->w * 0.0f) - 5.0f;
                gate->getInputNode1()->y = (gate->y + gate->h * 0.3f) - 5.0f;
                gate->getInputNode2()->x = (gate->x + gate->w * 0.0f) - 5.0f;
                gate->getInputNode2()->y = (gate->y + gate->h * 0.7f) - 5.0f;
                gate->getOutputNode()->x = (gate->x + gate->w * 1.0f) - 5.0f;
                gate->getOutputNode()->y = (gate->y + gate->h * 0.5f) - 5.0f;
            }
            else
            {
                m_ComponentBeingMoved = std::nullopt;
                m_UIState = UIState::None;
            }
        }
        break;
        case UIState::CanvasConnectingComponents:
        {
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && m_ComponentConnectionSource)
            {
                // Draw a line from the start of the drag to the current mouse position
                SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
                drawPrimitiveLine(m_MouseDragStartX, m_MouseDragStartY, m_CursorX, m_CursorY);

                auto source = std::dynamic_pointer_cast<Node>(*m_ComponentConnectionSource);

                // Keep source highlighted in yellow
                SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
                drawPrimitiveRectangle(m_OffsetX + source->x * m_Zoom, m_OffsetY + source->y * m_Zoom, source->w * m_Zoom, source->h * m_Zoom);

                m_ComponentConnectionTarget = std::nullopt;
                if (m_ComponentUnderMouse)
                {
                    if (auto target = std::dynamic_pointer_cast<Node>(*m_ComponentUnderMouse); target != nullptr)
                    {
                        // Check is valid
                        bool isSameNode    = source->id == target->id;
                        bool isSiblingNode = source->parent && source->parent == target->parent;

                        if (!isSameNode && !isSiblingNode)
                        {
                            // Highlight potential target in yellow
                            SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
                            drawPrimitiveRectangle(m_OffsetX + target->x * m_Zoom, m_OffsetY + target->y * m_Zoom, target->w * m_Zoom, target->h * m_Zoom);

                            m_ComponentConnectionTarget = m_ComponentUnderMouse;
                        }
                        else if (isSiblingNode)
                        {
                            // Draw everything in red to indicate invalid connection
                            SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 255);
                            drawPrimitiveRectangle(m_OffsetX + source->x * m_Zoom, m_OffsetY + source->y * m_Zoom, source->w * m_Zoom, source->h * m_Zoom);
                            drawPrimitiveRectangle(m_OffsetX + target->x * m_Zoom, m_OffsetY + target->y * m_Zoom, target->w * m_Zoom, target->h * m_Zoom);
                            drawPrimitiveLine(m_MouseDragStartX, m_MouseDragStartY, m_CursorX, m_CursorY);
                        }
                    }
                }
            }
            else
            {
                if (m_ComponentConnectionSource && m_ComponentConnectionTarget)
                {
                    auto source = std::dynamic_pointer_cast<Node>(*m_ComponentConnectionSource);
                    auto target = std::dynamic_pointer_cast<Node>(*m_ComponentConnectionTarget);
                    logicSim.connect(source, target);
                }

                // Clear the state
                m_ComponentConnectionSource = std::nullopt;
                m_ComponentConnectionTarget = std::nullopt;
                m_UIState = UIState::None;
            }
        }
        break;
    }

    // Render to the screen
    SDL_SetRenderTarget(m_Renderer, nullptr);
    drawUI(logicSim);
}

void Renderer::present()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Render();
    SDL_RenderSetScale(m_Renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    SDL_SetRenderDrawColor(m_Renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

    SDL_RenderPresent(m_Renderer);
}

void Renderer::drawUI(LogicSim& logicSim)
{
    // TODO: Instead of getting fresh information from logicSim every time there is a UI update, we should only do it 2-4 times per second.
    //       This includes generating the texture, and getting the stats.

    // TODO: Break out each UI section into its own function and make resizing reactive instead of hard-coding sizes.

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
                m_UIState = UIState::UIDragging;
                ImGui::SetDragDropPayload("NAND", "NAND", sizeof("NAND"));

                // Preview tooltip
                ImGui::Text("Drag and drop me!");

                ImGui::EndDragDropSource();
            }

            ImGui::SameLine();

            ImGui::Button("Node", ImVec2(50, 50));
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
            {
                m_UIState = UIState::UIDragging;
                ImGui::SetDragDropPayload("NODE", "NODE", sizeof("NODE"));

                // Preview tooltip
                ImGui::Text("Drag and drop me!");

                ImGui::EndDragDropSource();
            }

            // Break

            ImGui::Button("Output", ImVec2(50, 50));
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
            {
                m_UIState = UIState::UIDragging;
                ImGui::SetDragDropPayload("OUTPUT", "OUTPUT", sizeof("OUTPUT"));

                // Preview tooltip
                ImGui::Text("Drag and drop me!");

                ImGui::EndDragDropSource();
            }

            // Break

            ImGui::Button("CLK", ImVec2(50, 50));
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
            {
                m_UIState = UIState::UIDragging;
                ImGui::SetDragDropPayload("CLK", "CLK", sizeof("CLK"));

                // Preview tooltip
                ImGui::Text("Drag and drop me!");

                ImGui::EndDragDropSource();
            }

            // Break

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

                    ImGui::EndChild(); // "ButtonsPane"
                }

                // Canvas Image w/ drag and drop
                // 0,0 is top left corner of the window
                m_CursorX = 0.0f;
                m_CursorY = 0.0f;
                {
                    ImGui::Image(m_CanvasTexture, ImVec2(Config::kCanvasWidth, Config::kCanvasHeight));
                    auto rect = ImGui::GetItemRectMin();
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
                        else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("OUTPUT"))
                        {
                            logicSim.addOutputNode(x + 50.0f, y + 50.0f); // TODO
                            m_UIState = UIState::None;
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
            if (m_CursorX == 0.0f && m_CursorY == 0.0f)
            {
                ImGui::Text("Canvas pos: None");
            }
            else
            {
                ImGui::Text(fmt::format("Canvas pos: {}, {}", m_CursorX, m_CursorY).c_str());
            }

            ImGui::Separator();

            ImGui::Text(fmt::format("UI State: {}", UIStateToString(m_UIState)).c_str());

            ImGui::Separator();

            ImGui::Text("0 is the placeholder id for nullopt.");

            ImGui::Separator();

            ImGui::Text(fmt::format("m_ComponentUnderMouse: {}", m_ComponentUnderMouse ? m_ComponentUnderMouse.value()->id : 0).c_str());
            ImGui::Text(fmt::format("m_ComponentUnderMouse type: {}", m_ComponentUnderMouse ? componentTypeToString(m_ComponentUnderMouse.value()->type) : "").c_str());
            if (m_ComponentUnderMouse)
            {
                if (auto gate = std::dynamic_pointer_cast<NandGate>(*m_ComponentUnderMouse); gate != nullptr)
                {
                    ImGui::Text(fmt::format(" - a: {}", gate->getInputNode1()->getValue()).c_str());
                    ImGui::Text(fmt::format(" - b: {}", gate->getInputNode2()->getValue()).c_str());
                    ImGui::Text(fmt::format(" - o: {}", gate->getOutputNode()->getValue()).c_str());
                }
                else if (auto node = std::dynamic_pointer_cast<Node>(*m_ComponentUnderMouse); node != nullptr)
                {
                    ImGui::Text(fmt::format(" - value: {}", node->getValue()).c_str());
                }
            }

            ImGui::Separator();

            ImGui::Text(fmt::format("m_ComponentBeingMoved: {}", m_ComponentBeingMoved ? m_ComponentBeingMoved.value()->id : 0).c_str());
            ImGui::Text(fmt::format("m_ComponentBeingMoved type: {}", m_ComponentBeingMoved ? componentTypeToString(m_ComponentBeingMoved.value()->type) : "").c_str());

            ImGui::Separator();

            ImGui::Text(fmt::format("m_ComponentConnectionSource: {}", m_ComponentConnectionSource ? m_ComponentConnectionSource.value()->id : 0).c_str());
            ImGui::Text(fmt::format("m_ComponentConnectionSource type: {}", m_ComponentConnectionSource ? componentTypeToString(m_ComponentConnectionSource.value()->type) : "").c_str());
            ImGui::Text(fmt::format("m_ComponentConnectionTarget: {}", m_ComponentConnectionTarget ? m_ComponentConnectionTarget.value()->id : 0).c_str());
            ImGui::Text(fmt::format("m_ComponentConnectionTarget type: {}", m_ComponentConnectionTarget ? componentTypeToString(m_ComponentConnectionTarget.value()->type) : "").c_str());

            ImGui::EndChild();
        }

        ImGui::End(); // "MainWindow"
    }
}
