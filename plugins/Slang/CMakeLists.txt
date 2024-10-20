project(slang_fetch)

cmake_policy(SET CMP0135 NEW) # extracted file timestamps

if(WISDOM_USE_SYSTEM_SLANGC)
  set(slangc_EXECUTABLE
      "slangc"
      CACHE INTERNAL "")
elseif(WISDOM_BUILD_SLANGC)
  message("Building slang from source...")

  set(SLANG_ENABLE_SLANGD
      OFF
      CACHE BOOL "")
  set(SLANG_ENABLE_SLANGRT
      OFF
      CACHE BOOL "")
  set(SLANG_ENABLE_SLANG_GLSLANG
      OFF
      CACHE BOOL "")
  set(SLANG_ENABLE_TESTS
      OFF
      CACHE BOOL "")
  set(SLANG_ENABLE_EXAMPLES
      OFF
      CACHE BOOL "")
  set(SLANG_ENABLE_REPLAYER
      OFF
      CACHE BOOL "")
  # set(SLANG_ENABLE_PREBUILT_BINARIES OFF CACHE BOOL)

  FetchContent_Declare(
    slang
    GIT_REPOSITORY https://github.com/shader-slang/slang.git
    GIT_TAG f2b2db57cb7b1e8da8abc74bcf1068b52d756d00
    # URL
    # "https://github.com/shader-slang/slang/archive/f2b2db57cb7b1e8da8abc74bcf1068b52d756d00.tar.gz"
    # URL_HASH
    # SHA256="bca0bf454644be08a8b2372e4f180b864df19b476ba6bf8bca53072a30d11a97"
  )
  FetchContent_MakeAvailable(slang)

  set(slangc_EXECUTABLE
      $<TARGET_FILE:slang_slangc>
      CACHE INTERNAL "")

else()
  # fetch binary directly
  message("Fetching prebuilt slangc...")

  if(WISDOM_WINDOWS)
    set(SLANGC_FILE
        https://github.com/shader-slang/slang/releases/download/v2024.14.2/slang-2024.14.2-windows-x86_64.zip
    )
  else()
    set(SLANGC_FILE
        https://github.com/shader-slang/slang/releases/download/v2024.14.2/slang-2024.14.2-linux-x86_64.tar.gz
    )
  endif()

  set(DOWNLOAD_EXTRACT_TIMESTAMP ON)

  FetchContent_Declare(slangc URL ${SLANGC_FILE})
  FetchContent_GetProperties(slangc)
  if(NOT slangc_POPULATED)
    FetchContent_Populate(slangc)
  endif()

  if(WIN32)
    set(slangc_EXECUTABLE
        ${slangc_SOURCE_DIR}/bin/slangc.exe
        CACHE INTERNAL "")
  else()
    set(slangc_EXECUTABLE
        ${slangc_SOURCE_DIR}/bin/slangc
        CACHE INTERNAL "")
  endif()
endif()
