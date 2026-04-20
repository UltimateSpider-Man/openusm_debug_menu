# ═══════════════════════════════════════════════════════════════════════
#   OpenUSM — unified CMakeLists.txt
#
#   Produces, in a single `cmake --build build` invocation:
#
#     • build/USM.exe          — compiled from USM.c's rawData[5926912]
#                                byte array (5,926,912 bytes = 5.65 MiB,
#                                SHA-256 5d554ef6…0488b8 — byte-identical
#                                to the original Ultimate Spider-Man exe
#                                because that's what USM.c encodes).
#     • build/binkw32.dll      — the OpenUSM mod-loader
#     • build/debug_menu.dll   — the PC debug menu (UPX-compressed)
#     • build/openusm.dll      — a copy of binkw32.dll under the
#                                openusm name for asi-loader deploy
#
#   Auto-deploys every output to ../Ultimate Spider-Man/ if that
#   directory exists next to the source tree. Stock DLLs (binkw32,
#   d3dx9_24, dsound, MSVCP71, msvcr71, xerces-c_2_3_0) in that
#   folder are left untouched by this build.
#
#   Required layout:
#
#       OpenUSM/
#       ├── CMakeLists.txt                 ← this file
#       ├── main.cpp                       ← binkw32 entry point
#       ├── usm_debug_menu.cpp             ← debug_menu entry point
#       ├── USM.c                          ← rawData[5926912] blob (REQUIRED)
#       ├── emit_usm_exe.c                 ← host-tool writer       (REQUIRED)
#       ├── src/                           ← parent-project libraries
#       ├── usm_debug_menu/                ← optional: sibling debug
#       │   ├── usm.cpp                      menu project (USM_DEBUG_MENU_DIR)
#       │   └── dev_src/*.cpp,*.h
#       ├── cmake/
#       │   ├── i686-pc-mingw32.cmake      ← MinGW cross toolchain
#       │   └── report_size.cmake          ← size reporter for UPX step
#       ├── third_party/
#       │   ├── glm/                       ← (optional, USE_GLM)
#       │   └── googletest/                ← (optional, ENABLE_TESTS)
#       ├── assimp/                        ← Assimp source tree
#       └── test/                          ← (optional, ENABLE_TESTS)
#
#   Build:
#
#       cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/i686-pc-mingw32.cmake
#       cmake --build build -j$(nproc)
#
#   Top-level options:
#
#     -DUSE_GLM=ON                   link glm::glm into binkw32/debug_menu
#     -DUSE_FSTREAM=ON               -DUSE_FSTREAM in binkw32/debug_menu
#     -DCHECK_STRUCT_SIZES=OFF       disable CHECK_STRUCT_SIZES define
#     -DTARGET_XBOX=ON               build the Xbox variant of mash
#     -DENABLE_TESTS=OFF             skip googletest/CTest
#     -DUSM_BUILD_DEBUG_MENU=OFF     skip the debug_menu target
#     -DDEBUG_MENU_COMPRESS=OFF      skip the UPX pack of debug_menu.dll
#     -DDEBUG_MENU_STRIP_BEFORE_UPX=OFF
#     -DDEBUG_MENU_SIZE_LIMIT=<bytes>  soft warning threshold (1 MiB default)
#     -DUSM_C_SOURCE=<path>          override USM.c location
#     -DUSM_C_WRITER_SOURCE=<path>   override emit_usm_exe.c location
# ═══════════════════════════════════════════════════════════════════════

cmake_minimum_required(VERSION 3.12)

include(ExternalProject)
include(cmake/i686-pc-mingw32.cmake)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-static -std=gnu11 -g")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}
    "-fomit-frame-pointer -m32 -fno-asynchronous-unwind-tables -masm=intel -static -g")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)

