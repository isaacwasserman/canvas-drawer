/**
 * Implementations of the Canvas class methods
 *
 * @author: Isaac Wasserman
 * @version: February 23, 2023
 */

#include "canvas.h"

#include <math.h>

#include <cassert>

using namespace std;
using namespace agl;

Canvas::Canvas(int w, int h) : _canvas(w, h) {}

Canvas::~Canvas() {}

void Canvas::save(const std::string& filename) { _canvas.save(filename); }

int Canvas::getHeight() { return _canvas.height(); }
int Canvas::getWidth() { return _canvas.width(); }

void Canvas::begin(PrimitiveType type) { _type = type; }

void Canvas::end() {
  if (_type == LINES) {
    struct Line line;
    while (_vertices.size() > 1) {
      line = popLine();
      drawLine(line);
    }
  } else if (_type == TRIANGLES) {
    struct Triangle triangle;
    while (_vertices.size() > 2) {
      triangle = popTriangle();
      drawTriangle(triangle);
    }
  } else if (_type == QUADS) {
    struct Quad quad;
    while (_vertices.size() > 3) {
      quad = popQuad();
      drawQuad(quad);
    }
  } else if (_type == FANS) {
    struct Fan fan;
    struct Vertex outerVertsPointer[_vertices.size() - 1];
    fan = popFan(outerVertsPointer);
    drawFan(fan);
  }
  _vertices.clear();
}

struct Line Canvas::popLine() {
  struct Line line;

  struct Vertex v1 = _vertices[0];
  struct Vertex v2 = _vertices[1];

  _vertices.pop_front();

  line.v1 = v1;
  line.v2 = v2;

  return line;
}

struct Triangle Canvas::popTriangle() {
  struct Triangle triangle;

  struct Vertex v1 = _vertices[0];
  struct Vertex v2 = _vertices[1];
  struct Vertex v3 = _vertices[2];

  _vertices.pop_front();
  _vertices.pop_front();
  _vertices.pop_front();

  triangle.v1 = v1;
  triangle.v2 = v2;
  triangle.v3 = v3;

  return triangle;
}

struct Quad Canvas::popQuad() {
  struct Triangle t1;
  struct Triangle t2;
  struct Quad quad;

  struct Vertex v1 = _vertices[0];
  struct Vertex v2 = _vertices[1];
  struct Vertex v3 = _vertices[2];
  struct Vertex v4 = _vertices[3];

  _vertices.pop_front();
  _vertices.pop_front();
  _vertices.pop_front();
  _vertices.pop_front();

  t1.v1 = v1;
  t1.v2 = v2;
  t1.v3 = v3;

  t2.v1 = v3;
  t2.v2 = v1;
  t2.v3 = v4;

  quad.t1 = t1;
  quad.t2 = t2;

  return quad;
}

struct Fan Canvas::popFan(struct Vertex* outerVertsPointer) {
  struct Fan fan;

  struct Vertex center = _vertices[0];
  _vertices.pop_front();
  fan.center = center;

  int nOuterVerts = _vertices.size();
  fan.nOuterVerts = nOuterVerts;
  fan.outerVerts = outerVertsPointer;

  for (int i = 0; i < nOuterVerts; i++) {
    fan.outerVerts[i] = _vertices[i];
  }

  return fan;
}

void Canvas::drawLine(Line line) {
  int W = line.v2.x - line.v1.x;
  int H = line.v2.y - line.v1.y;

  if (abs(H) < abs(W)) {
    if (abs(line.v1.x) > abs(line.v2.x)) {
      line = Line{line.v2, line.v1};
    }
    drawLineLow(line);
  } else {
    if (abs(line.v1.y) > abs(line.v2.y)) {
      line = Line{line.v2, line.v1};
    }
    drawLineHigh(line);
  }
}

int ipart(float x) { return (int)x; }

float fpart(float x) { return x - floor(x); }

float rfpart(float x) { return 1 - fpart(x); }

