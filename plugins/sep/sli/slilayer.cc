#include "slilayer.hh"

SliLayer::Ptr SliLayer::create(const std::string &filepath,
                              const std::string &name, int xoffset, int yoffset)
{
  SliLayer::Ptr layer(new SliLayer());
  layer->filepath = filepath;
  layer->name = name;
  layer->xoffset = xoffset;
  layer->yoffset = yoffset;
  return layer;
}

SliLayer::SliLayer() : height(0), width(0)
{
  visible = true;
}

SliLayer::~SliLayer()
{
  free(bitmap);
}

Scroom::Utils::Rectangle<int> SliLayer::toRectangle()
{
  Scroom::Utils::Rectangle<int> rect {xoffset, yoffset, width, height};
  
  return rect;
}

Scroom::Utils::Rectangle<int> SliLayer::toBytesRectangle()
{
  Scroom::Utils::Rectangle<int> rect {xoffset*(int)spp, yoffset, width*(int)spp, height};
  
  return rect;
}