#pragma once

// Win32 specific content
#define VK_USE_PLATFORM_WIN32_KHR
#include <accctrl.h>
#include <aclapi.h>
#include <sddl.h>

// Std
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <array>
#include <set>
#include <map>
#include <optional>

// Vulkan
#include <vulkan/vulkan.hpp>