void Canvas::drawLineAntialiased(Line line) {
  int dx = line.v2.x - line.v1.x;
  int dy = line.v2.y - line.v1.y;

  bool steep = abs(dy) > abs(dx);

  if (steep) {
    int oldX = line.v1.x;
    int oldY = line.v1.y;
    line.v1.x = oldY;
    line.v1.y = oldX;
    oldX = line.v2.x;
    oldY = line.v2.y;
    line.v2.x = oldY;
    line.v2.y = oldX;
  }
  if (line.v1.x > line.v2.x) {
    line = Line{line.v2, line.v1};
  }

  float gradient;
  if (dx == 0) {
    gradient = 0;
  } else {
    gradient = (float)dy / (float)dx;
  }

  float xEnd = line.v1.x;
  float yEnd = line.v1.y;
  float xGap = 0.5;
  int xpxl1 = xEnd;
  int ypxl1 = ipart(yEnd);

  if (steep) {
    plot(xpxl1, ypxl1, line.v1.strokeColor, rfpart(yEnd) * xGap,
         _currentBlendMode);
    plot(xpxl1, ypxl1 + 1, line.v1.strokeColor, fpart(yEnd) * xGap,
         _currentBlendMode);
  } else {
    plot(ypxl1, xpxl1, line.v1.strokeColor, rfpart(yEnd) * xGap,
         _currentBlendMode);
    plot(ypxl1 + 1, xpxl1, line.v1.strokeColor, fpart(yEnd) * xGap,
         _currentBlendMode);
  }
  float intery = yEnd + gradient;

  xEnd = line.v2.x;
  yEnd = line.v2.y + gradient;
  xGap = 0.5;
  int xpxl2 = xEnd;
  int ypxl2 = ipart(yEnd);
  if (steep) {
    plot(xpxl2, ypxl2, line.v1.strokeColor,
         rfpart(yEnd) * xGap * line.v1.strokeAlpha, _currentBlendMode);
    plot(xpxl2, ypxl2 + 1, line.v1.strokeColor,
         fpart(yEnd) * xGap * line.v1.strokeAlpha, _currentBlendMode);
  } else {
    plot(ypxl2, xpxl2, line.v2.strokeColor,
         rfpart(yEnd) * xGap * line.v2.strokeAlpha, _currentBlendMode);
    plot(ypxl2 + 1, xpxl2, line.v2.strokeColor,
         fpart(yEnd) * xGap * line.v2.strokeAlpha, _currentBlendMode);
  }

  Pixel color;
  float alpha;
  float prog;
  float minAlpha = abs(gradient) == 1 ? 0.2 : 0.0;
  if (steep) {
    for (int x = xpxl1 + 1; x < xpxl2; x++) {
      prog = (float)(x - xpxl1) / (float)(xpxl2 - xpxl1);
      color = line.v1.strokeColor * (1 - prog) + line.v2.strokeColor * prog;
      alpha = line.v1.strokeAlpha * (1 - prog) + line.v2.strokeAlpha * prog;
      plot(x, ipart(intery) - 1, color,
           std::max(minAlpha, fpart(intery)) * alpha, _currentBlendMode);
      plot(x, ipart(intery), color, std::max(minAlpha, rfpart(intery)) * alpha,
           _currentBlendMode);
      plot(x, ipart(intery) + 1, color,
           std::max(minAlpha, fpart(intery)) * alpha, _currentBlendMode);
      intery += gradient;
    }
  } else {
    for (int x = xpxl1 + 1; x < xpxl2; x++) {
      prog = (float)(x - xpxl1) / (float)(xpxl2 - xpxl1);
      color = line.v1.strokeColor * (1 - prog) + line.v2.strokeColor * prog;
      alpha = line.v1.strokeAlpha * (1 - prog) + line.v2.strokeAlpha * prog;
      plot(ipart(intery) - 1, x, color,
           std::max(minAlpha, fpart(intery)) * alpha, _currentBlendMode);
      plot(ipart(intery), x, color, std::max(minAlpha, rfpart(intery)) * alpha,
           _currentBlendMode);
      plot(ipart(intery) + 1, x, color,
           std::max(minAlpha, fpart(intery)) * alpha, _currentBlendMode);
      intery += gradient;
    }
  }
}