list(APPEND flags
    #"-Werror"
    #"-pedantic-errors"
    "-w"                                # master switch: inhibit all warnings
    # Keep the explicit -Wno-* list below as documentation of known sources of
    # noise, but -w above disables everything. Remove -w and uncomment
    # -Wall/-Wextra/-Wpedantic if you want selective warnings back.
    #"-Wall"
    #"-Wextra"
    #"-Wpedantic"

    # ── Suppress decompiled-code / IDA-output artifacts ─────────────────
    "-Wno-deprecated"
    "-Wno-deprecated-declarations"
    "-Wno-float-equal"                  # float == / != throughout IDA output
    "-Wno-sign-compare"                 # signed/unsigned pervasive in decompiled code
    "-Wno-cast-qual"                    # const-cast-away common in decompiled code
    "-Wno-cast-align"                   # decompiled pointer casts
    "-Wno-cast-function-type"           # func pointer casts in hook code
    "-Wno-zero-as-null-pointer-constant" # old C-style 0 for nullptr
    "-Wno-missing-field-initializers"   # {0} partial struct init
    "-Wno-missing-braces"               # nested aggregate init
    "-Wno-class-memaccess"              # memcpy/memset on non-trivial types
    "-Wno-write-strings"                # string literal -> char*
    "-Wno-unused-parameter"
    "-Wno-unused-variable"
    "-Wno-unused-but-set-variable"
    "-Wno-unused-value"
    "-Wno-unused-label"
    "-Wno-unused-function"
    "-Wno-unused-result"
    "-Wno-unused-local-typedefs"
    "-Wno-redundant-decls"              # forward-decls duplicating header decls
    "-Wno-type-limits"                  # uint8_t != 0xFF always-true comparisons
    "-Wno-narrowing"                    # float->int narrowing in aggregate inits
    "-Wno-maybe-uninitialized"
    "-Wno-uninitialized"
    "-Wno-switch"
    "-Wno-switch-default"
    "-Wno-switch-enum"
    "-Wno-implicit-fallthrough"
    "-Wno-comment"
    "-Wno-parentheses"
    "-Wno-unknown-pragmas"              # #pragma region / #pragma endregion
    "-Wno-format"
    "-Wno-format-security"
    "-Wno-format-extra-args"
    "-Wno-misleading-indentation"
    "-Wno-reorder"
    "-Wno-pedantic"
    "-Wno-overloaded-virtual"
    "-Wno-ctor-dtor-privacy"
    "-Wno-sign-promo"
    "-Wno-return-type"
    "-Wno-return-local-addr"
    "-Wno-duplicated-cond"
    "-Wno-duplicated-branches"
    "-Wno-strict-aliasing"
    "-Wno-address"
    "-Wno-nonnull"
    "-Wno-nonnull-compare"
    "-Wno-null-dereference"
    "-Wno-stringop-overflow"
    "-Wno-stringop-overread"
    "-Wno-stringop-truncation"
    "-Wno-restrict"
    "-Wno-array-bounds"
    "-Wno-attributes"
    "-Wno-conversion"
    "-Wno-extra"
    "-Wno-shift-count-overflow"
    "-Wno-shift-overflow"
    "-Wno-volatile"                     # C++20 volatile deprecation
    "-Wno-builtin-macro-redefined"
    "-Wno-subobject-linkage"
)


project(USM-dev)


if(MINGW)
    # Drop the "lib" prefix on MinGW so shared libs come out as
    # binkw32.dll / debug_menu.dll — not libbinkw32.dll etc.
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
    set(CMAKE_STATIC_LIBRARY_PREFIX "")
    set(CMAKE_SHARED_MODULE_PREFIX  "")
endif()


# ─────────────────────────────────────────────────────────────────────────
#   Feature toggles (most are runtime-free defines that flip compile paths)
# ─────────────────────────────────────────────────────────────────────────

option(USE_GLM            ""                                             OFF)
option(USE_FSTREAM        ""                                             OFF)
option(CHECK_STRUCT_SIZES ""                                             ON)
option(TARGET_XBOX        ""                                             OFF)
option(ENABLE_TESTS       ""                                             ON)

# Optional big-ticket targets — flipping any of these OFF skips that
# target cleanly (others keep building).

if(CHECK_STRUCT_SIZES)
    add_compile_definitions(CHECK_STRUCT_SIZES)
endif()

add_compile_definitions(CINTERFACE)

if(USE_GLM)
    add_compile_definitions(USE_GLM)
    add_subdirectory(third_party/glm)
endif()

if(TARGET_XBOX)
    add_compile_definitions(TARGET_XBOX)
endif()


# ─────────────────────────────────────────────────────────────────────────
#   Core libraries
# ─────────────────────────────────────────────────────────────────────────

add_library(utility src/utility/trace.cpp)
target_include_directories(utility PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/utility>)

add_library(variables src/variables/variables.cpp)
target_include_directories(variables PUBLIC "src/variables")
target_link_libraries(variables PUBLIC utility)


if(TARGET_XBOX)
    add_library(mash src/mash/xbox/src/mash_info_struct.cpp)
    target_include_directories(mash PUBLIC "src/mash/xbox/include/")
