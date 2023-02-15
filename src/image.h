// Copyright 2021, Aline Normoyle, alinen

/**
* This file contains the class and class method declarations for Image and Pixel.
*
* @author: Isaac Wasserman
* @version: February 2, 2023
*/

#ifndef AGL_IMAGE_H_
#define AGL_IMAGE_H_

#include <iostream>
#include <string>

namespace agl {

/**
 * @brief Holder for a RGB color
 *
 */
class Pixel {
 public:
  Pixel();
  Pixel(unsigned char r, unsigned char g, unsigned char b);
  Pixel operator=(const Pixel& other);
  bool operator==(const Pixel& other);
  Pixel operator*(const Pixel& other);
  Pixel operator*(const float& operand) const;
  Pixel operator/(const float& operand);
  Pixel operator+(const Pixel& other);
  int *toArray();

  unsigned char r;
  unsigned char g;
  unsigned char b;
};

/**
 * @brief Implements loading, modifying, and saving RGB images
 */
class Image {
 public:
  Image();
  Image(int width, int height);
  Image(const Image& orig);
  Image& operator=(const Image& orig);

  virtual ~Image();

  /**
   * @brief Load the given filename
   * @param filename The file to load, relative to the running directory
   * @param flip Whether the file should flipped vertally when loaded
   *
   * @verbinclude sprites.cpp
   */
  bool load(const std::string& filename, bool flip = false);

  /**
   * @brief Save the image to the given filename (.png)
   * @param filename The file to load, relative to the running directory
   * @param flip Whether the file should flipped vertally before being saved
   */
  bool save(const std::string& filename, bool flip =  false) const;

  /** @brief Return the image width in pixels
   */
  int width() const;

  /** @brief Return the image height in pixels
   */
  int height() const;

  /**
   * @brief Return the RGB data
   *
   * Data will have size width * height * 4 (RGB)
   */
  unsigned char* data() const;

  /**
   * @brief Replace image RGB data
   * @param width The new image width
   * @param height The new image height
   *
   * This call will replace the old data with the new data. Data should
   * match the size width * height * 3
   */
  void set(int width, int height, unsigned char* data);

  /**
   * @brief Get the pixel at index (row, col)
   * @param row The row (value between 0 and height)
   * @param col The col (value between 0 and width)
   *
   * Pixel colors are unsigned char, e.g. in range 0 to 255
   */
  Pixel get(int row, int col) const;

  /**
   * @brief Set the pixel RGBA color at index (row, col)
   * @param row The row (value between 0 and height)
   * @param col The col (value between 0 and width)
   *
   * Pixel colors are unsigned char, e.g. in range 0 to 255
   */

  Pixel get_rel(float yPercent, float xPercent,
                       std::string method) const;

  void set(int row, int col, const Pixel& color);

  /**
   * @brief Set the pixel RGB color at index i
   * @param i The index (value between 0 and width * height)
   *
   * Pixel colors are unsigned char, e.g. in range 0 to 255
   */
  Pixel get(int i) const;

  /**
   * @brief Set the pixel RGB color at index i
   * @param i The index (value between 0 and width * height)
   *
   * Pixel colors are unsigned char, e.g. in range 0 to 255
   */
  void set(int i, const Pixel& c);

  // resize the image
  Image resize(int width, int height) const;

  // flip around the horizontal midline
  Image flipHorizontal() const;

  // flip around the vertical midline
  Image flipVertical() const;

  // rotate the Image 90 degrees
  Image rotate90() const;

  // Return a sub-Image having the given top,left coordinate and (width, height)
  Image subimage(int x, int y, int w, int h) const;

  // Replace the portion starting at (row, col) with the given image
  // Clamps the image if it doesn't fit on this image
  void replace(const Image& image, int x, int y);

  // swirl the colors
  Image swirl() const;

  // Apply the following calculation to the pixels in
  // our image and the given image:
  //    result.pixel = this.pixel + other.pixel
  // Assumes that the two images are the same size
  Image add(const Image& other) const;

  // Apply the following calculation to the pixels in
  // our image and the given image:
  //    result.pixel = this.pixel - other.pixel
  // Assumes that the two images are the same size
  Image subtract(const Image& other) const;

  // Apply the following calculation to the pixels in
  // our image and the given image:
  //    result.pixel = this.pixel * other.pixel
  // Assumes that the two images are the same size
  Image multiply(const Image& other) const;

  // Apply the following calculation to the pixels in
  // our image and the given image:
  //    result.pixel = abs(this.pixel - other.pixel)
  // Assumes that the two images are the same size
  Image difference(const Image& other) const;

  // Apply the following calculation to the pixels in
  // our image and the given image:
  //    result.pixel = max(this.pixel, other.pixel)
  // Assumes that the two images are the same size
  Image lightest(const Image& other) const;

  // Apply the following calculation to the pixels in
  // our image and the given image:
  //    result.pixel = min(this.pixel, other.pixel)
  // Assumes that the two images are the same size
  Image darkest(const Image& other) const;

  // Apply gamma correction
  Image gammaCorrect(float gamma) const;

  // Apply the following calculation to the pixels in
  // our image and the given image:
  //    this.pixels = this.pixels * (1-alpha) + other.pixel * alpha
  // Assumes that the two images are the same size
  Image alphaBlend(const Image& other, float amount) const;

  // Convert the image to grayscale
  Image invert() const;

  // Convert the image to grayscale
  Image grayscale() const;

  // return a bitmap version of this image
  Image colorJitter(int size) const;

  // return a bitmap version of this image
  Image bitmap(int size) const;

  // Fill this image with a color
  void fill(const Pixel& c);

  // Displace the individual color channels of the image
  Image channelShift(int rShift[2], int gShift[2], int bShift[2]) const;

  // Emulate a halftone print with channel shifted dots
  Image halftone(int rShift[2], int gShift[2], int bShift[2]) const;

  // Replace all pixels with the given color within the given tolerance
  Image colorReplace(const Pixel& oldColor, const Pixel& newColor, int tolerance) const;

  void convolve(float *kernel, int kSize, float *out) const;

  // Apply sobel filtering to the image
  Image sobel() const;

  // Apply a gaussian blur to the image
  Image gaussianBlur(float sigma) const;

  // Gives every black pixel the color of the nearest non-black pixel
  Image expandOutlines(int iterations) const;

  // Replace all pixels with the given hue within the given tolerance
  Image hueReplace(const Pixel& hue, const Pixel& newColor, int tolerance) const;

 private:
  // todo
  unsigned char* mData = NULL;
  int mWidth;
  int mHeight;
  int mChannels;
};
}  // namespace agl
#endif  // AGL_IMAGE_H_
