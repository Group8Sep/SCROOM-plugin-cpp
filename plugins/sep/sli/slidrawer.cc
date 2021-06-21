//
// Created by jelle on 21-06-21.
//

#include <utility>

#include "../colorconfig/CustomColorHelpers.hh"
#include "slidrawer.hh"

// region Shared methods

void SliDrawer::advanceIAndSurfacePointer(
    const Scroom::Utils::Rectangle<int> &layerRect,
    const Scroom::Utils::Rectangle<int> &intersectRect, int layerBound,
    int stride, uint8_t *&surfacePointer, int &i) {
  // Increment the surface pointer and I
  i++;
  surfacePointer++;

  // we are past the image bounds; go to the next next line
  if (i % layerRect.getWidth() == layerBound) {
    surfacePointer += stride - intersectRect.getWidth();
    i += layerRect.getWidth() - intersectRect.getWidth();
  }
}

// endregion

// region CMYK implementation

SliDrawerCMYK::SliDrawerCMYK() {}

void SliDrawerCMYK::drawXoffset(uint8_t *surfacePointer, uint8_t *bitmap,
                                int bitmapStart, int bitmapOffset,
                                Scroom::Utils::Rectangle<int> layerRect,
                                Scroom::Utils::Rectangle<int> intersectRect,
                                int layerBound, int stride) {
  for (int i = bitmapStart; i < bitmapStart + bitmapOffset;) {
    // increment the value of the current surface byte
    // NOTE if it is known that the total value will not exceed 255,
    // the min(...) can be simply replaced by "bitmap[i]"
    // this will save a few thousand/million cpu cycles
    *surfacePointer +=
        std::min(bitmap[i], static_cast<uint8_t>(255 - *surfacePointer));

    // Move the iterator and surface pointer forward
    advanceIAndSurfacePointer(layerRect, intersectRect, layerBound, stride,
                              surfacePointer, i);
  }
}

void SliDrawerCMYK::draw(uint8_t *surfacePointer, uint8_t *bitmap,
                         int bitmapStart, int bitmapOffset) {
  for (int i = bitmapStart; i < bitmapStart + bitmapOffset; i++) {
    // increment the value of the current surface byte
    // NOTE if it is known that the total value will not exceed 255,
    // the min(...) can be simply replaced by "bitmap[i]"
    // this will save a few thousand/million cpu cycles
    *surfacePointer +=
        std::min(bitmap[i], static_cast<uint8_t>(255 - *surfacePointer));

    // go to the next surface byte
    surfacePointer++;
  }
}

// endregion

// region CustomColor implementation

SliDrawerCustomColor::SliDrawerCustomColor(SliLayer::Ptr layerPtr) {
  layer = std::move(layerPtr);
}

void SliDrawerCustomColor::drawXoffset(
    uint8_t *surfacePointer, uint8_t *bitmap, int bitmapStart, int bitmapOffset,
    Scroom::Utils::Rectangle<int> layerRect,
    Scroom::Utils::Rectangle<int> intersectRect, int layerBound, int stride) {
  std::array<uint8_t *, 4> addresses{};
  for (int i = bitmapStart; i < bitmapStart + bitmapOffset;) {
    int k = i;

    addresses[0] =
        surfacePointer; // Store the address, so it can later be written to
    int32_t C = *surfacePointer; // Initialize the CMYK holder values to the
    // current values for their color

    // Advance k and the surface pointer, while keeping the the layer bounds in
    // mind
    advanceIAndSurfacePointer(layerRect, intersectRect, layerBound, stride,
                              surfacePointer, k);

    addresses[1] = surfacePointer; // Store the m address
    int32_t M = *surfacePointer;   // Surface pointer has been advanced, so now
    // the M value can be loaded
    advanceIAndSurfacePointer(layerRect, intersectRect, layerBound, stride,
                              surfacePointer, k);

    addresses[2] = surfacePointer;
    int32_t Y = *surfacePointer;
    advanceIAndSurfacePointer(layerRect, intersectRect, layerBound, stride,
                              surfacePointer, k);

    addresses[3] = surfacePointer;
    int32_t K = *surfacePointer;
    advanceIAndSurfacePointer(layerRect, intersectRect, layerBound, stride,
                              surfacePointer, k);

    for (uint32_t j = 0; j < layer->spp;
         j++) { // Add values to the 32bit cmyk holders
      CustomColorHelpers::calculateCMYK(layer->channels.at(j), C, M, Y, K,
                                        bitmap[i + j]);
    }
    // Write the CMYK values back to the surface, clipped to uint_8

    *(addresses[0]) = CustomColorHelpers::toUint8(C);
    *(addresses[1]) = CustomColorHelpers::toUint8(M);
    *(addresses[2]) = CustomColorHelpers::toUint8(Y);
    *(addresses[3]) = CustomColorHelpers::toUint8(K);
    // set i to the incremented value
    i = k;
  }
}

void SliDrawerCustomColor::draw(uint8_t *surfacePointer, uint8_t *bitmap,
                                int bitmapStart, int bitmapOffset) {
  for (int i = bitmapStart; i < bitmapStart + bitmapOffset;
       i += layer->spp) {        // Iterate over all pixels
    int32_t C = *surfacePointer; // Initialize the CMYK holder values to the
    // current values for their color
    int32_t M = *(surfacePointer + 1);
    int32_t Y = *(surfacePointer + 2);
    int32_t K = *(surfacePointer + 3);
    for (uint32_t j = 0; j < layer->spp;
         j++) { // Add values to the 32bit cmyk holders
      auto &color = layer->channels.at(j);
      CustomColorHelpers::calculateCMYK(color, C, M, Y, K, bitmap[i + j]);
    }
    // Store the CMYK values back into the surface, clipped to uint_8
    *surfacePointer = CustomColorHelpers::toUint8(C);
    *(surfacePointer + 1) = CustomColorHelpers::toUint8(M);
    *(surfacePointer + 2) = CustomColorHelpers::toUint8(Y);
    *(surfacePointer + 3) = CustomColorHelpers::toUint8(K);

    surfacePointer += 4; // Advance the pointer
  }
}

// endregion