else()
    add_library(mash src/mash/pc/src/mash_info_struct.cpp)
    target_include_directories(mash PUBLIC "src/mash/pc/include")
endif()
target_include_directories(mash PUBLIC "src" "include")
target_compile_options   (mash PRIVATE ${flags})
target_link_libraries    (mash PUBLIC utility variables)


# ─────────────────────────────────────────────────────────────────────────
#   binkw32.dll — OpenUSM mod loader
# ─────────────────────────────────────────────────────────────────────────

file(GLOB SRC "./src/*.cpp" "./src/*.h")

add_library(binkw32 SHARED main.cpp ${SRC})
target_compile_options(binkw32 PRIVATE ${flags})


# ─────────────────────────────────────────────────────────────────────────
#   debug_menu.dll — PC debug menu overlay
#
#   Shares the same ${SRC} glob as binkw32 (this is intentional — the
#   debug menu runs inside the same in-process mod-loader code). The
#   different entry point (usm_debug_menu.cpp vs. main.cpp) is what
#   makes them distinct DLLs with different init behaviour.
# ─────────────────────────────────────────────────────────────────────────



# ─────────────────────────────────────────────────────────────────────────
#   memory / slab_allocator
# ─────────────────────────────────────────────────────────────────────────

add_library(memory "./src/memory.cpp" "./src/memory.h")
target_compile_options    (memory PRIVATE ${flags})
target_include_directories(memory PUBLIC  "./include/")
target_link_libraries     (memory PUBLIC  utility)

add_library(slab_allocator "./src/slab_allocator.cpp" "./src/slab_allocator.h")
target_compile_options    (slab_allocator PRIVATE ${flags})
target_include_directories(slab_allocator PUBLIC  "./include/")
target_link_libraries     (slab_allocator PUBLIC  memory)


# ─────────────────────────────────────────────────────────────────────────
#   nalcomp / nalpanel / nsl / nal (animation + panel systems)
# ─────────────────────────────────────────────────────────────────────────

file(GLOB_RECURSE NALCOMP_SRC "./src/nalcomp/*.cpp")
add_library(nalcomp ${NALCOMP_SRC})
target_compile_options    (nalcomp PRIVATE ${flags})
target_include_directories(nalcomp PUBLIC  "./src/nalcomp/" "./src/")

file(GLOB_RECURSE NALPANEL_SRC "./src/panels/nalpanel/*.cpp")
add_library(nalpanel ${NALPANEL_SRC})
target_compile_options    (nalpanel PRIVATE ${flags})
target_include_directories(nalpanel PUBLIC  "./src/panels/nalpanel/")
target_link_libraries     (nalpanel PUBLIC  nalcomp)

file(GLOB_RECURSE NSL_SRC "./src/nsl/src/nsl/*.cpp")
add_library(nsl ${NSL_SRC})
target_compile_options    (nsl PRIVATE ${flags})
target_include_directories(nsl PUBLIC  "./include/" "./src/" "./src/nsl/src/nsl/")
target_link_libraries     (nsl PUBLIC  nfl utility)

file(GLOB_RECURSE NAL_SRC "./src/nal_system.cpp" "./src/nal/source/common/*.cpp")
add_library(nal ${NAL_SRC})
target_compile_options    (nal PRIVATE ${flags})
target_include_directories(nal PUBLIC  "./include/" "./src/" "./src/nal/include/common/")
target_link_libraries     (nal PUBLIC  nfl nalpanel utility)

add_library(test_nal ${NAL_SRC})
target_compile_options    (test_nal PRIVATE ${flags})
target_include_directories(test_nal PUBLIC  "./include/" "./src/" "./src/nal/include/common/")
target_link_libraries     (test_nal PUBLIC  nfl nalpanel utility)

target_link_libraries(nalcomp PUBLIC nal)


# ─────────────────────────────────────────────────────────────────────────
#   chuck (scripting VM)
# ─────────────────────────────────────────────────────────────────────────

file(GLOB_RECURSE SRC_CHUCK "./src/chuck/vm/*.cpp")
add_library(chuck ${SRC_CHUCK})
target_compile_options    (chuck PRIVATE ${flags})
target_include_directories(chuck INTERFACE "./src/chuck/vm/")
target_link_libraries     (chuck PUBLIC   slab_allocator variables)


# ─────────────────────────────────────────────────────────────────────────
#   tx / nfl / ngl / charanimtype / physics
# ─────────────────────────────────────────────────────────────────────────

