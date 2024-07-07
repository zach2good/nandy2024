#pragma once

#include <cstddef>
#include <cstdint>

// SDL Forward declarations
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Surface;
struct SDL_Texture;
typedef union SDL_Event SDL_Event;

// Typedefs
typedef std::uint8_t   u8;
typedef std::uint16_t  u16;
typedef std::uint32_t  u32;
typedef std::uint64_t  u64;
typedef std::int8_t    s8;
typedef std::int16_t   s16;
typedef std::int32_t   s32;
typedef std::int64_t   s64;
typedef float          f32;
typedef double         f64;
typedef std::size_t    usize;
typedef std::ptrdiff_t isize;

struct Position final
{
    f64 x;
    f64 y;
};

inline auto toString(const Position& position) -> std::string
{
    return fmt::format("Position: x={}, y={}", position.x, position.y);
}

struct Delta final
{
    f64 dx;
    f64 dy;
};

inline auto toString(const Delta& delta) -> std::string
{
    return fmt::format("Delta: dx={}, dy={}", delta.dx, delta.dy);
}

struct Size final
{
    f64 width;
    f64 height;
};

inline auto toString(const Size& size) -> std::string
{
    return fmt::format("Size: width={}, height={}", size.width, size.height);
}

enum class Colour
{
    Red,
    Green,
    Blue,
    Yellow,
    Cyan,
    Magenta,
    White,
    Black,
    Grey,
    LightGrey,
    DarkGrey,
};

inline auto toString(Colour colour) -> std::string
{
    switch (colour)
    {
        case Colour::Red:
            return "Red";
        case Colour::Green:
            return "Green";
        case Colour::Blue:
            return "Blue";
        case Colour::Yellow:
            return "Yellow";
        case Colour::Cyan:
            return "Cyan";
        case Colour::Magenta:
            return "Magenta";
        case Colour::White:
            return "White";
        case Colour::Black:
            return "Black";
        case Colour::Grey:
            return "Grey";
        case Colour::LightGrey:
            return "LightGrey";
        case Colour::DarkGrey:
            return "DarkGrey";
    }

    return "Unknown";
}

enum class Facing
{
    Right,
    Down,
    Left,
    Up,
};

inline auto toString(Facing facing) -> std::string
{
    switch (facing)
    {
        case Facing::Right:
            return "Right";
        case Facing::Down:
            return "Down";
        case Facing::Left:
            return "Left";
        case Facing::Up:
            return "Up";
    }

    return "Unknown";
}

enum class SimControl
{
    Step,
    Run,
    Stop,
    Reset,
};

inline std::string toString(SimControl control)
{
    switch (control)
    {
        case SimControl::Step:
            return "Step";
        case SimControl::Run:
            return "Run";
        case SimControl::Stop:
            return "Stop";
        case SimControl::Reset:
            return "Reset";
        default:
            return "Unknown";
    }
}

#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace nlohmann::literals;
