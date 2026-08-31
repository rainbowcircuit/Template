#pragma once

// Single source of truth for every scalar plugin parameter's id, display name,
// JUCE parameter type, range/choices, and default value. Included by both
// Parameters.h/.cpp (which owns the long-lived ParameterInstance objects read
// on the audio thread) and PluginEditor.h/.cpp (which owns the
// WebSliderRelay/WebSliderParameterAttachment objects rebuilt each time the
// editor opens) so both sides expand into their own compile-time named
// members from one list instead of duplicating each id by hand in both places.
//
// Add a parameter by adding one line below to the matching macro - it will
// automatically become an APVTS parameter, a Parameters::<id> ParameterInstance
// member, and a <id>Relay/<id>Attachment pair in the editor.

#define TEMPLATE_FLOAT_PARAMS(X) \
    X(gain, "Gain", -60.0f, 12.0f, 0.01f, 2.0f, 0.0f)

#define TEMPLATE_BOOL_PARAMS(X)

#define TEMPLATE_INT_PARAMS(X)

#define TEMPLATE_CHOICE_PARAMS(X)

#define TEMPLATE_ALL_SCALAR_PARAMS(X) \
    TEMPLATE_FLOAT_PARAMS(X) \
    TEMPLATE_BOOL_PARAMS(X) \
    TEMPLATE_INT_PARAMS(X) \
    TEMPLATE_CHOICE_PARAMS(X)
