#include "renderer.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>
#include <SDL_render.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "logic_sim.h"
#include "ui.h"

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

void drawPrimitiveCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius)
{
    std::vector<SDL_Vertex> circleVertices;
    std::vector<int> indices;
    GenerateCircleVerts(circleVertices, indices, 8, centerX, centerY, radius);
    SDL_RenderGeometry(renderer, nullptr, circleVertices.data(), circleVertices.size(), indices.data(), indices.size());
}

void Renderer::drawPrimitiveNAND(float x, float y, float w, float h, bool a, bool b, bool o)
{
    // Legs
    SDL_RenderDrawLine(m_Renderer, x + w * 0.0f, y + h * 0.3f, x + w * 0.2f, y + h * 0.3f);
    SDL_RenderDrawLine(m_Renderer, x + w * 0.0f, y + h * 0.7f, x + w * 0.2f, y + h * 0.7f);
    SDL_RenderDrawLine(m_Renderer, x + w * 0.8f, y + h * 0.5f, x + w * 1.0f, y + h * 0.5f);

    // Body
    SDL_RenderDrawLine(m_Renderer, x + w * 0.2f, y + h * 0.2f, x + w * 0.2f, y + h * 0.8f);
    SDL_RenderDrawLine(m_Renderer, x + w * 0.2f, y + h * 0.2f, x + w * 0.5f, y + h * 0.2f);
    SDL_RenderDrawLine(m_Renderer, x + w * 0.5f, y + h * 0.2f, x + w * 0.7f, y + h * 0.3f);
    SDL_RenderDrawLine(m_Renderer, x + w * 0.7f, y + h * 0.3f, x + w * 0.8f, y + h * 0.5f);
    SDL_RenderDrawLine(m_Renderer, x + w * 0.7f, y + h * 0.7f, x + w * 0.8f, y + h * 0.5f);
    SDL_RenderDrawLine(m_Renderer, x + w * 0.5f, y + h * 0.8f, x + w * 0.7f, y + h * 0.7f);
    SDL_RenderDrawLine(m_Renderer, x + w * 0.2f, y + h * 0.8f, x + w * 0.5f, y + h * 0.8f);

    // Draw circle
    drawPrimitiveCircle(m_Renderer, x + w * 0.8f, y + h * 0.5f, w * 0.05f);
}

void Renderer::draw(LogicSim& logicSim)
{
    // Render to a texture
    SDL_SetRenderTarget(m_Renderer, m_CanvasTexture);
    SDL_RenderClear(m_Renderer); // Clear the texture

    // White
    SDL_SetRenderDrawColor(m_Renderer, 255, 255, 255, 255);

    // TODO: Track canvas x and y, the size of the component, and the zoom level
    // TODO: The UI should own the renderer and the logic sim
    // Draw_Primitive_NAND(canvas_x, canvas_y, sz * zoom, sz * zoom, a, b, o);

    for (auto& gate : logicSim.gates)
    {
        drawPrimitiveNAND(m_OffsetX + gate->x * m_Zoom, m_OffsetY + gate->y * m_Zoom, 100 * m_Zoom, 100 * m_Zoom, false, false, false);
    }

    // Render to the screen
    SDL_SetRenderTarget(m_Renderer, nullptr);

    UI::draw(*this, logicSim, m_CanvasTexture);
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
