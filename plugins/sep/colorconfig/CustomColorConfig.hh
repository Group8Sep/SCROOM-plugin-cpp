//
// Created by developer on 01-06-21.
//

#pragma once
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>

#include "CustomColor.hh"
#include <scroom/plugininformationinterface.hh>
#include <scroom/utilities.hh>
#include <unordered_set>

namespace pt = boost::property_tree;
class ColorConfig {

private:
  ColorConfig();

private:
  std::vector<CustomColor::Ptr> colors;

public:
  bool defaultCMYK = true;

  static ColorConfig &getInstance() {
    static ColorConfig INSTANCE;
    return INSTANCE;
  }

  std::vector<CustomColor::Ptr> getDefinedColors();

  CustomColor::Ptr getColorByNameOrAlias(const std::string &name);

  void loadFile(std::string file = "colours.json");

  void addNonExistentDefaultColors();

private:
  void parseColor(pt::ptree::value_type &v,
                  std::unordered_set<std::string> &seenNamesAndAliases);

  static bool isColor(CustomColor::Ptr &color, std::string name);
  /**
   * Push the color to the vector
   * Makes sure that, if the color is C, M, Y, K,
   * that it will be placed in the correct of the first four places in the
   * vector
   * @param color
   */
  void pushToVector(CustomColor::Ptr &color);
};
