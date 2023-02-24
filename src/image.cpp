// Copyright 2021, Aline Normoyle, alinen

/**
* This file contains the method definitions for Image and Pixel classes.
*
* @author: Isaac Wasserman
* @version: February 2, 2023
*/

#include "image.h"

#include <cassert>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <cstring>
#include <string>
#include <cmath>
#include <time.h>

namespace agl {

/**
* @brief Instantiate a new Pixel object with all channels set to 0
*/
Pixel::Pixel(){
  this->r = 0;
  this->g = 0;
  this->b = 0;
}

/**
* @brief Instantiate a new Pixel object
* @param r Red channel value [0, 255]
* @param g Green channel value [0, 255]
* @param b Blue channel value [0, 255]
*/
Pixel::Pixel(unsigned char r, unsigned char g, unsigned char b){
  this->r = r;
  this->g = g;
  this->b = b;
}

/**
* @brief Copy the values of another Pixel object into this one
* @param other The Pixel object to copy
* @return A reference to this Pixel object
*/
Pixel Pixel::operator=(const Pixel& other){
  this->r = other.r;
  this->g = other.g;
  this->b = other.b;
  return *this;
}

/**
 * @brief Compare two Pixel objects for equality
 * @param other The Pixel object to compare to
 * @return True if the two Pixel objects have the same values for all channels
 */
bool Pixel::operator==(const Pixel& other){
  return this->r == other.r && this->g == other.g && this->b == other.b;
}

/**
 * @brief Multiply two Pixel objects together using dot product
 * @param other The Pixel object to multiply by
 * @return A new Pixel object with the values of the two multiplied together (r1 * r2, g1 * g2, b1 * b2)
 */
Pixel Pixel::operator*(const Pixel& other){
  return Pixel(this->r * other.r, this->g * other.g, this->b * other.b);
}

/**
 * @brief Multiply a Pixel object by a scalar value
 * @param operand scalar value to multiply by
 * @return Pixel object with each channel multiplied by the scalar value
 */
Pixel Pixel::operator*(const float& operand) const{
  return Pixel(std::round(this->r * operand), std::round(this->g * operand), std::round(this->b * operand));
}

/**
 * @brief Divide a Pixel object by a scalar value
 * @param operand scalar value to divide by
 * @return Pixel object with each channel divided by the scalar value
 */
Pixel Pixel::operator/(const float& operand){
  return Pixel(std::round(this->r / operand), std::round(this->g / operand), std::round(this->b / operand));
}

/**
 * @brief Add two Pixel objects together
 * @param other The Pixel object to add
 * @return A new Pixel object with the values of the two added together (r1 + r2, g1 + g2, b1 + b2)
 */
Pixel Pixel::operator+(const Pixel& other){
  return Pixel(std::min(255, this->r + other.r), std::min(255, this->g + other.g), std::min(255, this->b + other.b));
}

/**
 * @brief Subtract two Pixel objects
 * @param other The Pixel object to subtract
 * @return A new Pixel object with the values of the two subtracted (r1 - r2, g1 - g2, b1 - b2)
 */
Pixel Pixel::operator-(const Pixel& other){
  return Pixel(std::max(0, this->r - other.r), std::max(0, this->g - other.g), std::max(0, this->b - other.b));
}

/**
 * @brief Stream a Pixel object to an output stream
 * @param stream The output stream to write to
 * @param p The Pixel object to write
 * @return stream
 */
std::ostream& operator<<(std::ostream& stream, const Pixel& p) {
  stream << "(" << (int) p.r << ", " << (int) p.g << ", " << (int) p.b << ")";
  return stream;
}

/**
 * @brief Construct an empty Image object
 */
Image::Image() {}

/**
 * @brief Construct a new Image object
 * @param width The width of the image in pixels
 * @param height The height of the image in pixels
 */
Image::Image(int width, int height) {
  mWidth = width;
  mHeight = height;
  mChannels = 3;
  if(mData != NULL){
    delete[] mData;
  }
  mData = new unsigned char[width * height * mChannels];
  memset(mData, (unsigned char) 0, width * height * mChannels);
}

/**
 * @brief Construct a new Image object by copying another Image object
 * @param orig The Image object to copy
 */
Image::Image(const Image& orig) {
  mWidth = orig.mWidth;
  mHeight = orig.mHeight;
  mChannels = orig.mChannels;
  if(mData != NULL){
    delete[] mData;
  }
  mData = new unsigned char[mWidth * mHeight * mChannels];
  memcpy(mData, orig.data(), mWidth * mHeight * mChannels);
}

/**
 * @brief Copy the values of another Image object into this one
 * @param orig The Image object to copy
 * @return A reference to this Image object
 */
Image& Image::operator=(const Image& orig) {
  if (this != &orig) {
    mWidth = orig.mWidth;
    mHeight = orig.mHeight;
    mChannels = orig.mChannels;
    if(mData != NULL){
      delete[] mData;
    }
    mData = new unsigned char[mWidth * mHeight * mChannels];
    memcpy(mData, orig.data(), mWidth * mHeight * mChannels);
  }
  return *this;
}

/**
 * @brief Destruct the Image object
 */
Image::~Image() {
  if (mData != NULL) {
    delete[] mData;
  }
}

/**
 * @brief Get the width of the image in pixels
 * @return The width of the image in pixels
 */
int Image::width() const { return mWidth; }

/**
 * @brief Get the height of the image in pixels
 * @return The height of the image in pixels
 */
int Image::height() const { return mHeight; }

/**
 * @brief Get the image data as an array of unsigned chars
 * @return The image data as an array of unsigned chars
 */
unsigned char* Image::data() const { return mData; }

/**
 * @brief Replace the image data with new data
 * @param width 
 * @param height 
 * @param data 
 */
void Image::set(int width, int height, unsigned char* data) {
  mWidth = width;
  mHeight = height;
  if(mData != NULL){
    delete[] mData;
  }
  mData = data;
}

/**
 * @brief Load an image from a file
 * @param filename Path to source file
 * @param flip Whether to flip the image vertically
 * @return true if the image was loaded successfully, false otherwise
 */
bool Image::load(const std::string& filename, bool flip) {
  int width, height, channels;
  stbi_set_flip_vertically_on_load(flip);
  if(mData != NULL){
    delete[] mData;
  }
  mData = stbi_load(filename.c_str(), &width, &height, &channels, 3);
  mWidth = width;
  mHeight = height;
  mChannels = channels;
  if (mData != NULL) {
    return true;
  } else {
    return false;
  }
}

/**
 * @brief Save the image to a file
 * @param filename Path to destination file
 * @param flip Whether to flip the image vertically
 * @return true if the image was saved successfully, false otherwise
 */
bool Image::save(const std::string& filename, bool flip) const {
  std::string ext = filename.substr(filename.find_last_of(".") + 1);
  stbi_flip_vertically_on_write(flip);
  for (int i = 0; i < ext.length(); i++) {
    ext[i] = std::tolower(ext[i]);
  }
  if (ext == "png"){
    return stbi_write_png(filename.c_str(), mWidth, mHeight, mChannels, mData, mWidth * mChannels);
  }
  else if(ext == "jpg" || ext == "jpeg"){
    return stbi_write_jpg(filename.c_str(), mWidth, mHeight, mChannels, mData, 90);
  }
  else if(ext == "bmp") {
    return stbi_write_bmp(filename.c_str(), mWidth, mHeight, mChannels, mData);
  }
  else if(ext == "tga") {
    return stbi_write_tga(filename.c_str(), mWidth, mHeight, mChannels, mData);
  }
  else if(ext == "hdr") {
    return stbi_write_hdr(filename.c_str(), mWidth, mHeight, mChannels, (float *) mData);
  }
  else {
    std::cerr << "Error: " << ext << " is not a valid file type." << std::endl;
    return false;
  }
}

/**
 * @brief Get a pixel at a given row and column
 * @param row The row of the pixel
 * @param col The column of the pixel
 * @return The pixel at the given row and column
 */
Pixel Image::get(int row, int col) const {
  int i = (row * mWidth + col) * 3;
  if(i < 0 || i >= mWidth * mHeight * 3){
    return Pixel{0, 0, 0};
  }
  return Pixel{mData[i], mData[i + 1], mData[i + 2]};
}

/**
 * @brief Get a pixel at a given position represented by a percent from the top left corner
 * @param yPercent The percent from the top of the image [0, 1]
 * @param xPercent The percent from the left of the image [0, 1]
 * @param method The method to use for sampling ("nearest" or "bilinear")
 */
Pixel Image::get_rel(float yPercent, float xPercent, std::string method) const {
  if(method == "nearest"){
    int row = (int) std::lround(yPercent * mHeight);
    int col = (int) std::lround(xPercent * mWidth);
    return get(row, col);
  }
  else if(method == "bilinear"){
    float y = yPercent * mHeight;
    float x = xPercent * mWidth;

    float x1 = std::floor(x);
    float x2 = std::ceil(x);
    float y1 = std::floor(y);
    float y2 = std::ceil(y);

    Pixel Q11 = get(y1, x1);
    Pixel Q12 = get(y1, x2);
    Pixel Q21 = get(y2, x1);
    Pixel Q22 = get(y2, x2);

    Pixel P1;
    Pixel P2;
    if(x1 == x2){
      P1 = Q11;
      P2 = Q21;
    }
    else {
      P1 = (Q11 * (x2 - x)) + (Q12 * (x - x1));
      P2 = (Q21 * (x2 - x)) + (Q22 * (x - x1));
    }

    Pixel P;
    if(y1 == y2){
      P = P1;
    }
    else {
      P = (P1 * (y2 - y)) + (P2 * (y - y1));
    }

    return P;
  }
  else {
    std::cerr << "Error: \"" << method << "\" is not a sampling method" << std::endl;
    return Pixel{0, 0, 0};
  }
}

/**
 * @brief Set a pixel at a given row and column
 * @param row The row of the pixel
 * @param col The column of the pixel
 * @param color The color to set the pixel to
 */
void Image::set(int row, int col, const Pixel& color) {
  int i = (row * mWidth + col) * 3;
  mData[i] = color.r;
  mData[i + 1] = color.g;
  mData[i + 2] = color.b;
}

/**
 * @brief Get the ith pixel from the top left corner of the image
 * @param i 
 * @return Pixel 
 */
Pixel Image::get(int i) const {
  return get(i / mWidth, i % mWidth);
}

/**
 * @brief Set the ith pixel from the top left corner of the image
 * @param i 
 * @param c 
 */
void Image::set(int i, const Pixel& c) {
  set(i / mWidth, i % mWidth, c);
}

/**
 * @brief Resize the image to the given width and height using bilinear interpolation
 * @param w 
 * @param h 
 * @return Image 
 */
Image Image::resize(int w, int h) const {
  Image result(w, h);
  for(int row = 0; row < h; row++){
    for(int col = 0; col < w; col++){
      Pixel p = get_rel((float) row / h, (float) col / w, "bilinear");
      result.set(row, col, p);
    }
  }
  return result;
}

/**
 * @brief Flip the image horizontally
 * @return Image 
 */
Image Image::flipHorizontal() const {
  Image result(mWidth, mHeight);
  for(int row = 0; row < mHeight; row++){
    for(int col = 0; col < mWidth; col++){
      result.set(row, col, get(row, mWidth - col - 1));
    }
  }
  return result;
}

/**
 * @brief Flip the image vertically
 * @return Image 
 */
Image Image::flipVertical() const {
  Image result(mWidth, mHeight);
  for(int col = 0; col < mWidth; col++){
    for(int row = 0; row < mHeight; row++){
      result.set(row, col, get(mHeight - row - 1, col));
    }
  }
  return result;
}

// Part 2: Operator 1
/**
 * @brief Rotate the image 90 degrees clockwise
 * @return Image 
 */
Image Image::rotate90() const {
  Image result(mHeight, mWidth);
  for(int row = 0; row < mHeight; row++){
    for(int col = 0; col < mWidth; col++){
      result.set(col, mHeight - row - 1, get(row, col));
    }
  }
  return result;
}

/**
 * @brief Extract a subimage from the image starting at the given position and with the given width and height
 * @param startx 
 * @param starty 
 * @param w 
 * @param h 
 * @return Image 
 */
Image Image::subimage(int startx, int starty, int w, int h) const {
  Image sub(w, h);
  for(int row = starty; row < starty + h; row++){
    for(int col = startx; col < startx + w; col++){
      sub.set(row - starty, col - startx, get(row, col));
    }
  }
  return sub;
}

/**
 * @brief Replace a subimage of the image with the given image starting at the given position
 * @param image The replacement image
 * @param startx 
 * @param starty 
 */
void Image::replace(const Image& image, int startx, int starty) {
  for(int row = starty; row < starty + image.mHeight; row++){
    for(int col = startx; col < startx + image.mWidth; col++){
      set(row, col, image.get(row - starty, col - startx));
    }
  }
}

Image Image::swirl() const {
  Image result(0, 0);
  return result;
}

// Part 2: Operator 9
/**
 * @brief Add two images together, clipping at 255
 * @param other Second image to be added
 * @return Sum of the two images
 */
Image Image::add(const Image& other) const {
  Image result(width(), height());
  for(int row = 0; row < height(); row++){
    for(int col = 0; col < width(); col++){
      Pixel p1 = get(row, col);
      Pixel p2 = other.get(row, col);
      Pixel p3 = p1 + p2;
      result.set(row, col, p3);
    }
  }
  return result;
}

Image Image::subtract(const Image& other) const {
  Image result(0, 0);

  return result;
}

Image Image::multiply(const Image& other) const {
  Image result(0, 0);

  return result;
}

Image Image::difference(const Image& other) const {
  Image result(0, 0);

  return result;
}

Image Image::lightest(const Image& other) const {
  Image result(mWidth, mHeight);
  for(int row = 0; row < mHeight; row++){
    for(int col = 0; col < mWidth; col++){
      Pixel p1 = get(row, col);
      Pixel p2 = other.get(row, col);
      float v1 = sqrt((int)p1.r * p1.r + (int)p1.g * p1.g + (int)p1.b * p1.b);
      float v2 = sqrt((int)p2.r * p2.r + (int)p2.g * p2.g + (int)p2.b * p2.b);
      if(v2 > v1){
        result.set(row, col, p2);
      }
      else {
        result.set(row, col, p1);
      }
    }
  }
  return result;
}

Image Image::darkest(const Image& other) const {
  Image result(mWidth, mHeight);
  for(int row = 0; row < mHeight; row++){
    for(int col = 0; col < mWidth; col++){
      Pixel p1 = get(row, col);
      Pixel p2 = other.get(row, col);
      float v1 = sqrt((int)p1.r * p1.r + (int)p1.g * p1.g + (int)p1.b * p1.b);
      float v2 = sqrt((int)p2.r * p2.r + (int)p2.g * p2.g + (int)p2.b * p2.b);
      if(v2 < v1){
        result.set(row, col, p2);
      }
      else {
        result.set(row, col, p1);
      }
    }
  }
  return result;
}

/**
 * @brief Correct the gamma of the image using the given gamma value
 * @param gamma 
 * @return Corrected image 
 */
Image Image::gammaCorrect(float gamma) const {
  Image result(mWidth, mHeight);
  for(int row = 0; row < mHeight; row++){
    for(int col = 0; col < mWidth; col++){
      Pixel p = get(row, col);
      float r_tmp = p.r / 255.0;
      float g_tmp = p.g / 255.0;
      float b_tmp = p.b / 255.0;
      r_tmp = std::pow(r_tmp, 1/gamma);
      g_tmp = std::pow(g_tmp, 1/gamma);
      b_tmp = std::pow(b_tmp, 1/gamma);
      p.r = r_tmp * 255.0;
      p.g = g_tmp * 255.0;
      p.b = b_tmp * 255.0;
      result.set(row, col, p);
    }
  }
  return result;
}

/**
 * @brief Blend the image with the given image using the given alpha value
 * @param other The other image
 * @param alpha
 * @return Blended image 
 */
Image Image::alphaBlend(const Image& other, float alpha) const {
  Image result(mWidth, mHeight);
  for(int row = 0; row < mHeight; row++){
    for(int col = 0; col < mWidth; col++){
      Pixel p1 = get(row, col);
      Pixel p2 = other.get(row, col);
      p1 = p1 * (1 - alpha) + p2 * alpha;
      result.set(row, col, p1);
    }
  }
  return result;
}

// Part 2: Operator 2
/**
 * @brief Invert the colors of the image
 * @return Inverted image 
 */
Image Image::invert() const {
  Image result(mWidth, mHeight);
  for(int row = 0; row < mHeight; row++){
    for(int col = 0; col < mWidth; col++){
      Pixel p = get(row, col);
      p.r = 255 - p.r;
      p.g = 255 - p.g;
      p.b = 255 - p.b;
      result.set(row, col, p);
    }
  }
  return result;
}

/**
 * @brief Convert the image to grayscale using a weighted average of channels
 * @return Grayscale image 
 */
Image Image::grayscale() const {
  Image result(mWidth, mHeight);
  for(int row = 0; row < mHeight; row++){
    for(int col = 0; col < mWidth; col++){
      Pixel p = get(row, col);
      float value = 0.3 * p.r + 0.59 * p.g + 0.11 * p.b;
      p.r = value;
      p.g = value;
      p.b = value;
      result.set(row, col, p);
    }
  }
  return result;
}

// Part 2: Operator 3
/**
 * @brief Color jitter the image by the given amount
 * @param size Degree of jitter
 * @return Jittered image 
 */
Image Image::colorJitter(int size) const {
  Image image(mWidth, mHeight);
  srand(time(NULL));
  Pixel delta = Pixel((rand() % 255), (rand() % 255), (rand() % 255));
  delta = (delta / 255.0) * size;
  for(int row = 0; row < mHeight; row++){
    for(int col = 0; col < mWidth; col++){
      Pixel p = get(row, col);
      p = p + delta;
      image.set(row, col, p);
    }
  }
  return image;
}

// Part 2: Operator 4
/**
 * @brief Shift the channels of the image by the given amount
 * @param rShift Amount to shift the red channel
 * @param gShift Amount to shift the green channel
 * @param bShift Amount to shift the blue channel
 * @return Shifted image 
 */
Image Image::channelShift(int rShift[2], int gShift[2], int bShift[2]) const {
  Image result(mWidth, mHeight);
  for(int col = 0; col < mWidth; col++){
    for(int row = 0; row < mHeight; row++){
      Pixel rSource = get(row + rShift[1], col + rShift[0]);
      Pixel gSource = get(row + gShift[1], col + gShift[0]);
      Pixel bSource = get(row + bShift[1], col + bShift[0]);
      Pixel p = Pixel(rSource.r, gSource.g, bSource.b);
      result.set(row, col, p);
    }
  }
  return result;
}

// Part 2: Operator 5
/**
 * @brief Emulate a halftone print using the given shift values
 * @param rShift Amount to shift the red channel
 * @param gShift Amount to shift the green channel
 * @param bShift Amount to shift the blue channel
 * @return Halftoned image 
 */
Image Image::halftone(int rShift[2], int gShift[2], int bShift[2]) const {
  int scale_factor = 2;
  int enlarge_factor = 4;
  Image downsampled = resize(mWidth / scale_factor, mHeight / scale_factor);
  Image dots(mWidth * enlarge_factor, mHeight * enlarge_factor);
  for(int col = 0; col < dots.width(); col++){
    for(int row = 0; row < dots.height(); row++){
      Pixel p = Pixel(0, 0, 0);
      dots.set(row, col, p);
    }
  }
  for(int col = 0; col < downsampled.width(); col++){
    for(int row = 0; row < downsampled.height(); row++){
      Pixel p = downsampled.get(row, col);
      int r = (scale_factor * enlarge_factor) / 2;
      for(int x = 0; x < scale_factor * enlarge_factor; x++){
        for(int y = 0; y < scale_factor * enlarge_factor; y++){
          if((x - r) * (x - r) + (y - r) * (y - r) <= r * r){
            dots.set(row * scale_factor * enlarge_factor + y, col * scale_factor * enlarge_factor + x, p);
          }
        }
      }
    }
  }
  return dots.channelShift(rShift, gShift, bShift);
}

// Part 2: Operator 6
/**
 * @brief Replace pixels of oldColor with newColor within the given tolerance
 * @param oldColor Color to replace
 * @param newColor Color to replace with
 * @param tolerance Tolerance for color replacement
 * @return Color replaced image 
 */
Image Image::colorReplace(const Pixel& oldColor, const Pixel& newColor, int tolerance) const {
  Image image(mWidth, mHeight);
  int r_diff, g_diff, b_diff, diff;
  float alpha = 1;
  for(int row = 0; row < mHeight; row++){
    for(int col = 0; col < mWidth; col++){
      Pixel p = get(row, col);
      r_diff = abs(p.r - oldColor.r);
      g_diff = abs(p.g - oldColor.g);
      b_diff = abs(p.b - oldColor.b);
      diff = sqrt(r_diff * r_diff + g_diff * g_diff + b_diff * b_diff);
      if(diff <= tolerance){
        p = (newColor * alpha) + (p * (1 - alpha));
      }
      image.set(row, col, p);
    }
  }
  return image;
}

void normalize(const Pixel& p, float *normalized){
  int maxComponent = p.r;
  if(p.g > maxComponent){
    maxComponent = p.g;
  }
  if(p.b > maxComponent){
    maxComponent = p.b;
  }
  normalized[0] = p.r / (float)maxComponent;
  normalized[1] = p.g / (float)maxComponent;
  normalized[2] = p.b / (float)maxComponent;
}

/**
 * @brief Performs convolution on the image with the given kernel and places the result in "out"
 * @param kernel 1D array describing the square convolution kernel
 * @param kSize width of kernel
 */
void Image::convolve(float *kernel, int kSize, float *out) const {
  memset(out, 0, mWidth * mHeight * 3 * sizeof(float));
  int padding = (kSize - 1) / 2;
  int outWidth = (mWidth - kSize + (2 * padding)) + 1;
  // int outHeight = (mHeight - kSize + (2 * padding)) + 1;
  int outRow = 0;
  int outCol = 0;
  for(int imageRow = 0 - padding; imageRow < 0 - padding + mHeight; imageRow++){
    for(int imageCol = 0 - padding; imageCol < 0 - padding + mWidth; imageCol++){
      float rSum = 0;
      float gSum = 0;
      float bSum = 0;
      for(int kernelRow = 0; kernelRow < kSize; kernelRow++){
        for(int kernelCol = 0; kernelCol < kSize; kernelCol++){
          rSum += get(imageRow + kernelRow, imageCol + kernelCol).r * kernel[kernelRow * kSize + kernelCol];
          gSum += get(imageRow + kernelRow, imageCol + kernelCol).g * kernel[kernelRow * kSize + kernelCol];
          bSum += get(imageRow + kernelRow, imageCol + kernelCol).b * kernel[kernelRow * kSize + kernelCol];
        }
      }
      out[(outRow * outWidth * 3) + (outCol * 3) + 0] = rSum;
      out[(outRow * outWidth * 3) + (outCol * 3) + 1] = gSum;
      out[(outRow * outWidth * 3) + (outCol * 3) + 2] = bSum;
      outCol++;
    }
    outCol = 0;
    outRow++;
  }
}

/**
 * @brief Convert a 1D array of floats by scaling each value to be between 0 and 255
 * @param arr 1D array of floats
 * @param width width of image
 * @param height height of image
 * @return Image instance 
 */
Image arrToImage(float *arr, int width, int height) {
  // get maximum value of out
  float max = 0;
  float min = -1;
  for (int i = 0; i < width*height*3; i++) {
    if (arr[i] > max) {
        max = arr[i];
    }
    if (arr[i] < min || min == -1) {
        min = arr[i];
    }
  }

  unsigned char outChar[width*height*3];
   // divide each value by max
   for (int i = 0; i < width*height*3; i++) {
      outChar[i] = 255*((arr[i])/max);
      if(outChar[i] > 255){
         outChar[i] = 255;
      }
      if(outChar[i] < 0){
         outChar[i] = 0;
      }
   }

   Image newImage = Image(width, height);
   for(int row = 0; row < newImage.height(); row++){
      for(int col = 0; col < newImage.width(); col++){
         Pixel p = Pixel(outChar[row*width*3 + col*3], outChar[row*width*3 + col*3 + 1], outChar[row*width*3 + col*3 + 2]);
         newImage.set(row, col, p);
      }
   }
   return newImage;
}

// Part 2: Operator 7
/**
 * @brief Sobel edge detection (horizontal and vertical)
 * @return Filtered image 
 */
Image Image::sobel() const {
  float horizontal_kernel[9] = {
    -1,0,1,
    -2,0,2,
    -1,0,1
  };

  float vertical_kernel[9] = {
    -1,-2,-1,
    0,0,0,
    1,2,1
  };

  float *out_horizontal = new float[mWidth * mHeight * 3];
  float *out_vertical = new float[mWidth * mHeight * 3];

  convolve(horizontal_kernel, 3, out_horizontal);
  convolve(vertical_kernel, 3, out_vertical);

  Image horizontal_result = arrToImage(out_horizontal, mWidth, mHeight);
  Image vertical_result = arrToImage(out_vertical, mWidth, mHeight);

  Image result = horizontal_result.add(vertical_result);

  delete[] out_horizontal;
  delete[] out_vertical;
  return result;
}

// Part 2: Operator 8
/**
 * @brief Blur the image with a Gaussian kernel
 * @param sigma Standard deviation of Gaussian kernel
 * @return Blurred image
 */
Image Image::gaussianBlur(float sigma) const {
  int kSize = 2 * ceil(3 * sigma) + 1;
  float *kernel = new float[kSize * kSize];
  float sum = 0;
  for(int row = 0; row < kSize; row++){
    for(int col = 0; col < kSize; col++){
      kernel[row * kSize + col] = exp(-((row - kSize/2) * (row - kSize/2) + (col - kSize/2) * (col - kSize/2)) / (2 * sigma * sigma));
      sum += kernel[row * kSize + col];
    }
  }
  for(int row = 0; row < kSize; row++){
    for(int col = 0; col < kSize; col++){
      kernel[row * kSize + col] /= sum;
    }
  }
  float *out = new float[mWidth * mHeight * 3];
  convolve(kernel, kSize, out);
  Image result = arrToImage(out, mWidth, mHeight);
  delete[] kernel;
  delete[] out;
  return result;
}

Image Image::expandOutlines(int iterations) const {
  Image result(*this);

  for(int k = 0; k < iterations; k++){
    int nColorPixels = 0;
    int colorPixels[mWidth * mHeight][2];

    for(int row = 0; row < mHeight; row++){
      for(int col = 0; col < mWidth; col++){
        if(result.get(row, col).r > 10 || result.get(row, col).g > 10 || result.get(row, col).b > 10){
          colorPixels[nColorPixels][0] = col;
          colorPixels[nColorPixels][1] = row;
          nColorPixels++;
        }
      }
    }

    for(int i = 0; i < nColorPixels; i++){
      int col = colorPixels[i][0];
      int row = colorPixels[i][1];
      for(int rowOffset = -1; rowOffset <= 1; rowOffset++){
        for(int colOffset = -1; colOffset <= 1; colOffset++){
          if(row + rowOffset >= 0 && row + rowOffset < mHeight && col + colOffset >= 0 && col + colOffset < mWidth){
            if(result.get(row + rowOffset, col + colOffset).r < 10 && result.get(row + rowOffset, col + colOffset).g < 10 && result.get(row + rowOffset, col + colOffset).b < 10){
              result.set(row + rowOffset, col + colOffset, result.get(row, col));
            }
          }
        }
      }
    }
  }
  return result;
}

Image Image::bitmap(int size) const {
  Image image(0, 0);

  return image;
}

void Image::fill(const Pixel& c) {}

}  // namespace agl
