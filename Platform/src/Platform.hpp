#pragma once

namespace CppUtil
{
struct Platform
{
#if defined(_WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS 1
  static constexpr const char * PlatformName  = "Windows";
  static constexpr const char   PathSeparator = '\\';
  static constexpr const char * LineEnding    = "\r\n";
#elif defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_OS_MAC
#define PLATFORM_MACOS 1
#define PLATFORM_UNIX_LIKE 1
  static constexpr const char * PlatformName  = "macOS";
  static constexpr const char   PathSeparator = '/';
  static constexpr const char * LineEnding    = "\n";
#endif
#elif defined(__linux__)
#define PLATFORM_LINUX 1
#define PLATFORM_UNIX_LIKE 1
  static constexpr const char * PlatformName  = "Linux";
  static constexpr const char   PathSeparator = '/';
  static constexpr const char * LineEnding    = "\n";
#elif defined(__unix__)
#define PLATFORM_UNIX 1
#define PLATFORM_UNIX_LIKE 1
  static constexpr const char * PlatformName  = "Unix";
  static constexpr const char   PathSeparator = '/';
  static constexpr const char * LineEnding    = "\n";
#else
#define PLATFORM_UNKNOWN 1
  static constexpr const char * PlatformName  = "Unknown";
  static constexpr const char   PathSeparator = '/';
  static constexpr const char * LineEnding    = "\n";
#endif
};
} // namespace CppUtil