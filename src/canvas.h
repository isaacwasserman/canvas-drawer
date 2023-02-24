/**
 * Declarations for the Canvas class
 *
 * @author: Isaac Wasserman
 * @version: February 23, 2023
 */

#ifndef canvas_H_
#define canvas_H_

#include <deque>
#include <string>
#include <vector>

#include "image.h"

struct Vertex {
  int x;
  int y;

  agl::Pixel fillColor;
  agl::Pixel strokeColor;

  float fillAlpha;
  float strokeAlpha;

  float strokeWidth;
};

struct Line {
  struct Vertex v1;
  struct Vertex v2;
};

struct Triangle {
  struct Vertex v1;
  struct Vertex v2;
  struct Vertex v3;
  agl::Pixel color;
};

struct Quad {
  struct Triangle t1;
  struct Triangle t2;
};

struct Fan {
  struct Vertex center;
  int nOuterVerts;
  struct Vertex *outerVerts;
};

namespace agl {
enum PrimitiveType { UNDEFINED, LINES, TRIANGLES, QUADS, FANS };
class Canvas {
 public:
  Canvas(int w, int h);
  virtual ~Canvas();

  /**
   * @brief Save the canvas to the given filename
   * @param filename The file to load, relative to the running directory
   */
  void save(const std::string &filename);

  /**
   * @brief Get the width of the canvas
   * @return The width of the canvas
   */
  int getWidth();

  /**
   * @brief Get the height of the canvas
   * @return the height of the canvas
   */
  int getHeight();

  // Draw primitives with a given type (either LINES or TRIANGLES)
  // For example, the following draws a red line followed by a green line
  // begin(LINES);
  //    color(255,0,0);
  //    vertex(0,0);
  //    vertex(100,0);
  //    color(0,255,0);
  //    vertex(0, 0);
  //    vertex(0,100);
  // end();
  void begin(PrimitiveType type);
  void end();

  /**
   * @brief Specify a vertex at raster position (x,y)
   * @param x The x-coordinate of the vertex
   * @param y The y-coordinate of the vertex
   */
  void vertex(int x, int y);

  /**
   * @brief Specify the stroke (outline) width
   * @param width The width of the stroke
   */
  void strokeWidth(float width);

  /**
   * @brief Specify the stroke (outline) color and alpha
   * @param r The red component of the color
   * @param g The green component of the color
   * @param b The blue component of the color
   * @param alpha The alpha component of the color
   */
  void strokeColor(unsigned char r, unsigned char g, unsigned char b,
                   float alpha = 1.0);

  /**
   * @brief Specify the fill color and alpha
   * @param r The red component of the color
   * @param g The green component of the color
   * @param b The blue component of the color
   * @param alpha The alpha component of the color
   */
  void fillColor(unsigned char r, unsigned char g, unsigned char b,
                 float alpha = 1.0);

  /**
   * @brief Specify the blend mode
   * @param mode A string representing the blend mode
   */
  void blendMode(std::string mode);

  /**
   * @brief Fill the canvas with a solid color
   * @param r The red component of the color
   * @param g The green component of the color
   * @param b The blue component of the color
   */
  void background(unsigned char r, unsigned char g, unsigned char b);

  /**
   * @brief Fill the canvas with an image
   * @param path The path to the image file
   */
  void background(std::string path);

  /**
   * @brief "Pop" two vertices from the vertex queue and return them as a line
   * @return A Line struct
   */
  struct Line popLine();

  /**
   * @brief "Pop" three vertices from the vertex queue and return them as a
   * triangle
   * @return A Triangle struct
   */
  struct Triangle popTriangle();

  /**
   * @brief "Pop" four vertices from the vertex queue and return them as a quad
   * @return A Quad struct
   */
  struct Quad popQuad();

  /**
   * @brief "Pop" all vertices from the vertex queue and return them as a Fan
   * struct with the outer vertices added to the given array
   * @param outerVertsPointer A pointer to be populated with the outer vertices
   * @return A Fan struct
   */
  struct Fan popFan(struct Vertex *outerVertsPointer);

  /**
   * @brief Draw the given line
   * @param line The line
   */
  void drawLine(Line line);

  /**
   * @brief Draw the given steep line
   * @param line The line
   */
  void drawLineHigh(Line line);

  /**
   * @brief Draw the given non-steep line
   * @param line The line
   */
  void drawLineLow(Line line);

  /**
   * @brief Draw the given triangle
   * @param triangle The triange
   */
  void drawTriangle(Triangle triangle);

  /**
   * @brief Draw the given quad
   * @param quad The quad
   */
  void drawQuad(Quad quad);

  /**
   * @brief Draw the given triangle fan
   * @param fan The triangle fan
   */
  void drawFan(Fan fan);

  /**
   * @brief Draw the given line using the Xiaolin Wu algorithm
   * @param line The line
   */
  void drawLineAntialiased(Line line);

  /**
   * @brief Draw to the canvas using the given color, blend mode, and alpha
   * @param x The x-coordinate of the pixel
   * @param y The y-coordinate of the pixel
   * @param color The color of the pixel
   * @param alpha The alpha of the pixel
   * @param blendMode The blend mode of the pixel
   */
  void plot(int x, int y, Pixel color, float alpha, std::string blendMode);

 private:
  Image _canvas;
  std::deque<Vertex> _vertices;
  float _fillAlpha = 1.0;
  float _strokeAlpha = 1.0;
  agl::Pixel _fillColor = agl::Pixel(255, 255, 255);
  agl::Pixel _strokeColor = agl::Pixel(255, 255, 255);
  float _strokeWidth = 1.0;
  std::string _currentBlendMode = "normal";
  PrimitiveType _type;
};
}  // namespace agl

#endif
