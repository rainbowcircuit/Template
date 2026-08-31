cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

if(NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS deployment target" FORCE)
endif()

project(TemplatePlugin VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Statically link the MSVC runtime so the shipped plugin doesn't require users to
# have the Visual C++ Redistributable installed. Without this the binary imports
# MSVCP140.dll / VCRUNTIME140.dll / VCRUNTIME140_1.dll, which are absent on a clean
# Windows install, and the DAW's plugin scan fails to load it.
# Must be set before targets are created (including JUCE's).
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Fetch JUCE from GitHub
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.12
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(JUCE)

# Plugin configuration
# NOTE: change COMPANY_NAME / BUNDLE_ID / PLUGIN_MANUFACTURER_CODE / PLUGIN_CODE
# for every new project built from this template.
juce_add_plugin(TemplatePlugin
    COMPANY_NAME "Your Company"
    BUNDLE_ID "com.yourcompany.TemplatePlugin"
    PRODUCT_NAME "TemplatePlugin"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    PLUGIN_MANUFACTURER_CODE Ycom
    PLUGIN_CODE Tmpl
    FORMATS VST3 AU Standalone
    NEEDS_WEBVIEW2 TRUE
)

juce_generate_juce_header(TemplatePlugin)

# ------------------------------------------------------------------------------
# Web UI: bundle lit + three (npm) with esbuild, embed the result as binary data
# ------------------------------------------------------------------------------
find_program(NPM_EXECUTABLE npm REQUIRED)

set(TEMPLATE_UI_DIR      "${CMAKE_SOURCE_DIR}/src/ui")
set(TEMPLATE_UI_DIST_DIR "${TEMPLATE_UI_DIR}/dist")
set(TEMPLATE_UI_HTML      "${TEMPLATE_UI_DIST_DIR}/index.html")
set(TEMPLATE_UI_BUNDLE_JS "${TEMPLATE_UI_DIST_DIR}/app.bundle.js")
set(TEMPLATE_UI_NPM_STAMP "${TEMPLATE_UI_DIR}/node_modules/.install-stamp")

file(GLOB_RECURSE TEMPLATE_UI_SOURCES CONFIGURE_DEPENDS "${TEMPLATE_UI_DIR}/public/*")

# Reinstall node_modules whenever package.json/package-lock.json change.
add_custom_command(
    OUTPUT "${TEMPLATE_UI_NPM_STAMP}"
    COMMAND ${NPM_EXECUTABLE} install
    COMMAND ${CMAKE_COMMAND} -E touch "${TEMPLATE_UI_NPM_STAMP}"
    WORKING_DIRECTORY "${TEMPLATE_UI_DIR}"
    DEPENDS "${TEMPLATE_UI_DIR}/package.json" "${TEMPLATE_UI_DIR}/package-lock.json"
    COMMENT "Installing TemplatePlugin UI npm dependencies (lit, three, esbuild)"
    VERBATIM
)

# Re-bundle whenever any UI source file (or the build script) changes.
add_custom_command(
    OUTPUT "${TEMPLATE_UI_HTML}" "${TEMPLATE_UI_BUNDLE_JS}"
    COMMAND ${NPM_EXECUTABLE} run build
    WORKING_DIRECTORY "${TEMPLATE_UI_DIR}"
    DEPENDS "${TEMPLATE_UI_NPM_STAMP}" "${TEMPLATE_UI_DIR}/build.mjs" ${TEMPLATE_UI_SOURCES}
    COMMENT "Bundling TemplatePlugin UI with esbuild"
    VERBATIM
)

add_custom_target(TemplateUIBundle DEPENDS "${TEMPLATE_UI_HTML}" "${TEMPLATE_UI_BUNDLE_JS}")

juce_add_binary_data(TemplateUIData
    NAMESPACE TemplateUIData
    SOURCES
        "${TEMPLATE_UI_HTML}"
        "${TEMPLATE_UI_BUNDLE_JS}"
)
add_dependencies(TemplateUIData TemplateUIBundle)

# Source files
target_sources(TemplatePlugin
    PRIVATE
        src/plugin/PluginProcessor.cpp
        src/plugin/PluginProcessor.h
        src/plugin/PluginEditor.cpp
        src/plugin/PluginEditor.h
        src/params/Parameters.cpp
        src/params/Parameters.h
        src/params/ParameterList.h
        src/params/PresetLoader.cpp
        src/params/PresetLoader.h
)

# Preprocessor definitions
target_compile_definitions(TemplatePlugin
    PUBLIC
        JUCE_WEB_BROWSER=1
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_USE_WIN_WEBVIEW2_WITH_STATIC_LINKING=1
)

# Link JUCE modules.
# Add further DSP/math dependencies here (e.g. more juce_dsp usage, or an
# Eigen FetchContent) as a real project's processing needs grow.
target_link_libraries(TemplatePlugin
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_extra
        TemplateUIData
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

juce_enable_copy_plugin_step(TemplatePlugin)
