project(slang_fetch)

if(WISDOM_USE_SYSTEM_SLANGC)
  set(slangc_EXECUTABLE
      "slangc"
      CACHE INTERNAL "")
else()
  message("Building slang from source...")

  set(SLANG_ENABLE_SLANGD OFF CACHE BOOL "")
  set(SLANG_ENABLE_SLANGRT OFF CACHE BOOL "")
  set(SLANG_ENABLE_SLANG_GLSLANG OFF CACHE BOOL "")
  set(SLANG_ENABLE_TESTS OFF CACHE BOOL "")
  set(SLANG_ENABLE_EXAMPLES OFF CACHE BOOL "")
  set(SLANG_ENABLE_REPLAYER OFF CACHE BOOL "")
  # set(SLANG_ENABLE_PREBUILT_BINARIES OFF CACHE BOOL)

  FetchContent_Declare(slang
      GIT_REPOSITORY https://github.com/shader-slang/slang.git
      GIT_TAG        f2b2db57cb7b1e8da8abc74bcf1068b52d756d00
  #     URL "https://github.com/shader-slang/slang/archive/f2b2db57cb7b1e8da8abc74bcf1068b52d756d00.tar.gz"
  #     URL_HASH SHA256="bca0bf454644be08a8b2372e4f180b864df19b476ba6bf8bca53072a30d11a97"
  )
  FetchContent_MakeAvailable(slang)

  set(slangc_EXECUTABLE
      $<TARGET_FILE:slang_slangc>
      CACHE INTERNAL "")

endif()
