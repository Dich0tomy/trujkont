#pragma once

#include <unordered_map>
#include <string_view>
#include <functional>
#include <thread>
#include <string>

#include <tl/expected.hpp>

class Commandline
{
public:
  using CommandName = std::string;

  using CommandArgs = std::vector<std::string>;
  using CommandResult = tl::expected<std::string, std::string>;
  using CommandCallback = std::function<CommandResult(CommandArgs)>;

  Commandline() = default;

  auto run() -> void;

  auto add_command(CommandName name, CommandCallback callback) -> bool;

private:
  using CommandlineResult = std::pair<CommandName, CommandArgs>;

  auto static parse_commandline(std::string line) -> CommandlineResult;

  auto dispatch_command(CommandlineResult result) -> void;

  std::unordered_map<CommandName, CommandCallback> commands;
};
