#ifndef SOURCEMETA_ONE_BUILD_RULES_H_
#define SOURCEMETA_ONE_BUILD_RULES_H_

#include <sourcemeta/one/build.h>

#include <array>   // std::array
#include <cstdint> // std::uint8_t, std::size_t

namespace sourcemeta::one {

static constexpr const char *SCHEMAS_DIRECTORY = "schemas";
static constexpr const char *EXPLORER_DIRECTORY = "explorer";

enum class TargetBase : std::uint8_t { Schema, Explorer };

enum class TargetGate : std::uint8_t { Always, FullOnly, IfEvaluate };

enum class DirtyOverride : std::uint8_t {
  Normal,
  ForceIfAffected,
  ForceOnGraphChange
};

enum class DependencySource : std::uint8_t {
  SchemaBase,
  ExplorerBase,
  GlobalOutput,
  ExternalSource,
  ExternalConfig
};

struct DependencyReference {
  DependencySource source;
  const char *filename;
};

static constexpr std::size_t MAX_DEPENDENCIES_PER_RULE = 4;

struct PerSchemaRule {
  BuildPlan::Action::Type action;
  TargetBase base;
  const char *filename;
  TargetGate gate;
  DirtyOverride dirty;
  bool is_root;
  std::array<DependencyReference, MAX_DEPENDENCIES_PER_RULE> dependencies;
  std::uint8_t dependency_count;
};

static constexpr std::array<PerSchemaRule, 13> PER_SCHEMA_RULES{{
    {.action = BuildPlan::Action::Type::Materialise,
     .base = TargetBase::Schema,
     .filename = "schema.metapack",
     .gate = TargetGate::Always,
     .dirty = DirtyOverride::Normal,
     .is_root = true,
     .dependencies =
         {{{.source = DependencySource::ExternalSource, .filename = nullptr},
           {.source = DependencySource::ExternalConfig, .filename = nullptr}}},
     .dependency_count = 2},

    {.action = BuildPlan::Action::Type::Positions,
     .base = TargetBase::Schema,
     .filename = "positions.metapack",
     .gate = TargetGate::Always,
     .dirty = DirtyOverride::Normal,
     .is_root = false,
     .dependencies = {{{.source = DependencySource::SchemaBase,
                        .filename = "schema.metapack"}}},
     .dependency_count = 1},

    {.action = BuildPlan::Action::Type::Locations,
     .base = TargetBase::Schema,
     .filename = "locations.metapack",
     .gate = TargetGate::Always,
     .dirty = DirtyOverride::Normal,
     .is_root = false,
     .dependencies = {{{.source = DependencySource::SchemaBase,
                        .filename = "schema.metapack"}}},
     .dependency_count = 1},

    {.action = BuildPlan::Action::Type::Dependencies,
     .base = TargetBase::Schema,
     .filename = "dependencies.metapack",
     .gate = TargetGate::Always,
     .dirty = DirtyOverride::Normal,
     .is_root = false,
     .dependencies = {{{.source = DependencySource::SchemaBase,
                        .filename = "schema.metapack"}}},
     .dependency_count = 1},

    {.action = BuildPlan::Action::Type::Stats,
     .base = TargetBase::Schema,
     .filename = "stats.metapack",
     .gate = TargetGate::Always,
     .dirty = DirtyOverride::Normal,
     .is_root = false,
     .dependencies = {{{.source = DependencySource::SchemaBase,
                        .filename = "schema.metapack"}}},
     .dependency_count = 1},

    {.action = BuildPlan::Action::Type::Health,
     .base = TargetBase::Schema,
     .filename = "health.metapack",
     .gate = TargetGate::Always,
     .dirty = DirtyOverride::Normal,
     .is_root = false,
     .dependencies = {{{.source = DependencySource::SchemaBase,
                        .filename = "schema.metapack"},
                       {.source = DependencySource::SchemaBase,
                        .filename = "dependencies.metapack"}}},
     .dependency_count = 2},

    {.action = BuildPlan::Action::Type::Bundle,
     .base = TargetBase::Schema,
     .filename = "bundle.metapack",
     .gate = TargetGate::Always,
     .dirty = DirtyOverride::Normal,
     .is_root = false,
     .dependencies = {{{.source = DependencySource::SchemaBase,
                        .filename = "schema.metapack"},
                       {.source = DependencySource::SchemaBase,
                        .filename = "dependencies.metapack"}}},
     .dependency_count = 2},

    {.action = BuildPlan::Action::Type::Editor,
     .base = TargetBase::Schema,
     .filename = "editor.metapack",
     .gate = TargetGate::Always,
     .dirty = DirtyOverride::Normal,
     .is_root = false,
     .dependencies = {{{.source = DependencySource::SchemaBase,
                        .filename = "bundle.metapack"}}},
     .dependency_count = 1},

    {.action = BuildPlan::Action::Type::BlazeExhaustive,
     .base = TargetBase::Schema,
     .filename = "blaze-exhaustive.metapack",
     .gate = TargetGate::IfEvaluate,
     .dirty = DirtyOverride::Normal,
     .is_root = false,
     .dependencies = {{{.source = DependencySource::SchemaBase,
                        .filename = "bundle.metapack"}}},
     .dependency_count = 1},

    {.action = BuildPlan::Action::Type::BlazeFast,
     .base = TargetBase::Schema,
     .filename = "blaze-fast.metapack",
     .gate = TargetGate::IfEvaluate,
     .dirty = DirtyOverride::Normal,
     .is_root = false,
     .dependencies = {{{.source = DependencySource::SchemaBase,
                        .filename = "bundle.metapack"}}},
     .dependency_count = 1},

    {.action = BuildPlan::Action::Type::SchemaMetadata,
     .base = TargetBase::Explorer,
     .filename = "schema.metapack",
     .gate = TargetGate::Always,
     .dirty = DirtyOverride::Normal,
     .is_root = false,
     .dependencies = {{{.source = DependencySource::SchemaBase,
                        .filename = "schema.metapack"},
                       {.source = DependencySource::SchemaBase,
                        .filename = "health.metapack"},
                       {.source = DependencySource::SchemaBase,
                        .filename = "dependencies.metapack"}}},
     .dependency_count = 3},

    {.action = BuildPlan::Action::Type::Dependents,
     .base = TargetBase::Schema,
     .filename = "dependents.metapack",
     .gate = TargetGate::Always,
     .dirty = DirtyOverride::ForceOnGraphChange,
     .is_root = false,
     .dependencies = {{{.source = DependencySource::GlobalOutput,
                        .filename = "dependency-tree.metapack"}}},
     .dependency_count = 1},

    {.action = BuildPlan::Action::Type::WebSchema,
     .base = TargetBase::Explorer,
     .filename = "schema-html.metapack",
     .gate = TargetGate::FullOnly,
     .dirty = DirtyOverride::ForceIfAffected,
     .is_root = false,
     .dependencies = {{{.source = DependencySource::ExplorerBase,
                        .filename = "schema.metapack"},
                       {.source = DependencySource::SchemaBase,
                        .filename = "dependencies.metapack"},
                       {.source = DependencySource::SchemaBase,
                        .filename = "health.metapack"},
                       {.source = DependencySource::SchemaBase,
                        .filename = "dependents.metapack"}}},
     .dependency_count = 4},
}};

enum class AggregateOutputBase : std::uint8_t { Output, Explorer };

struct AggregateRule {
  BuildPlan::Action::Type action;
  const char *output_filename;
  AggregateOutputBase output_base;
  TargetBase collector_base;
  const char *collector_filename;
};

static constexpr std::array<AggregateRule, 2> AGGREGATE_RULES{{
    {.action = BuildPlan::Action::Type::DependencyTree,
     .output_filename = "dependency-tree.metapack",
     .output_base = AggregateOutputBase::Output,
     .collector_base = TargetBase::Schema,
     .collector_filename = "dependencies.metapack"},

    {.action = BuildPlan::Action::Type::SearchIndex,
     .output_filename = "search.metapack",
     .output_base = AggregateOutputBase::Explorer,
     .collector_base = TargetBase::Explorer,
     .collector_filename = "schema.metapack"},
}};

enum class DirectoryScope : std::uint8_t { AllDirectories, NonRoot, RootOnly };

enum class DirectoryDependencyKind : std::uint8_t {
  SchemaMetadata,
  ChildDirectories,
  SameDirectoryTarget,
  ExternalConfig
};

struct DirectoryDependency {
  DirectoryDependencyKind kind;
  const char *filename;
};

static constexpr std::size_t MAX_DIRECTORY_DEPENDENCIES = 2;

struct DirectoryRule {
  BuildPlan::Action::Type action;
  const char *filename;
  TargetGate gate;
  DirectoryScope scope;
  bool only_full_rebuild;
  std::array<DirectoryDependency, MAX_DIRECTORY_DEPENDENCIES> dependencies;
  std::uint8_t dependency_count;
};

static constexpr std::array<DirectoryRule, 4> DIRECTORY_RULES{{
    {.action = BuildPlan::Action::Type::DirectoryList,
     .filename = "directory.metapack",
     .gate = TargetGate::Always,
     .scope = DirectoryScope::AllDirectories,
     .only_full_rebuild = false,
     .dependencies = {{{.kind = DirectoryDependencyKind::SchemaMetadata,
                        .filename = nullptr},
                       {.kind = DirectoryDependencyKind::ChildDirectories,
                        .filename = nullptr}}},
     .dependency_count = 2},

    {.action = BuildPlan::Action::Type::WebIndex,
     .filename = "directory-html.metapack",
     .gate = TargetGate::FullOnly,
     .scope = DirectoryScope::RootOnly,
     .only_full_rebuild = false,
     .dependencies = {{{.kind = DirectoryDependencyKind::SameDirectoryTarget,
                        .filename = "directory.metapack"}}},
     .dependency_count = 1},

    {.action = BuildPlan::Action::Type::WebDirectory,
     .filename = "directory-html.metapack",
     .gate = TargetGate::FullOnly,
     .scope = DirectoryScope::NonRoot,
     .only_full_rebuild = false,
     .dependencies = {{{.kind = DirectoryDependencyKind::SameDirectoryTarget,
                        .filename = "directory.metapack"}}},
     .dependency_count = 1},

    {.action = BuildPlan::Action::Type::WebNotFound,
     .filename = "404.metapack",
     .gate = TargetGate::FullOnly,
     .scope = DirectoryScope::RootOnly,
     .only_full_rebuild = true,
     .dependencies = {{{.kind = DirectoryDependencyKind::ExternalConfig,
                        .filename = nullptr}}},
     .dependency_count = 1},
}};

enum class GlobalTrigger : std::uint8_t {
  FullRebuild,
  WebTransition,
  CommentPresent
};

struct GlobalRule {
  BuildPlan::Action::Type action;
  const char *filename;
  GlobalTrigger trigger;
};

static constexpr std::array<GlobalRule, 4> GLOBAL_RULES{{
    {.action = BuildPlan::Action::Type::Version,
     .filename = "version.json",
     .trigger = GlobalTrigger::FullRebuild},
    {.action = BuildPlan::Action::Type::Configuration,
     .filename = "configuration.json",
     .trigger = GlobalTrigger::FullRebuild},
    {.action = BuildPlan::Action::Type::Comment,
     .filename = "comment.json",
     .trigger = GlobalTrigger::CommentPresent},
    {.action = BuildPlan::Action::Type::Routes,
     .filename = "routes.bin",
     .trigger = GlobalTrigger::WebTransition},
}};

static constexpr auto find_global_rule(BuildPlan::Action::Type action)
    -> const GlobalRule & {
  for (const auto &rule : GLOBAL_RULES) {
    if (rule.action == action) {
      return rule;
    }
  }

  return GLOBAL_RULES[0];
}

static constexpr const auto &VERSION_RULE =
    find_global_rule(BuildPlan::Action::Type::Version);
static constexpr const auto &CONFIGURATION_RULE =
    find_global_rule(BuildPlan::Action::Type::Configuration);
static constexpr const auto &COMMENT_RULE =
    find_global_rule(BuildPlan::Action::Type::Comment);

static constexpr auto find_root_rule() -> const PerSchemaRule & {
  for (const auto &rule : PER_SCHEMA_RULES) {
    if (rule.is_root) {
      return rule;
    }
  }

  return PER_SCHEMA_RULES[0];
}

static constexpr const auto &ROOT_RULE = find_root_rule();

static constexpr auto find_rule_by_action(BuildPlan::Action::Type action)
    -> const PerSchemaRule & {
  for (const auto &rule : PER_SCHEMA_RULES) {
    if (rule.action == action) {
      return rule;
    }
  }

  return PER_SCHEMA_RULES[0];
}

static constexpr const auto &SCHEMA_METADATA_RULE =
    find_rule_by_action(BuildPlan::Action::Type::SchemaMetadata);

} // namespace sourcemeta::one

#endif
