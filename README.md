# TemplatePlugin

A minimal JUCE + WebView audio plugin skeleton: CMake wiring for a JUCE 8
WebView UI backed by a bundled Lit + Three.js frontend, a single "gain"
parameter wired end-to-end (APVTS -> WebSliderRelay -> Lit component), a
full preset save/load system, and a small library of reusable Lit UI
primitives (a canvas-drawable button/slider, a Three.js visualizer).

## Starting a new plugin from this template

1. Copy this directory to your new project's location.
2. In `CMakeLists.txt`: change `project(...)`/`juce_add_plugin(...)`'s
   `COMPANY_NAME`, `BUNDLE_ID`, `PLUGIN_MANUFACTURER_CODE`, and `PLUGIN_CODE`.
3. Rename `TemplatePlugin`/`Template` throughout (CMake target name, the
   `TemplateAudioProcessor`/`TemplateAudioProcessorEditor` classes, the
   `TemplateUIData` binary-data namespace) to your project's name.
4. Add parameters in `src/params/ParameterList.h` — each entry you add to
   the `TEMPLATE_*_PARAMS` macros automatically becomes an APVTS parameter,
   a smoothed `ParameterInstance`, and a WebView relay/attachment pair.
5. Replace the `juce::dsp::Gain` in `PluginProcessor.cpp` with your DSP.

## Building

```sh
cmake -B build
cmake --build build --target TemplatePlugin_Standalone
```

The build automatically runs `npm install` / `npm run build` under
`src/ui/` and embeds the resulting `dist/index.html` + `dist/app.bundle.js`
as binary data.

## Frontend dev loop

```sh
cd src/ui
npm install
npm run dev   # serves src/ui/public unbundled on http://localhost:4000
```

Run a **Debug** build of the plugin while the dev server is running —
`PluginEditor` points the WebView at `localhost:4000` in Debug and at the
packaged resources in Release.
