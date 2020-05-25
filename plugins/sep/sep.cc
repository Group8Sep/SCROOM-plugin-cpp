#include "sep.hh"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <fstream>
#include <string>
#include <tiffio.h>
#include <iostream>
#include <map>
#include "scroom/transformpresentation.hh"

// #include "tiff.hh"
// #include "../scroom/gui/src/view.hh"
// #include "../scroom/gui/src/loader.hh"

#include "seppresentation.hh"

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////

Sep::Sep()
{
}

Sep::~Sep()
{
}

Sep::Ptr Sep::create()
{
	return Ptr(new Sep());
}

std::string Sep::getPluginName()
{
	return "SEP";
}

std::string Sep::getPluginVersion()
{
	return "0.0";
}

////////////////////////////////////////////////////////////////////////
// PluginInformationInterface
////////////////////////////////////////////////////////////////////////

void Sep::registerCapabilities(ScroomPluginInterface::Ptr host)
{
	host->registerOpenPresentationInterface("SEP viewer", shared_from_this<Sep>());
}

////////////////////////////////////////////////////////////////////////
// OpenPresentationInterface
////////////////////////////////////////////////////////////////////////

std::list<GtkFileFilter *> Sep::getFilters()
{
	std::list<GtkFileFilter *> result;

	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, "SEP files");
	// gtk_file_filter_add_mime_type(filter, "image/tiff"); // i think this has to be text/plain but not sure if that would cause any problems
	gtk_file_filter_add_pattern(filter, "*.sep");		 // maybe this one is better?
	result.push_back(filter);

	return result;
}

PresentationInterface::Ptr Sep::open(const std::string &fileName)
{
	std::ifstream file(fileName); // open file stream
	std::string str;			  // just a temporary variable

	// specifies whether we're using simple CMYK channels
	// or specced channels i.e. CMYKW+
	bool simple_channels;

	// maps <property> -> <value>
	// where <property> is width, height or channels
	std::map<std::string, std::string> file_content;

	// parse the file into file_content
	int line = 0;
	while (std::getline(file, str))
	{
		// std::cout << file_content[line] << "\n";
		if (line == 0)
		{
			file_content.insert(std::make_pair("width", str));
		}
		else if (line == 1)
		{
			file_content.insert(std::make_pair("height", str));
		}
		else if (line >= 2)
		{
			if (str.compare(4, 1, " ") != 0)
				file_content.insert(std::make_pair(str.substr(0, 1), str.substr(4, std::string::npos)));
			else
				file_content.insert(std::make_pair(str.substr(0, 2), str.substr(5, std::string::npos)));
		}

		line++;
	}

	// if file has 6 lines (width, height, CMYK) , then it a simple cmyk
	simple_channels = file_content.size() == 6;

	auto tiff = TIFFOpen("/home/ubuntu/Documents/swathStack_4pass_Triangles/M_6.tif", "r");

	std::cout << tiff << "\n";
	// if (file_content.size() == 6)
	// 	simple_channels = true;
	// else
	// 	simple_channels = false;

	// auto tiff = Tiff::create();
	// std::cout << typeid(tiff).name() << '\n';

	// PresentationInterface::Ptr result = loadPresentation((std::string) file_content["C"]);
	// return result;
	// std::string layer = fileName.substr(fileName.find_last_of("/\\"));

	// const char *symlinkpath = file_content["C"].c_str();
	// char actualpath[PATH_MAX + 1];
	// char *ptr;

	// ptr = realpath(symlinkpath, actualpath);
	// std::cout << fileName << std::endl;

	// auto result = tiff->open("/home/ubuntu/Documents/swathStack_4pass_Triangles/" + file_content["C"]);
	// std::cout << result << std::endl;

	return nullptr;
}
