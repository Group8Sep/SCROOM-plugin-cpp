#pragma once

#include <fstream>

#include <scroom/presentationinterface.hh>
#include <scroom/scroominterface.hh>
#include <scroom/threadpool.hh>

#include "slilayer.hh"
#include "slicontrolpanel.hh"


class SliPresentation : public PresentationBase,
                        public virtual Scroom::Utils::Base,
                        public SliPresentationInterface
{
public:
  typedef boost::shared_ptr<SliPresentation> Ptr;
  typedef boost::weak_ptr<SliPresentation> WeakPtr;

private:
  std::map<std::string, std::string> properties;
  std::set<ViewInterface::WeakPtr> views;

  /** The SliLayers that are part of the presentation */
  std::vector<SliLayer::Ptr> layers;

  /** Reference to the ScroomInterface, needed for showing presentation */
  ScroomInterface::Ptr scroomInterface;

  /** Reference to the associated SliControlPanel */
  SliControlPanel::Ptr controlPanel;

  /** Weak pointer to this. Needed for passing a reference to SliControlPanel */
  static SliPresentationInterface::WeakPtr weakPtrToThis;

  /** Width of all layers combined */
  int total_width = 0;

  /** Height of all layers combined */
  int total_height = 0;

  /** Area of all layers and offsets combined */
  int total_area;

  /** Area of all layers and offsets combined in bytes*/
  int total_area_bytes;

  int bpp;
  int Xresolution;
  int Yresolution;

  /** 
   * Contains the cached bitmaps for the different zoom levels. 
   * The zoom level is the key, the pointer to the bitmap the value.
   */
  std::map<int, uint8_t*> rgbCache;

  ThreadPool::Queue::Ptr threadQueue;

  /** Must be acquired by a thread before writing to the cached bitmaps */
  boost::recursive_mutex cachingPendingMtx;

private:
  /** Constructor */
  SliPresentation(ScroomInterface::Ptr scroomInterface);

  /**
   * Computes the RGB bitmap of the bottommost layer (zoom=0) without any reductions
   */
  virtual void cacheBottomZoomLevelRgb();

  /**
   * Computes the RGB bitmap for the zoom level from the zoom level bitmap below it, 
   * reducing it in the process. Reductions must only happen if zoom < 0
   */
  virtual void cacheZoomLevelRgb(int zoom);

public:
  /** Destructor */
  virtual ~SliPresentation();

  /** Constructor */
  static Ptr create(ScroomInterface::Ptr scroomInterface);

  /** 
   * Load the SLI file and instruct the Scroom core to display it
   * @param fileName the absolute path of the .sli file to be opened
   */
  virtual bool load(const std::string& fileName);

  /** 
   * Parse the SLI file and add all its information to the corresponding 
   * variables of the class
   * @param fileName the absolute path of the .sli file to be parsed
   */
  virtual void parseSli(const std::string &fileName);

  /** Compute the overall width and height of the SLi file (over all layers) */
  virtual void computeHeightWidth();

  /** Getter for the layers that the presentation consists of */
  std::vector<SliLayer::Ptr>& getLayers() {return layers;};

  ////////////////////////////////////////////////////////////////////////
  // SliPresentationInterface
  ////////////////////////////////////////////////////////////////////////

  /** Wipe the zoom level RGB cache of the presentation. Needed when layers are enabled or disabled. */
  virtual void wipeCache();

  /** Causes the complete canvas to be redrawn */
  virtual void triggerRedraw();

  ////////////////////////////////////////////////////////////////////////
  // PresentationInterface
  ////////////////////////////////////////////////////////////////////////

  virtual Scroom::Utils::Rectangle<double> getRect();
  virtual void redraw(ViewInterface::Ptr const& vi, cairo_t* cr, Scroom::Utils::Rectangle<double> presentationArea, int zoom);
  virtual bool getProperty(const std::string& name, std::string& value);
  virtual bool isPropertyDefined(const std::string& name);
  virtual std::string getTitle();

  ////////////////////////////////////////////////////////////////////////
  // PresentationBase
  ////////////////////////////////////////////////////////////////////////

  virtual void viewAdded(ViewInterface::WeakPtr viewInterface);
  virtual void viewRemoved(ViewInterface::WeakPtr vi);
  virtual std::set<ViewInterface::WeakPtr> getViews();
};
