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

struct Position
{
    f64 x;
    f64 y;
};

struct Size
{
    f64 width;
    f64 height;
};

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

enum class Facing
{
    Right,
    Down,
    Left,
    Up,
};

#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace nlohmann::literals;
