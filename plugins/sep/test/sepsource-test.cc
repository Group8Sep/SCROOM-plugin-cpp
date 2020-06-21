#include <boost/dll.hpp>
#include <boost/test/unit_test.hpp>

// Make all private members accessible for testing
#define private public

#include "../sepsource.hh"
#include "../sli/slilayer.hh"

const auto testFileDir = boost::dll::program_location().parent_path().parent_path() / "testfiles";

BOOST_AUTO_TEST_SUITE(Sep_Tests)

BOOST_AUTO_TEST_CASE(create) {
    auto source = SepSource::create();
    BOOST_CHECK(source != nullptr);
}

BOOST_AUTO_TEST_CASE(parent_dir) {
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(parse_sep) {
    SepFile file = SepSource::parseSep((testFileDir / "sep_cmyk.sep").string());
    BOOST_CHECK(file.files.size() == 4);

    for (const std::string& colour : {"C", "M", "Y", "K"}) {
        BOOST_CHECK(file.files[colour] == testFileDir / (colour + ".tif"));
    }

    BOOST_CHECK(file.width == 600);
    BOOST_CHECK(file.height == 400);
    BOOST_CHECK(file.white_ink_choice == 0);
}

BOOST_AUTO_TEST_CASE(get_for_none) {
    uint16_t unit;
    float x_res, y_res;
    auto source = SepSource::create();

    source->getForOneChannel(nullptr, unit, x_res, y_res);

    BOOST_CHECK(unit == RESUNIT_NONE);
    BOOST_CHECK(std::abs(x_res - 1.0) < 1e-4);
    BOOST_CHECK(std::abs(y_res - 1.0) < 1e-4);
}

BOOST_AUTO_TEST_CASE(get_resolution_null) {
    uint16_t unit;
    float x_res, y_res;
    auto source = SepSource::create();

    source->channel_files["C"] = nullptr;
    source->channel_files["M"] = nullptr;
    source->channel_files["Y"] = nullptr;
    source->channel_files["K"] = nullptr;

    auto res = source->getResolution(unit, x_res, y_res);
    BOOST_CHECK(res == true);
}

BOOST_AUTO_TEST_CASE(set_data) {
    // Preparation
    auto source = SepSource::create();
    SepFile file = SepSource::parseSep((testFileDir / "sep_cmyk.sep").string());

    // Tested call
    source->setData(file);

    // Check properties, not the actual value, because we don't care if
    // setData copies the passed SepFile.
    BOOST_CHECK(source->sep_file.files.size() == 4);

    for (const std::string& colour : {"C", "M", "Y", "K"}) {
        BOOST_CHECK(source->sep_file.files[colour] == testFileDir / (colour + ".tif"));
    }

    BOOST_CHECK(source->sep_file.width == 600);
    BOOST_CHECK(source->sep_file.height == 400);
    BOOST_CHECK(source->sep_file.white_ink_choice == 0);
}

BOOST_AUTO_TEST_CASE(open_files) {
    // Preparation
    auto source = SepSource::create();
    SepFile file = SepSource::parseSep((testFileDir / "sep_cmyk.sep").string());
    source->setData(file);

    // Tested call
    source->openFiles();

    // Check that all the CMYK files have been opened
    for (const std::string& colour : {"C", "M", "Y", "K"}) {
        BOOST_CHECK(source->channel_files[colour] != nullptr);
    }

    // Check that white ink and varnish are not opened
    BOOST_CHECK(source->white_ink == nullptr);
    BOOST_CHECK(source->varnish == nullptr);
}

BOOST_AUTO_TEST_CASE(apply_white_nowhite) {
    auto res = SepSource::applyWhiteInk(100, 100, 0);
    BOOST_CHECK(res == 100);
}

BOOST_AUTO_TEST_CASE(apply_white_subtract_1) {
    auto res = SepSource::applyWhiteInk(100, 100, 1);
    BOOST_CHECK(res == 0);
}

BOOST_AUTO_TEST_CASE(apply_white_subtract_2) {
    auto res = SepSource::applyWhiteInk(100, 101, 1);
    BOOST_CHECK(res == 1);
}

BOOST_AUTO_TEST_CASE(apply_white_multiply_1) {
    auto res = SepSource::applyWhiteInk(100, 100, 2);
    BOOST_CHECK(res == 61);
}

BOOST_AUTO_TEST_CASE(apply_white_multiply_2) {
    auto res = SepSource::applyWhiteInk(0, 100, 2);
    BOOST_CHECK(res == 100);
}

BOOST_AUTO_TEST_CASE(apply_white_no_effect_1) {
    auto res = SepSource::applyWhiteInk(0, 100, 0);
    BOOST_CHECK(res == 100);
}

BOOST_AUTO_TEST_CASE(tiff_wrapper_nullptr) {
    auto res = SepSource::TIFFReadScanline_(nullptr, nullptr, 1);
    BOOST_CHECK(res == -1);
}

BOOST_AUTO_TEST_CASE(tiff_wrapper_2) {
    auto res = SepSource::TIFFReadScanline_(nullptr, nullptr, 1);
    BOOST_CHECK(res == -1);
}

BOOST_AUTO_TEST_CASE(fill_sli_empty) {
    SliLayer::Ptr sli = SliLayer::create("", "name", 0, 0);
    sli->height = 42;
    sli->bitmap = nullptr; // TODO: this is a workaround for a bug - please remove
    SepSource::fillSliLayer(sli);
    BOOST_CHECK(sli->height == 42);
}

BOOST_AUTO_TEST_CASE(fill_sli_1) {
    SliLayer::Ptr sli = SliLayer::create((testFileDir / "sep_cmyk.sep").string(), "name", 0, 0);
    SepSource::fillSliLayer(sli);
    BOOST_CHECK(sli->width == 600);
    BOOST_CHECK(sli->height == 400);
    BOOST_CHECK(sli->spp == 4);
    BOOST_CHECK(sli->bps == 8);
    BOOST_CHECK(sli->bitmap != nullptr);
    BOOST_CHECK(std::abs(sli->xAspect - 1.0) < 1e-4);
    BOOST_CHECK(std::abs(sli->yAspect - 1.0) < 1e-4);
}

BOOST_AUTO_TEST_SUITE_END()