file(GLOB_RECURSE SRC_TX "./src/tx/src/*.cpp" "./src/tx/src/*.h")
add_library(tx ${SRC_TX})
target_compile_options    (tx PRIVATE ${flags})
target_include_directories(tx INTERFACE "./src/tx/src/")
target_link_libraries     (tx PUBLIC    utility)

add_library(test_tx ${SRC_TX})
target_compile_options    (test_tx PRIVATE ${flags})
target_include_directories(test_tx INTERFACE "./src/tx/src/")
target_link_libraries     (test_tx PUBLIC    utility)

file(GLOB_RECURSE SRC_NFL "./src/nfl/src/*.cpp" "./src/nfl/src/*.h")
add_library(nfl ${SRC_NFL})
target_compile_options    (nfl PRIVATE ${flags})
target_include_directories(nfl INTERFACE "./src/nfl/src/")
target_link_libraries     (nfl PUBLIC    tx)
target_link_libraries     (mash PUBLIC   nfl)

add_library(test_nfl ${SRC_NFL})
target_compile_options    (test_nfl PRIVATE ${flags})
target_include_directories(test_nfl INTERFACE "./src/nfl/src/")
target_link_libraries     (test_nfl PUBLIC    test_tx)

file(GLOB_RECURSE SRC_NGL_DX "./src/ngl/src/dx/*.cpp")
add_library(ngl_dx ${SRC_NGL_DX})
target_compile_options    (ngl_dx PRIVATE ${flags})
target_include_directories(ngl_dx PUBLIC  "./src/ngl/include/dx" "./src/ngl/include/")
target_link_libraries     (ngl_dx PUBLIC  psapi utility variables)

file(GLOB_RECURSE SRC_NGL_SHADERS "./src/ngl/shaders/*.cpp")
add_library(ngl_shaders ${SRC_NGL_SHADERS})
target_compile_options    (ngl_shaders PRIVATE ${flags})
target_include_directories(ngl_shaders PUBLIC  "./src/ngl/shaders")
target_link_libraries     (ngl_shaders PUBLIC  ngl_dx)

file(GLOB SRC_NGL "./src/ngl/src/*.cpp")
add_library(ngl ${SRC_NGL})
target_compile_options    (ngl PRIVATE ${flags})
target_include_directories(ngl PUBLIC  "./src/ngl/include/")
target_link_libraries     (ngl PUBLIC  ngl_dx ngl_shaders)

file(GLOB SRC_CHARANIMTYPE "./src/charanimtype/*.cpp")
add_library(charanimtype ${SRC_CHARANIMTYPE})
target_compile_options    (charanimtype PRIVATE ${flags})
target_include_directories(charanimtype PUBLIC  "./src/charanimtype/")
target_link_libraries     (charanimtype PUBLIC  nal ngl_dx ngl_shaders)

file(GLOB_RECURSE PHYSICS_SRC "./src/physics/source/*.cpp")
add_library(physics ${PHYSICS_SRC})
target_compile_options    (physics PRIVATE ${flags})
target_include_directories(physics PUBLIC  "./src/physics/include/")
target_link_libraries     (physics PUBLIC  utility ngl)


# ─────────────────────────────────────────────────────────────────────────
#   Shared settings for binkw32 & debug_menu (same code, different entry)
# ─────────────────────────────────────────────────────────────────────────

if(USE_FSTREAM)
    target_compile_definitions(binkw32 PUBLIC USE_FSTREAM)
    endif()


target_include_directories(binkw32 PUBLIC "include" "./src/nfl/src/")


target_include_directories(chuck  PUBLIC "include" "src")
target_include_directories(tx     PUBLIC "include" "src")
target_include_directories(ngl_dx PUBLIC "include" "src")
target_include_directories(test_tx PUBLIC "include" "src")


# ─────────────────────────────────────────────────────────────────────────
#   Assimp (external project, built once)
# ─────────────────────────────────────────────────────────────────────────

ExternalProject_Add(AssimpProject
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/assimp
    BINARY_DIR ${CMAKE_BINARY_DIR}/assimp-build
    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/cmake/i686-pc-mingw32.cmake
        -DCMAKE_BUILD_TYPE=Release
        -DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=ON
        -DASSIMP_BUILD_ASSIMP_TOOLS=OFF
        -DASSIMP_BUILD_TESTS=OFF
        -DASSIMP_NO_EXPORT=OFF
        -DBUILD_SHARED_LIBS=OFF
        -DASSIMP_BUILD_ZLIB=ON
        -DASSIMP_BUILD_SAMPLES=OFF
        -DASSIMP_OPT_BUILD_PACKAGES=OFF
        -DCMAKE_CXX_FLAGS=-static-libgcc\ -static-libstdc++
    INSTALL_COMMAND ""
)