void Canvas::plot(int x, int y, Pixel color, float alpha,
                  std::string blendMode) {
  if (x < 0 || x >= _canvas.height() || y < 0 || y >= _canvas.width()) {
    return;
  }
  Pixel pixel = _canvas.get(x, y);
  if (blendMode == "normal") {
    pixel = pixel * (1 - alpha) + color * alpha;
  } else if (blendMode == "add") {
    pixel = pixel + (color * alpha);
  } else if (blendMode == "subtract") {
    pixel = pixel - (color * alpha);
  }
  _canvas.set(x, y, pixel);
}

void Canvas::drawLineLow(Line line) {
  int y = line.v1.y;
  int W = line.v2.x - line.v1.x;
  int H = line.v2.y - line.v1.y;
  int dy = 1;
  if (H < 0) {
    dy = -1;
    H = -H;
  }
  int F = 2 * H - W;
  Pixel color;
  float alpha;
  float width;
  for (int x = line.v1.x; x < line.v2.x; x++) {
    float progress = (float)(x - line.v1.x) / (float)(line.v2.x - line.v1.x);
    color = (line.v1.strokeColor * (1 - progress)) +
            (line.v2.strokeColor * progress);
    alpha = (line.v1.strokeAlpha * (1 - progress)) +
            (line.v2.strokeAlpha * progress);
    width = (line.v1.strokeWidth * (1 - progress)) +
            (line.v2.strokeWidth * progress);
    for (int i = x - (width / 2); i < x + (width / 2); i++) {
      if (!(i < 0 || i >= _canvas.width() || i < line.v1.x || i > line.v2.x)) {
        plot(y, i, color, alpha, _currentBlendMode);
      }
    }
    if (F > 0) {
      y += dy;
      F += 2 * (H - W);
    } else {
      F += 2 * H;
    }
  }
}

void Canvas::drawLineHigh(Line line) {
  int x = line.v1.x;
  int W = line.v2.x - line.v1.x;
  int H = line.v2.y - line.v1.y;
  int dx = 1;
  if (W < 0) {
    dx = -1;
    W = -W;
  }
  int F = 2 * W - H;
  Pixel color;
  float alpha;
  float width;
  for (int y = line.v1.y; y < line.v2.y; y++) {
    float progress = (float)(y - line.v1.y) / (float)(line.v2.y - line.v1.y);
    color = (line.v1.strokeColor * (1 - progress)) +
            (line.v2.strokeColor * progress);
    alpha = (line.v1.strokeAlpha * (1 - progress)) +
            (line.v2.strokeAlpha * progress);
    width = (line.v1.strokeWidth * (1 - progress)) +
            (line.v2.strokeWidth * progress);
    for (int i = y - (width / 2); i < y + (width / 2); i++) {
      if (!(i < 0 || i >= _canvas.height() || i < line.v1.y || i > line.v2.y)) {
        plot(i, x, color, alpha, _currentBlendMode);
      }
    }
    if (F > 0) {
      x += dx;
      F += 2 * (W - H);
    } else {
      F += 2 * W;
    }
  }
}

float ile(struct Vertex a, struct Vertex b, struct Vertex p) {
  return p.x * (a.y - b.y) + p.y * (b.x - a.x) + a.x * b.y - b.x * a.y;
}

