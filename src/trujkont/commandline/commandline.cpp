#include <iostream>
#include <utility>
#include <string>

#include "trujkont/commandline/commandline.hpp"

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/common.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/trim.hpp>

#include <fmt/format.h>
#include <fmt/color.h>

namespace
{

auto whitespace(char const c) // NOLINT
{
  return std::isspace(c);
};

} // namespace

auto Commandline::run() -> void
{
  while(not should_stop) {
    auto line = std::string();

    fmt::print("> ");
    std::getline(std::cin, line);

    dispatch_command(parse_commandline(std::move(line)));
  }
}

auto Commandline::stop() -> void
{
  should_stop = true;
}

auto Commandline::add_command(CommandName name, CommandCallback callback) -> bool
{
  auto [_it, did_emplace] = commands.try_emplace(std::move(name), std::move(callback));

  return did_emplace;
}

auto Commandline::parse_commandline(std::string line) -> CommandlineResult
{
  namespace rg = ranges;
  namespace vw = rg::views;

  auto elems = line | vw::trim(whitespace) | vw::split(' ');

  auto command_name = elems | vw::take(1) | vw::join | rg::to<std::string>();
  auto args = elems | vw::drop(1) | rg::to<std::vector<std::string>>();

  return CommandlineResult { std::move(command_name), std::move(args) };
}

auto Commandline::dispatch_command(CommandlineResult result) -> void
{
  auto [name, args] = std::move(result);

  auto const command_it = commands.find(name);

  if(command_it == commands.end()) {
    fmt::print(fg(fmt::color::indian_red), "Command '{}' not found.\n", name);
    return;
  }

  auto const command_result = command_it->second(std::move(args));

  if(command_result.has_value()) {
    fmt::print(fg(fmt::color::sky_blue), "{}\n", command_result.value());
  } else {
    fmt::print(fg(fmt::color::indian_red), "Error while running command '{}':\n\t{}\n", name, command_result.error());
  }
}
