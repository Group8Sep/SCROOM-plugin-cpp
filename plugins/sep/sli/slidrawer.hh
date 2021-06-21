//
// Created by jelle on 21-06-21.
//
#pragma once

#include "slilayer.hh"
#include <cstdint>
#include <scroom/rectangle.hh>

/**
 * SliDrawer is a interface which can be implemented to realize
 * different calculations to draw an image. An implementation will be injected
 * into the SliSource on creation following the Dependency Injection design
 * pattern There are two important methods, draw and drawXoffset.
 * The draw methods will take some input from the bitmap pointer and calculate
 * relevant CMYK values, which they will then write to the target surface.
 * This will later be converted to ARGB before being shown to the user
 */
class SliDrawer {
public:
  using Ptr = boost::shared_ptr<SliDrawer>;
  /**
   * Calculate CMYK for the surface from the bitmap source while there is an X
   * offeset in the Sli Layer
   * @param surfacePointer Pointer to to target surface
   * @param bitmap Pointer to the source surface
   * @param bitmapStart Start location of the bitmap
   * @param bitmapOffset Bitmap offset from start
   * @param layerRect Rectangle of the layer
   * @param intersectRect Rectangle of the intersection
   * @param layerBound Boundary of the layer
   * @param stride Stride over the layer
   */
  virtual void drawXoffset(uint8_t *surfacePointer, uint8_t *bitmap,
                           int bitmapStart, int bitmapOffset,
                           Scroom::Utils::Rectangle<int> layerRect,
                           Scroom::Utils::Rectangle<int> intersectRect,
                           int layerBound, int stride) = 0;

  /**
   * Calculate CMYK for the surface from the bitmap source
   * @param surfacePointer Target surface
   * @param bitmap Pointer to the source surface
   * @param bitmapStart Start location of the bitmap
   * @param bitmapOffset Bitmap offset from start
   */
  virtual void draw(uint8_t *surfacePointer, uint8_t *bitmap, int bitmapStart,
                    int bitmapOffset) = 0;

protected:
  /**
   * Default destructor
   */
  virtual ~SliDrawer() = default;

  /**
   * Advance the surface pointer and the iterator int.
   * If we iterated beyond the image bounds, go to the next line
   * @param layerRect Rectangle of the layer
   * @param intersectRect Rectangle of the intersection
   * @param layerBound Boundary of the layer
   * @param stride Stride int over the surface
   * @param surfacePointer Pointer to the target surface
   * @param i Iterator integer
   */
  static void
  advanceIAndSurfacePointer(const Scroom::Utils::Rectangle<int> &layerRect,
                            const Scroom::Utils::Rectangle<int> &intersectRect,
                            int layerBound, int stride,
                            uint8_t *&surfacePointer, int &i);
};

/**
 * Fast CMYK implementation
 * Has some incorrect White ink interactions
 */
class SliDrawerCMYK : public SliDrawer {
private:
public:
  static SliDrawer::Ptr create() { return SliDrawer::Ptr(new SliDrawerCMYK()); }
  SliDrawerCMYK();

  void drawXoffset(uint8_t *surfacePointer, uint8_t *bitmap, int bitmapStart,
                   int bitmapOffset, Scroom::Utils::Rectangle<int> layerRect,
                   Scroom::Utils::Rectangle<int> intersectRect, int layerBound,
                   int stride) override;

  void draw(uint8_t *surfacePointer, uint8_t *bitmap, int bitmapStart,
            int bitmapOffset) override;
};

/**
 * Slower extensible Custom Color implementation
 * Could possibly be sped up
 */
class SliDrawerCustomColor : public SliDrawer {
private:
  SliLayer::Ptr layer;

public:
  static SliDrawer::Ptr create(const SliLayer::Ptr &layerPtr) {
    return SliDrawer::Ptr(new SliDrawerCustomColor(layerPtr));
  }
  explicit SliDrawerCustomColor(SliLayer::Ptr layerPtr);

  void drawXoffset(uint8_t *surfacePointer, uint8_t *bitmap, int bitmapStart,
                   int bitmapOffset, Scroom::Utils::Rectangle<int> layerRect,
                   Scroom::Utils::Rectangle<int> intersectRect, int layerBound,
                   int stride) override;

  void draw(uint8_t *surfacePointer, uint8_t *bitmap, int bitmapStart,
            int bitmapOffset) override;
};