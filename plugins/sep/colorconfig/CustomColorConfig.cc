//
// Created by developer on 01-06-21.
//
#include "CustomColorConfig.hh"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <list>
#include <unordered_set>

namespace pt = boost::property_tree;

ColorConfig::ColorConfig() = default;

void ColorConfig::loadFile(std::string file) {
  // Set the defaultCMYK true, will become false if there is any color defined
  // in the config file
  defaultCMYK = true;
  colors.clear();

  // reserve the first 4 spaces for CMYK
  colors.emplace_back(nullptr);
  colors.emplace_back(nullptr);
  colors.emplace_back(nullptr);
  colors.emplace_back(nullptr);

  pt::ptree root;
  boost::filesystem::path full_path(boost::filesystem::current_path());
  if (file == "colours.json") {
    full_path.append(file);
  } else {
    full_path = file;
  }
  if (!boost::filesystem::exists(
          full_path)) { // File does not exist on file system
    std::cout << "WARNING: Colours file does not exist at path: " +
                     full_path.string() + "\n";
    std::cout << "Loading default CMYK \n";
    addNonExistentDefaultColors();
    return;
  }

  try {
    pt::read_json(full_path.string(), root);
  } catch (const std::exception &e) {
    // Loading didnt work
    std::cout << "WARNING: Loading colours file failed. file at: " +
                     full_path.string() + " is most likely ill formed.\n";
    std::cout << "Loading default CMYK \n";
    addNonExistentDefaultColors();
    return;
  }

  std::unordered_set<std::string> seenNamesAndAliases = {};
  seenNamesAndAliases.insert("V"); // Insert placeholder for varnish

  std::cout << "Loading colour config file. NOTE: v is reserved for varnish, "
               "so should not be defined as name or alias!!\n";
  for (pt::ptree::value_type &v : root.get_child("colours")) {
    parseColor(v, seenNamesAndAliases);
  }
  addNonExistentDefaultColors();
}

void ColorConfig::addNonExistentDefaultColors() {

  // If no cyan configuration exists, add the default configuration
  if (!getColorByNameOrAlias("c")) {
    CustomColor::Ptr newColour =
        boost::make_shared<CustomColor>("C", 1, 0, 0, 0);

    colors[0] = newColour;
  }

  // If no magenta configuration exists, add the default configuration
  if (!getColorByNameOrAlias("m")) {
    CustomColor::Ptr newColour =
        boost::make_shared<CustomColor>("M", 0, 1, 0, 0);

    colors[1] = newColour;
  }

  // If no yellow configuration exists, add the default configuration
  if (!getColorByNameOrAlias("y")) {
    CustomColor::Ptr newColour =
        boost::make_shared<CustomColor>("Y", 0, 0, 1, 0);

    colors[2] = newColour;
  }

  // If no key configuration exists, add the default configuration
  if (!getColorByNameOrAlias("k")) {
    CustomColor::Ptr newColour =
        boost::make_shared<CustomColor>("K", 0, 0, 0, 1);

    colors[3] = newColour;
  }
}

CustomColor::Ptr ColorConfig::getColorByNameOrAlias(const std::string &name) {
  auto definedColors = getDefinedColors();
  for (auto &color : definedColors) {
    if (isColor(color, name))
      return color;
  }
  return nullptr;
}

std::vector<CustomColor::Ptr> ColorConfig::getDefinedColors() { return colors; }

void ColorConfig::parseColor(
    pt::ptree::value_type &v,
    std::unordered_set<std::string> &seenNamesAndAliases) {
  auto name = v.second.get<std::string>("name");
  boost::algorithm::to_upper(name); // Convert the name to uppercase

  // Check if this name has not yet been seen before
  if (seenNamesAndAliases.find(name) != seenNamesAndAliases.end()) {
    // It exists
    std::cout << "ERROR: Duplicate name or alias: " << name << "!\n";
    // Color already exists, so it should not be added to the colors
    return;
  }
  // Color is new, so we can add it to the defined colors
  // First add the name to the seen names and aliases
  seenNamesAndAliases.insert(name);

  auto c = v.second.get<double>("cMultiplier");
  auto m = v.second.get<double>("mMultiplier");
  auto y = v.second.get<double>("yMultiplier");
  auto k = v.second.get<double>("kMultiplier");
  CustomColor::Ptr newColour =
      boost::make_shared<CustomColor>(name, c, m, y, k);

  // Initialise aliases vector
  std::vector<std::string> validAliases = {};
  // Try to load aliases, if the field exists
  try {
    // Get the aliases array
    pt::ptree array = v.second.get_child("aliasses");
    // Initialise an iterator over the aliasses array
    pt::ptree::iterator iterator = array.begin();

    // Store aliases in vector
    for (; iterator != array.end(); iterator++) {
      // Load alias with uppercase included
      auto alias = iterator->second.get_value<std::string>();
      // Convert alias to all upercase
      boost::algorithm::to_upper(alias);

      // Test if an alias already exists in a different colour
      if (seenNamesAndAliases.find(alias) != seenNamesAndAliases.end()) {
        // It exists
        std::cout << "ERROR: Duplicate alias: " + alias + "!\n";
      } else {
        // It is a new alias
        std::cout << "New alias: " + alias + "\n";
        seenNamesAndAliases.insert(alias);
      }

      validAliases.push_back(alias);
    }

    // Set aliassses for newColour
    newColour->aliases = validAliases;
  } catch (const std::exception &e) {
    // When no aliasses exist, ignore exception
    std::cout << "No aliasses found.\n";
  }

  pushToVector(newColour);
  defaultCMYK = false;
}

void ColorConfig::pushToVector(CustomColor::Ptr &color) {
  std::vector<std::string> defaultColors = {"C", "M", "Y", "K"};
  for (size_t i = 0; i < defaultColors.size(); i++) {
    if (isColor(color, defaultColors[i])) {
      colors[i] = color;
      return;
    }
  }
  colors.push_back(color);
}

bool ColorConfig::isColor(CustomColor::Ptr &color, std::string name) {
  if (color == nullptr)
    return false;

  boost::algorithm::to_upper(name);

  if (name == color->name)
    return true;

  if (std::find(color->aliases.begin(), color->aliases.end(), name) !=
      color->aliases.end()) {
    return true;
  }

  return false;
}