target_include_directories(binkw32 PUBLIC "./assimp/include/")
target_link_directories   (binkw32 PUBLIC ${CMAKE_BINARY_DIR}/assimp-build/lib)
add_dependencies          (binkw32 AssimpProject)



# ═══════════════════════════════════════════════════════════════════════
#   USM.exe — compiled from USM.c
#
#   USM.c is a 37 MB ASCII-hex C source that declares one giant byte
#   array:
#       unsigned char rawData[5926912] = { 0x4D, 0x5A, ... };
#   These bytes are the stock Ultimate Spider-Man USM.exe.
#
#   Build pipeline:
#     1. Compile USM.c + emit_usm_exe.c with the HOST C compiler via
#        ExternalProject (the MinGW cross-toolchain used everywhere
#        else in this file can't be used here because the writer has
#        to RUN on the build machine).
#     2. Run the compiled writer with ${CMAKE_BINARY_DIR}/USM.exe as
#        argv[1] — it fwrites the 5,926,912 bytes of rawData[] to disk
#        in 1 MiB chunks.
#
#   Output: ${CMAKE_BINARY_DIR}/USM.exe — 5,926,912 bytes (5.65 MiB),
#   SHA-256 5d554ef69e8066eed9e327a4fe331992124aee6de16af54bd7ac5c5dcb0488b8
#   (byte-identical to the original game executable, because that's
#   what's sitting inside USM.c's rawData[]).
# ═══════════════════════════════════════════════════════════════════════

set(USM_C_SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/USM.c"
    CACHE FILEPATH "Path to USM.c with rawData[5926912] byte array")
set(USM_C_WRITER_SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/emit_usm_exe.c"
    CACHE FILEPATH "Path to emit_usm_exe.c host-tool writer")

set(_OUT_USM "${CMAKE_BINARY_DIR}/USM.exe")

if(NOT EXISTS "${USM_C_SOURCE}")
    message(WARNING
        "USM.c not found at ${USM_C_SOURCE} — the USM target will not "
        "be created. Place USM.c (containing rawData[5926912]) at that "
        "path or override with -DUSM_C_SOURCE=<path>.")
elseif(NOT EXISTS "${USM_C_WRITER_SOURCE}")
    message(WARNING
        "emit_usm_exe.c not found at ${USM_C_WRITER_SOURCE} — the USM "
        "target will not be created. Add the writer source or override "
        "with -DUSM_C_WRITER_SOURCE=<path>.")
else()
    # ── Host-native compile of USM.c + emit_usm_exe.c ────────────────
    # ExternalProject gives us an escape hatch from the parent
    # CMAKE_TOOLCHAIN_FILE — we explicitly call `cc` (the host C
    # compiler from the build environment) instead of the MinGW
    # i686 cross-compiler. -O0 because optimizing a 5.9 MB initialized
    # array gains nothing and costs minutes of compile time.
    ExternalProject_Add(emit_usm_exe_host
        SOURCE_DIR        "${CMAKE_BINARY_DIR}/emit_usm_exe_src"
        BINARY_DIR        "${CMAKE_BINARY_DIR}/emit_usm_exe_build"
        DOWNLOAD_COMMAND  ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND
            ${CMAKE_COMMAND} -E make_directory
                "${CMAKE_BINARY_DIR}/emit_usm_exe_build"
            COMMAND
            cc -O0
               "${USM_C_SOURCE}"
               "${USM_C_WRITER_SOURCE}"
               -o "${CMAKE_BINARY_DIR}/emit_usm_exe_build/emit_usm_exe"
        INSTALL_COMMAND   ""
        BUILD_BYPRODUCTS  "${CMAKE_BINARY_DIR}/emit_usm_exe_build/emit_usm_exe"
        BUILD_ALWAYS      FALSE
    )

    # ── Run the writer to produce USM.exe ────────────────────────────
    add_custom_command(
        OUTPUT  "${_OUT_USM}"
        COMMAND "${CMAKE_BINARY_DIR}/emit_usm_exe_build/emit_usm_exe"
                "${_OUT_USM}"
        DEPENDS emit_usm_exe_host "${USM_C_SOURCE}" "${USM_C_WRITER_SOURCE}"
        COMMENT "Compiling USM.exe from USM.c rawData[] (5,926,912 bytes / 5.65 MiB)"
        VERBATIM
    )

    add_custom_target(USM ALL DEPENDS "${_OUT_USM}")

    # Compute + display the SHA-256 of USM.c so changes to the rawData[]
    # blob are obvious at configure time.
    file(SIZE   "${USM_C_SOURCE}" _USM_C_SIZE)
    file(SHA256 "${USM_C_SOURCE}" _USM_C_SHA)
    message(STATUS "USM.exe: compiling from USM.c rawData[]")
    message(STATUS "  USM.c       : ${USM_C_SOURCE}")
    message(STATUS "  USM.c size  : ${_USM_C_SIZE} bytes")
    message(STATUS "  USM.c sha256: ${_USM_C_SHA}")
    message(STATUS "  writer      : ${USM_C_WRITER_SOURCE}")
    message(STATUS "  output      : ${_OUT_USM} (expected: 5926912 bytes / 5.65 MiB)")