void Canvas::drawTriangle(Triangle triangle) {
  int xMin = min(min(triangle.v1.x, triangle.v2.x), triangle.v3.x);
  int xMax = max(max(triangle.v1.x, triangle.v2.x), triangle.v3.x);
  int yMin = min(min(triangle.v1.y, triangle.v2.y), triangle.v3.y);
  int yMax = max(max(triangle.v1.y, triangle.v2.y), triangle.v3.y);

  float f_alpha = ile(triangle.v2, triangle.v3, triangle.v1);
  float f_beta = ile(triangle.v3, triangle.v1, triangle.v2);
  float f_gamma = ile(triangle.v1, triangle.v2, triangle.v3);

  struct Vertex offscreen1 {
    - 1, -100, Pixel {
      0, 0, 0
    }
  };
  struct Vertex offscreen2 {
    - 100, -1, Pixel {
      0, 0, 0
    }
  };

  struct Vertex p;
  float alpha, beta, gamma;
  bool in_triangle;
  Pixel color;
  for (int x = xMin; x <= xMax; x++) {
    for (int y = yMin; y <= yMax; y++) {
      p = Vertex{
          x,           y, _fillColor, _strokeColor, _fillAlpha, _strokeAlpha,
          _strokeWidth};
      alpha = ile(triangle.v2, triangle.v3, p) / f_alpha;
      beta = ile(triangle.v3, triangle.v1, p) / f_beta;
      gamma = ile(triangle.v1, triangle.v2, p) / f_gamma;
      if (alpha >= 0 && beta >= 0 && gamma >= 0) {
        if ((alpha > 0 ||
             f_alpha * ile(triangle.v2, triangle.v3, offscreen1) > 0) &&
            (beta > 0 ||
             f_beta * ile(triangle.v3, triangle.v1, offscreen1) > 0) &&
            (gamma > 0 ||
             f_gamma * ile(triangle.v1, triangle.v2, offscreen1) > 0)) {
          color = (triangle.v1.fillColor * alpha) +
                  (triangle.v2.fillColor * beta) +
                  (triangle.v3.fillColor * gamma);
          alpha = (triangle.v1.fillAlpha * alpha) +
                  (triangle.v2.fillAlpha * beta) +
                  (triangle.v3.fillAlpha * gamma);
          plot(y, x, color, alpha, _currentBlendMode);
        } else if ((alpha > 0 ||
                    f_alpha * ile(triangle.v2, triangle.v3, offscreen2) > 0) &&
                   (beta > 0 ||
                    f_beta * ile(triangle.v3, triangle.v1, offscreen2) > 0) &&
                   (gamma > 0 ||
                    f_gamma * ile(triangle.v1, triangle.v2, offscreen2) > 0)) {
          color = (triangle.v1.fillColor * alpha) +
                  (triangle.v2.fillColor * beta) +
                  (triangle.v3.fillColor * gamma);
          alpha = (triangle.v1.fillAlpha * alpha) +
                  (triangle.v2.fillAlpha * beta) +
                  (triangle.v3.fillAlpha * gamma);
          plot(y, x, color, alpha, _currentBlendMode);
        }
      }
    }
  }

  drawLine(Line{triangle.v1, triangle.v2});
  drawLine(Line{triangle.v2, triangle.v3});
  drawLine(Line{triangle.v3, triangle.v1});
}

void Canvas::drawQuad(Quad quad) {
  drawTriangle(quad.t1);
  drawTriangle(quad.t2);
}

void Canvas::drawFan(Fan fan) {
  for (int i = 0; i < fan.nOuterVerts; i++) {
    if (i == fan.nOuterVerts - 1)
      drawTriangle(Triangle{fan.center, fan.outerVerts[i], fan.outerVerts[0]});
    else
      drawTriangle(
          Triangle{fan.center, fan.outerVerts[i], fan.outerVerts[i + 1]});
  }
}

void Canvas::vertex(int x, int y) {
  struct Vertex v = {
      x, y, _fillColor, _strokeColor, _fillAlpha, _strokeAlpha, _strokeWidth};
  _vertices.push_back(v);
}

void Canvas::fillColor(unsigned char r, unsigned char g, unsigned char b,
                       float alpha) {
  _fillColor = Pixel{r, g, b};
  _fillAlpha = alpha;
}

void Canvas::strokeColor(unsigned char r, unsigned char g, unsigned char b,
                         float alpha) {
  _strokeColor = Pixel{r, g, b};
  _strokeAlpha = alpha;
}

void Canvas::blendMode(std::string mode) { _currentBlendMode = mode; }

void Canvas::background(unsigned char r, unsigned char g, unsigned char b) {
  for (int i = 0; i < _canvas.height(); i++) {
    for (int j = 0; j < _canvas.width(); j++) {
      plot(i, j, Pixel{r, g, b}, 1, "normal");
    }
  }
}

void Canvas::background(std::string path) { _canvas.load(path); }

void Canvas::strokeWidth(float width) { _strokeWidth = width; }