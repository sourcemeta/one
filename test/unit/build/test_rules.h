#ifndef SOURCEMETA_ONE_BUILD_TEST_RULES_H_
#define SOURCEMETA_ONE_BUILD_TEST_RULES_H_

#include <sourcemeta/one/build.h>

namespace test_rules {

enum : sourcemeta::one::BuildPlan::Action::Type {
  ACTION_PRIMARY,
  ACTION_METADATA,
  ACTION_WEB,
  ACTION_LISTING,
  ACTION_VERSION,
  ACTION_CONFIGURATION,
  ACTION_COMMENT,
  ACTION_ROUTES,
  ACTION_REMOVE
};

enum : sourcemeta::one::BuildPlan::Type { MODE_HEADLESS, MODE_FULL };

inline constexpr sourcemeta::one::DeltaRuleSet<3, 1, 4, 2> RULES{
    .leaves = {{
        {.action = ACTION_PRIMARY,
         .base = 0,
         .filename = "primary.bin",
         .gate = sourcemeta::one::TargetGate::Always,
         .dirty = sourcemeta::one::DirtyOverride::Normal,
         .is_root = true,
         .combine_only = false,
         .container_target = false,
         .tracks_dependencies = false,
         .dependencies =
             {{{.source = sourcemeta::one::DependencySource::ExternalSource,
                .base = 0,
                .filename = nullptr},
               {.source = sourcemeta::one::DependencySource::ExternalConfig,
                .base = 0,
                .filename = nullptr}}},
         .dependency_count = 2},

        {.action = ACTION_METADATA,
         .base = 1,
         .filename = "metadata.bin",
         .gate = sourcemeta::one::TargetGate::Always,
         .dirty = sourcemeta::one::DirtyOverride::Normal,
         .is_root = false,
         .combine_only = false,
         .container_target = true,
         .tracks_dependencies = true,
         .dependencies = {{{.source = sourcemeta::one::DependencySource::Base,
                            .base = 0,
                            .filename = "primary.bin"}}},
         .dependency_count = 1},

        {.action = ACTION_WEB,
         .base = 1,
         .filename = "web.bin",
         .gate = sourcemeta::one::TargetGate::OnlyInFullMode,
         .dirty = sourcemeta::one::DirtyOverride::Normal,
         .is_root = false,
         .combine_only = false,
         .container_target = false,
         .tracks_dependencies = false,
         .dependencies = {{{.source = sourcemeta::one::DependencySource::Base,
                            .base = 1,
                            .filename = "metadata.bin"}}},
         .dependency_count = 1},
    }},
    .containers = {{
        {.action = ACTION_LISTING,
         .base = 1,
         .filename = "listing.bin",
         .gate = sourcemeta::one::TargetGate::Always,
         .scope = sourcemeta::one::ContainerScope::AllContainers,
         .only_full_rebuild = false,
         .is_listing = true,
         .dependencies =
             {{{.kind = sourcemeta::one::ContainerDependencyKind::LeafMetadata,
                .filename = nullptr},
               {.kind =
                    sourcemeta::one::ContainerDependencyKind::ChildContainers,
                .filename = nullptr}}},
         .dependency_count = 2},
    }},
    .globals = {{
        {.action = ACTION_VERSION,
         .filename = "version.json",
         .trigger = sourcemeta::one::GlobalTrigger::WithVersion,
         .external_config_anchor = false,
         .dependencies = {},
         .dependency_count = 0},
        {.action = ACTION_CONFIGURATION,
         .filename = "configuration.json",
         .trigger = sourcemeta::one::GlobalTrigger::FullRebuild,
         .external_config_anchor = true,
         .dependencies = {},
         .dependency_count = 0},
        {.action = ACTION_COMMENT,
         .filename = "comment.json",
         .trigger = sourcemeta::one::GlobalTrigger::WithComment,
         .external_config_anchor = false,
         .dependencies = {},
         .dependency_count = 0},
        {.action = ACTION_ROUTES,
         .filename = "routes.bin",
         .trigger = sourcemeta::one::GlobalTrigger::OnModeChange,
         .external_config_anchor = false,
         .dependencies = {},
         .dependency_count = 0},
    }},
    .directories = {{"primary", "secondary"}},
    .sentinel = "%",
    .remove_action = ACTION_REMOVE,
    .full_mode = MODE_FULL,
};

} // namespace test_rules

#endif
