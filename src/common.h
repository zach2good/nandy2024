#pragma once

// Forward declarations
class LogicSim;
class Renderer;
class UI;

// SDL Forward declarations
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Surface;
struct SDL_Texture;
typedef union SDL_Event SDL_Event;

// Stdlib includes
#include <atomic>
#include <cstdint>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <thread>
#include <vector>

// Internal includes
#include "config.h"
#include "utils.h"

// External includes
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

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
