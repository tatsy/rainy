#include "gtest/gtest.h"
#include "../include/spica.h"
using namespace spica;

#include <string>

namespace {

    Random rng = Random();
    const std::string filepath = kTempDirectory + "test_image.bmp";
    const std::string hdrpath  = kTempDirectory + "test_hdr.hdr";

}

class ImageTest : public ::testing::Test {
protected:
    const int width;
    const int height;

protected:
    ImageTest() 
        : width(320)
        , height(240) {
    }

    ~ImageTest() {}

    void SetUp() {
        path::createDirectory(kTempDirectory);
    }

    void randomImage(Image* rand) {
        rand->resize(width, height);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                rand->pixel(x, y) = Color(rng.nextReal(), rng.nextReal(), rng.nextReal());
            }
        }
    }
};

// --------------------------------------------------
// Image 
// --------------------------------------------------

TEST_F(ImageTest, DefaultInstance) {
    Image image;
    EXPECT_EQ(0, image.width());
    EXPECT_EQ(0, image.height());
    ASSERT_DEATH(image(0, 0), "");
    ASSERT_DEATH(Image(-1, -1), "");
}

TEST_F(ImageTest, InstanceWithSize) {
    Image image(width, height);
    EXPECT_EQ(320, image.width());
    EXPECT_EQ(240, image.height());
    for (int y = 0; y < image.height(); y++) {
        for (int x = 0; x < image.width(); x++) {
            EXPECT_EQ(0.0, image(x, y).red());
            EXPECT_EQ(0.0, image(x, y).green());
            EXPECT_EQ(0.0, image(x, y).blue());
        }
    }
}

TEST_F(ImageTest, CopyAndAssign) {
    Image rand;
    randomImage(&rand);

    Image image(rand);
    EXPECT_EQ(rand.width(), image.width());
    EXPECT_EQ(rand.height(), image.height());
    for (int y = 0; y < image.height(); y++) {
        for (int x = 0; x < image.width(); x++) {
            EXPECT_EQ(rand(x, y).red(), image(x, y).red());
            EXPECT_EQ(rand(x, y).green(), image(x, y).green());
            EXPECT_EQ(rand(x, y).blue(), image(x, y).blue());
        }
    }
}

TEST_F(ImageTest, Move) {
    Image rand;
    randomImage(&rand);

    Image copy(rand);
    Image image(std::move(rand));
    EXPECT_EQ(width, image.width());
    EXPECT_EQ(height, image.height());
    EXPECT_EQ(0, rand.width());
    EXPECT_EQ(0, rand.height());
    ASSERT_DEATH(rand(0, 0), "");
    for (int y = 0; y < image.height(); y++) {
        for (int x = 0; x < image.width(); x++) {
            EXPECT_EQ(copy(x, y).red(), image(x, y).red());
            EXPECT_EQ(copy(x, y).green(), image(x, y).green());
            EXPECT_EQ(copy(x, y).blue(), image(x, y).blue());
        }
    }
}

TEST_F(ImageTest, InvalidPathToLoad) {
    Image image;
    ASSERT_DEATH(image.load("dammy_path.bmp"), "");
    ASSERT_DEATH(image.load("image.jpg"), "unknown file extension");
}

TEST_F(ImageTest, Resize) {
    Image image;
    image.resize(width, height);
    EXPECT_EQ(width, image.width());
    EXPECT_EQ(height, image.height());
}

TEST_F(ImageTest, Fill) {
    Image image(width, height);
    image.fill(Color::BLUE);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            EXPECT_EQ(Color::BLUE.red(), image(x, y).red());
            EXPECT_EQ(Color::BLUE.green(), image(x, y).green());
            EXPECT_EQ(Color::BLUE.blue(), image(x, y).blue());
        }
    }
}

TEST_F(ImageTest, SaveAndLoad) {
    Image image(width, height);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            image.pixel(x, y) = Color(rng.nextReal(), rng.nextReal(), rng.nextReal());
        }
    }
    image.save(filepath);

    Image loaded;
    loaded.load(filepath);
    EXPECT_EQ(image.width(), loaded.width());
    EXPECT_EQ(image.height(), loaded.height());
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            EXPECT_NEAR(image(x, y).red(),   loaded(x, y).red(),   0.01) << "Red is different";        
            EXPECT_NEAR(image(x, y).green(), loaded(x, y).green(), 0.01) << "Green is different";        
            EXPECT_NEAR(image(x, y).blue(),  loaded(x, y).blue(),  0.01) << "Blue is different";        
        }
    }

    image.save(hdrpath);
    loaded.load(hdrpath);
    EXPECT_EQ(image.width(), loaded.width());
    EXPECT_EQ(image.height(), loaded.height());
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            EXPECT_NEAR(image(x, y).red(),   loaded(x, y).red(),   0.01) << "Red is different";        
            EXPECT_NEAR(image(x, y).green(), loaded(x, y).green(), 0.01) << "Green is different";        
            EXPECT_NEAR(image(x, y).blue(),  loaded(x, y).blue(),  0.01) << "Blue is different";        
        }
    }
}

TEST_F(ImageTest, Tonemap) {
    Image image;
    image.load(kDataDirectory + "gold_room.hdr");
    EXPECT_NO_FATAL_FAILURE(image.tonemap());
    image.save(kTempDirectory + "gold_room.png");
}