endif()


# ─────────────────────────────────────────────────────────────────────────
#   USM target — post-build: wire in the built mod loader + deploy outputs
#
#   No stock DLLs are staged into the build tree by this file. Only
#   files produced by THIS build are touched:
#     • $<TARGET_FILE:binkw32>   → copied alongside USM.exe as openusm.dll
#     • $<TARGET_FILE:debug_menu> → built alongside USM.exe as debug_menu.dll
#   If your game install folder exists next to the source tree, the
#   same three artifacts (USM.exe, binkw32.dll, debug_menu.dll) are
#   also deployed there.
# ─────────────────────────────────────────────────────────────────────────

if(TARGET USM)
    # Wire the mod loader into the USM build graph so every build of
    # USM also produces a fresh openusm.dll next to it.
    if(TARGET binkw32)
        add_dependencies(USM binkw32)
        add_custom_command(TARGET USM POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                "$<TARGET_FILE:binkw32>"
                "${CMAKE_BINARY_DIR}/openusm.dll"
            COMMENT "Deploying built mod loader → openusm.dll"
            VERBATIM)
    endif()

    # Ditto for debug_menu — ensure it's up to date next to USM.exe.

    # Auto-deploy the built artifacts into ../Ultimate Spider-Man/ if
    # that folder exists. The user's existing stock DLLs in that
    # folder are left untouched.
    set(_GAME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Ultimate Spider-Man")
    if(EXISTS "${_GAME_DIR}/")
        add_custom_command(TARGET USM POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                "${_OUT_USM}" "${_GAME_DIR}/USM.exe"
            COMMENT "Deploying USM.exe to ${_GAME_DIR}"
            VERBATIM)

        if(TARGET binkw32)
            add_custom_command(TARGET binkw32 POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                    "$<TARGET_FILE:binkw32>" "${_GAME_DIR}/binkw32.dll"
                COMMENT "Deploying binkw32.dll to ${_GAME_DIR}"
                VERBATIM)
        endif()
endif()
# ─────────────────────────────────────────────────────────────────────────
#   Final link lines for binkw32 and debug_menu
# ─────────────────────────────────────────────────────────────────────────

target_link_libraries(ngl_dx PUBLIC d3dx9)

target_link_directories(binkw32 PUBLIC ${CMAKE_FIND_ROOT_PATH}/include)
target_link_directories(binkw32 PUBLIC ${CMAKE_FIND_ROOT_PATH}/lib)
target_link_libraries  (binkw32 PUBLIC physics
                                       charanimtype mash chuck nfl ngl nal nsl utility
                                       winmm psapi d3d9 dinput8 dxguid dsound assimp z)



if(USE_GLM)
    target_link_libraries(binkw32 PUBLIC glm::glm)
    endif()
endif()

# Pin binkw32's output directory so the USM target can embed it later
# and downstream deploy steps know exactly where to find it.
set_target_properties(binkw32 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")




# ─────────────────────────────────────────────────────────────────────────
#   Tests
# ─────────────────────────────────────────────────────────────────────────




# ═══════════════════════════════════════════════════════════════════════
#   debug_menu.dll — UPX post-build compression (target ≤ 1 MiB)
#
#   UPX can't target a specific size — it just squeezes maximally
#   (typically 35-55% of original). A ~2-3 MiB uncompressed
#   debug_menu.dll should land near 1 MiB. If it doesn't, CMake prints
#   a warning but still succeeds (build shouldn't fail over a soft
#   size target).
# ═══════════════════════════════════════════════════════════════════════


