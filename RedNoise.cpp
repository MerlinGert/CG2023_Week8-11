#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <CanvasPoint.h>
#include <Colour.h>
#include <algorithm>
#include <map>
#include "TextureMap.h"
#include "ModelTriangle.h"
#include "RayTriangleIntersection.h"
#include <cmath>
#include <numeric>


#define WIDTH 320
#define HEIGHT 240


/*std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues) {
    float step = (to - from) / (numberOfValues - 1);
    std::vector<float> values(numberOfValues);
    for (int i = 0; i < numberOfValues; i++) {
        values[i] = from + step * i;
    }
    return values;
}*/

/*void draw(DrawingWindow &window) {
	window.clearPixels();
	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < window.width; x++) {
			float red = rand() % 256;
			float green = 0.0;
			float blue = 0.0;
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}
}*/
/*void draw(DrawingWindow &window) {
    window.clearPixels();
    // from 255 to 0 mean Gradient from wight to black
    std::vector<float> gradient = interpolateSingleFloats(255, 0, window.width);
    for (size_t y = 0; y < window.height; y++) {
        for (size_t x = 0; x < window.width; x++) {
            // red = green = blue mean Grayscale
            float red ;
            float green;
            float blue;
            red=green=blue= static_cast<uint8_t>(gradient[x]);
            uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
            window.setPixelColour(x, y, colour);
        }
    }
}*/

std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfValues) {
    std::vector<glm::vec3> interpolatedValues;
    if (numberOfValues <= 1) return interpolatedValues;  // Edge case
    glm::vec3 step = (to - from) / float(numberOfValues - 1);
    for (int i = 0; i < numberOfValues; i++) {
        interpolatedValues.push_back(from + step * float(i));
    }
    return interpolatedValues;
}


/*void draw(DrawingWindow &window) {
    window.clearPixels();
    //A good way to tackle this problem is to start by creating variables for the four corners of the window and initialising them with the following primary colours:
    glm::vec3 topLeft(255, 0, 0);        // red
    glm::vec3 topRight(0, 0, 255);       // blue
    glm::vec3 bottomRight(0, 255, 0);    // green
    glm::vec3 bottomLeft(255, 255, 0);   // yellow
    //Calculate the colour of all the pixels in the first (left-most) column of the window
    std::vector<glm::vec3> leftColors = interpolateThreeElementValues(topLeft, bottomLeft, window.height);
    //Calculate the colour of all the pixels in the first (left-most) column of the window
    std::vector<glm::vec3> rightColors = interpolateThreeElementValues(topRight, bottomRight, window.height);
    for (size_t y = 0; y < window.height; y++) {
       // Calculate the colour of all the pixels in the first (left-most) column of the window
        std::vector<glm::vec3> rowColors = interpolateThreeElementValues(leftColors[y], rightColors[y], window.width);
       // std::vector<glm::vec3> rowColors = interpolateThreeElementValues(rightColors[y], leftColors[y], window.width);
        for (size_t x = 0; x < window.width; x++) {
            float red =rowColors[x].r;
            float green= rowColors[x].g;
            float blue= rowColors[x].b;
            uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
            window.setPixelColour(x, y, colour);
        }
    }
}*/





//int main(int argc, char *argv[]) {
// std::vector<float> result;
// result = interpolateSingleFloats(2.2, 8.5, 7);
// for(size_t i=0; i<result.size(); i++)
//     std::cout << result[i] << " ";
// std::cout << std::endl;

// glm::vec3 from(1.0, 4.0, 9.2);
// glm::vec3 to(4.0, 1.0, 9.8);
// std::vector<glm::vec3> results = interpolateThreeElementValues(from, to, 4);
// for (glm::vec3 vec : results) {
//    std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
//  }


//DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
//SDL_Event event;
//while (true) {
// We MUST poll for events - otherwise the window will freeze !
//if (window.pollForInputEvents(event)) handleEvent(event, window);
//draw(window);
// Need to render the frame at the end, or nothing actually gets shown on the screen !
//window.renderFrame();
//}
//}


//画线
void drawLine(DrawingWindow &window, const CanvasPoint &p1, const CanvasPoint &p2, const Colour &color) {
    int steps = std::max(abs(p2.x - p1.x), abs(p2.y - p1.y));
    auto points = interpolateThreeElementValues(glm::vec3(p1.x, p1.y, 0), glm::vec3(p2.x, p2.y, 0), steps + 1);

    uint32_t pixelColor = (255 << 24) + (color.red << 16) + (color.green << 8) + color.blue;

    for (const auto &point : points) {
        window.setPixelColour(round(point.x), round(point.y), pixelColor);
    }
}


//画三角 draw a triangle   window name,
void drawTriangle(DrawingWindow &window, const CanvasTriangle &triangle, const Colour &color) {
    drawLine(window, triangle.vertices[0], triangle.vertices[1], color);
    drawLine(window, triangle.vertices[1], triangle.vertices[2], color);
    drawLine(window, triangle.vertices[0], triangle.vertices[2], color);
}


void drawTexturedLine(DrawingWindow &window, const CanvasPoint &start, const CanvasPoint &end, const TextureMap &texture) {
//    int steps = std::max(abs(end.x - start.x), abs(end.y - start.y));
//    float xStep = (end.x - start.x) / steps;
//    float yStep = (end.y - start.y) / steps;
    float deltaX = abs(end.x - start.x);
    float deltaY = abs(end.y - start.y);
    int steps = std::max(deltaX, deltaY);
    float xStep = deltaX / steps;
    float yStep = deltaY / steps;
    float uStep = (end.texturePoint.x - start.texturePoint.x) / steps;
    float vStep = (end.texturePoint.y - start.texturePoint.y) / steps;

    float x = start.x;
    float y = start.y;
    float u = start.texturePoint.x;
    float v = start.texturePoint.y;


    for (int i = 0; i <= steps; i++) {
        if (u < 0 || u >= texture.width || v < 0 || v >= texture.height) {
            std::cerr << "Texture coordinates out of bounds: u = " << u << ", v = " << v << std::endl;
            continue;
        }
        if (u >= 0 && u < texture.width && v >= 0 && v < texture.height) {
            uint32_t color = texture.pixels[int(v) * texture.width + int(u)];
            window.setPixelColour(round(x), round(y), color);
        } else {
            // 处理坐标(u, v)超出纹理范围的情况，例如设置为默认颜色
            window.setPixelColour(round(x), round(y), 0);
        }

        x += xStep;
        y += yStep;
        u += uStep;
        v += vStep;
    }






}

std::array<CanvasPoint, 3> getSortedVertices(const CanvasTriangle &triangle) {
    std::array<CanvasPoint, 3> sortedVertices = {triangle.vertices[0], triangle.vertices[1], triangle.vertices[2]};
    std::sort(sortedVertices.begin(), sortedVertices.end(), [](const CanvasPoint &a, const CanvasPoint &b) -> bool {
        if (a.y == b.y) {
            // 如果 y 坐标相同，就按 x 坐标排序
            return a.x < b.x;
        }
        return a.y < b.y;
    });
    return sortedVertices;
}

void fillTriangle(DrawingWindow &window, const CanvasTriangle &triangle, const Colour &color) {
    std::array<CanvasPoint, 3> sortedVertices = getSortedVertices(triangle);

    float hypotenuseSlope = (sortedVertices[2].x - sortedVertices[0].x) / (sortedVertices[2].y - sortedVertices[0].y);

    int totalSteps = sortedVertices[2].y - sortedVertices[0].y;

    for (int i = 0; i <= totalSteps; i++) {
        int y = sortedVertices[0].y + i;

        // 使用std::round来舍入到最接近的整数
        int startX = std::round(sortedVertices[0].x + hypotenuseSlope * i);
        int endX;

        if (y < sortedVertices[1].y) {
            float slopeTop = (sortedVertices[1].x - sortedVertices[0].x) / (sortedVertices[1].y - sortedVertices[0].y);
            endX = std::round(sortedVertices[0].x + slopeTop * i);
        } else {
            float slopeBottom =
                    (sortedVertices[2].x - sortedVertices[1].x) / (sortedVertices[2].y - sortedVertices[1].y);
            endX = std::round(sortedVertices[1].x + slopeBottom * (i - (sortedVertices[1].y - sortedVertices[0].y)));
        }
        if (startX > endX) std::swap(startX, endX);
        drawLine(window, CanvasPoint(startX, y), CanvasPoint(endX, y), color);
    }

}

// Assumes drawLine function is defined elsewhere to draw a line between two CanvasPoints





////填图 fill the triangle
//void fillTriangle(DrawingWindow &window, const CanvasTriangle &triangle, const Colour &color) {
//    std::array<CanvasPoint, 3> sortedVertices = {triangle.vertices[0], triangle.vertices[1], triangle.vertices[2]};
//    std::sort(sortedVertices.begin(), sortedVertices.end(), [](const CanvasPoint &a, const CanvasPoint &b) -> bool {
//        return a.y < b.y;
//    });
//
//    auto computeIntersection = [](const CanvasPoint &a, const CanvasPoint &b, float y) -> float {
//        if (a.y == b.y) return a.x;
//        return a.x + (b.x - a.x) * (y - a.y) / (b.y - a.y);
//    };
//
//    for (int y = static_cast<int>(sortedVertices[0].y); y < static_cast<int>(sortedVertices[1].y); y++) {
//        int startX = computeIntersection(sortedVertices[0], sortedVertices[2], y);
//        int endX = computeIntersection(sortedVertices[0], sortedVertices[1], y);
//        drawLine(window, CanvasPoint(startX, y), CanvasPoint(endX, y), color);
//    }
//
//    for (int y = static_cast<int>(sortedVertices[1].y); y <= static_cast<int>(sortedVertices[2].y); y++) {
//        int startX = computeIntersection(sortedVertices[0], sortedVertices[2], y);
//        int endX = computeIntersection(sortedVertices[1], sortedVertices[2], y);
//        drawLine(window, CanvasPoint(startX, y), CanvasPoint(endX, y), color);
//    }
//}









void fillTexturedTriangle(DrawingWindow &window, const CanvasTriangle &triangle, const TextureMap &texture) {
    std::array<CanvasPoint, 3> sortedVertices = getSortedVertices(triangle);

    auto computeIntersection = [](const CanvasPoint &a, const CanvasPoint &b, float y) -> CanvasPoint {
        float alpha = (y - a.y) / (b.y - a.y);
        float x = a.x + alpha * (b.x - a.x);
        float u = a.texturePoint.x + alpha * (b.texturePoint.x - a.texturePoint.x);
        float v = a.texturePoint.y + alpha * (b.texturePoint.y - a.texturePoint.y);

        CanvasPoint resultPoint(x, y);
        resultPoint.texturePoint = TexturePoint(u, v);

        return resultPoint;
    };


    for (int y = static_cast<int>(sortedVertices[0].y); y < static_cast<int>(sortedVertices[1].y); y++) {
        CanvasPoint start = computeIntersection(sortedVertices[0], sortedVertices[2], y);
        CanvasPoint end = computeIntersection(sortedVertices[0], sortedVertices[1], y);

        // 检查并交换 start 和 end，确保 start 在左边
        if(start.x > end.x) {
            std::swap(start, end);
        }

        drawTexturedLine(window, start, end, texture);
    }

    for (int y = static_cast<int>(sortedVertices[1].y); y <= static_cast<int>(sortedVertices[2].y); y++) {
        CanvasPoint start = computeIntersection(sortedVertices[0], sortedVertices[2], y);
        CanvasPoint end = computeIntersection(sortedVertices[1], sortedVertices[2], y);

        // 同样，检查并交换 start 和 end
        if(start.x > end.x) {
            std::swap(start, end);
        }

        drawTexturedLine(window, start, end, texture);
    }

//    for (int y = static_cast<int>(sortedVertices[1].y); y <= static_cast<int>(sortedVertices[2].y); y++) {
//        CanvasPoint start = computeIntersection(sortedVertices[0], sortedVertices[2],y );
//        drawDebugPoint(window, start);
//        CanvasPoint end = computeIntersection(sortedVertices[1], sortedVertices[2], y);
//        drawDebugPoint(window, end);
//        drawTexturedLine(window, start, end, texture);
//
//    }


}





//
//int main() {
//    // 初始化窗口
//    DrawingWindow window(1200, 600, false);  // 假设您的窗口大小为800x600
//    // 加载纹理
//    TextureMap texture("/home/merlin/CG2023/Weekly Workbooks/03 Triangles and Textures/texture.ppm");
//    // 设置CanvasPoint和对应的TexturePoint
//    CanvasPoint p1(160, 10);
//    p1.texturePoint = TexturePoint(195, 5);
//    CanvasPoint p2(300, 230);
//    p2.texturePoint = TexturePoint(395, 380);
//    CanvasPoint p3(10, 150);
//    p3.texturePoint = TexturePoint(65, 330);
//    // 创建三角形
//    CanvasTriangle triangle(p1, p2, p3);
//    // 清空窗口 (假设黑色背景)
//    window.clearPixels();
//    // 填充三角形
//    fillTexturedTriangle(window, triangle, texture);
//
//
//
//    // 渲染
//    window.renderFrame();
//
//    // 事件循环，等待窗口关闭
//    bool running = true;
//    while (running) {
//        SDL_Event event;
//        while (window.pollForInputEvents(event)) {
//            if (event.type == SDL_QUIT) {
//                running = false;
//            }
//            // 其他事件处理，如键盘、鼠标等
//            handleEvent(event, window);
//        }
//    }
//
//    return 0;
//}





//int main(int argc, char* argv[]) {
//    DrawingWindow window = DrawingWindow(800, 600, false);
//    bool running = true;
//    while (running) {
//        SDL_Event event;
//        while (window.pollForInputEvents(event)) {
//            if (event.type == SDL_QUIT) {
//                running = false;
//            } else {
//                handleEvent(event, window);
//            }
//        }
//        window.renderFrame();
//    }
//    return 0;
//}


//week3-1
//int main(int argc, char* argv[]) {
//    DrawingWindow window = DrawingWindow(800, 600, false);
//    bool running = true;
//    while (running) {
//        SDL_Event event;
//        while (window.pollForInputEvents((event)) ){ // This is correct now
//            if (event.type == SDL_QUIT) running = false;
//        }
//        Colour white(255, 255, 255);  // Define white color
//        window.clearPixels();
//        drawLine(window, CanvasPoint(0, 0), CanvasPoint(400, 300), Colour(white));
//        drawLine(window, CanvasPoint(800, 0), CanvasPoint(400, 300), Colour(white));
//        drawLine(window, CanvasPoint(400, 0), CanvasPoint(400, 600), Colour(white));
//        drawLine(window, CanvasPoint(266, 300), CanvasPoint(533, 300), Colour(white));
//        window.renderFrame();
//    }
//}



//
//
//
//void sortVerticesByY(CanvasPoint &p1, CanvasPoint &p2, CanvasPoint &p3) {
//    if(p1.y > p2.y) std::swap(p1, p2);
//    if(p1.y > p3.y) std::swap(p1, p3);
//    if(p2.y > p3.y) std::swap(p2, p3);
//}
//
//int roundToInteger(float value) {
//    return static_cast<int>(value + 0.5);
//}
//
//void fillScanline(DrawingWindow &window, int y, float startX, float endX, const Colour &color) {
//    std::cout << "Scanning line at y = " << y << " from x = " << startX << " to x = " << endX
//              << " with color (" << color.red << ", " << color.green << ", " << color.blue << ")" << std::endl;
//
//    int xStart = static_cast<int>(startX);
//    int xEnd = static_cast<int>(endX);
//
//    uint32_t colorValue = (255 << 24) + (color.red << 16) + (color.green << 8) + color.blue;
//    for(int x = xStart; x <= xEnd; x++) {
//        window.setPixelColour(x, y, colorValue);
//    }
//}
//
//
//void drawFilledTriangle(DrawingWindow &window, const CanvasTriangle &triangle, const Colour &color) {
//    // Assuming you have a sortVerticesByY function from previous steps
//    CanvasPoint p1 = triangle.vertices[0];
//    CanvasPoint p2 = triangle.vertices[1];
//    CanvasPoint p3 = triangle.vertices[2];
//    sortVerticesByY(p1, p2, p3);
//
//    float slopeA = (p2.y - p1.y) ? (p2.x - p1.x) / (p2.y - p1.y) : 1;
//    float slopeB = (p3.y - p1.y) ? (p3.x - p1.x) / (p3.y - p1.y) : 1;
//
//    // Handle the bottom flat triangle
//    for(int y = roundToInteger(p1.y); y <= roundToInteger(p2.y); y++) {
//        float startX = p1.x + slopeA * (y - p1.y);
//        float endX = p1.x + slopeB * (y - p1.y);
//        fillScanline(window, y, startX, endX, color);
//    }
//
//    slopeA = (p3.y - p2.y) ? (p3.x - p2.x) / (p3.y - p2.y) : 1;
//
//    // Handle the top flat triangle
//    for(int y = roundToInteger(p2.y); y <= roundToInteger(p3.y); y++) {
//        float startX = p2.x + slopeA * (y - p2.y);
//        float endX = p1.x + slopeB * (y - p1.y);
//        fillScanline(window, y, startX, endX, color);
//    }
//    Colour white(255, 255, 255);
//    drawTriangle(window, triangle, white);
//}
//
//int main(int argc, char* argv[]) {
//    DrawingWindow window = DrawingWindow(800, 600, false);
//    bool running = true;
//
//    while (running) {
//        SDL_Event event;
//        while (window.pollForInputEvents(event)) {
//            if (event.type == SDL_KEYDOWN) {
//                if (event.key.keysym.sym == SDLK_u) {
//
//                    CanvasPoint p1(rand() % window.width, rand() % window.height);
//                    CanvasPoint p2(rand() % window.width, rand() % window.height);
//                    CanvasPoint p3(rand() % window.width, rand() % window.height);
//                    CanvasTriangle triangle(p1, p2, p3);
//
//                    Colour randomColor(rand() % 256, rand() % 256, rand() % 256);
//                    std::cout << "Generated color: " << randomColor.red << ", " << randomColor.green << ", " << randomColor.blue << std::endl;
//                    drawFilledTriangle(window, triangle, randomColor);
//                }
//            }
//            window.renderFrame();
//        }
//    }
//    return 0;
//}
glm::vec3 calculateTriangleNormal(const ModelTriangle &triangle) {
    glm::vec3 edge1 = triangle.vertices[1] - triangle.vertices[0];
    glm::vec3 edge2 = triangle.vertices[2] - triangle.vertices[0];
    return glm::normalize(glm::cross(edge1, edge2));
}


std::vector<ModelTriangle> loadOBJ(const std::string &filename, const std::map<std::string, Colour>& palette) {
    std::vector<ModelTriangle> triangles;
    std::vector<glm::vec3> vertices;
    std::vector<TexturePoint> texturePoints; // 添加一个向量来存储纹理点
    Colour currentColour;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open the file: " << filename << std::endl;
        return triangles;
    }

    std::string line;
    std::string currentMaterial;

    while (std::getline(file, line)) {
        std::vector<std::string> tokens = split(line, ' ');
        if (tokens.empty()) continue;

        if (tokens[0] == "v") {
            vertices.emplace_back(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
        } else if (tokens[0] == "vt") {
            texturePoints.emplace_back(std::stof(tokens[1]), std::stof(tokens[2]));
        } else if (tokens[0] == "usemtl") {
            currentMaterial = tokens[1];
            currentColour = palette.at(currentMaterial);
        } else if (tokens[0] == "f") {
            std::array<glm::vec3, 3> faceVertices;
            std::array<TexturePoint, 3> faceTexturePoints;

            for (int i = 0; i < 3; i++) {
                std::vector<std::string> faceTokens = split(tokens[i + 1], '/');
                int vertexIndex = std::stoi(faceTokens[0]) - 1;
                faceVertices[i] = vertices[vertexIndex];

                if (faceTokens.size() > 1 && !faceTokens[1].empty()) {
                    int textureIndex = std::stoi(faceTokens[1]) - 1;
                    faceTexturePoints[i] = texturePoints[textureIndex];
                }
            }

            glm::vec3 normal = glm::normalize(glm::cross(faceVertices[1] - faceVertices[0], faceVertices[2] - faceVertices[0]));
            ModelTriangle triangle(faceVertices[0], faceVertices[1], faceVertices[2], currentColour);
            triangle.texturePoints = faceTexturePoints; // 这里我们添加了纹理点到三角形
            triangle.normal = calculateTriangleNormal(triangle);
            triangles.push_back(triangle);
        }
    }

    file.close();
    return triangles;
}

std::vector<ModelTriangle> loadOBJWithTexture(const std::string &filename, const std::map<std::string, Colour>& palette) {
    std::vector<ModelTriangle> triangles;
    std::vector<glm::vec3> vertices;
    std::vector<TexturePoint> texturePoints;
    Colour currentColour;
    bool isMirror = false;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open the file: " << filename << std::endl;
        return triangles;
    }

    std::string line;
    std::string currentMaterial;

    while (std::getline(file, line)) {
        std::vector<std::string> tokens = split(line, ' ');
        if (tokens.empty()) continue;

        if (tokens[0] == "v") {
            vertices.emplace_back(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
        } else if (tokens[0] == "vt") {
            texturePoints.emplace_back(std::stof(tokens[1]), std::stof(tokens[2]));
        } else if (tokens[0] == "usemtl") {
            currentMaterial = tokens[1];
            if (palette.find(currentMaterial) != palette.end()) {
                currentColour = palette.at(currentMaterial);
                isMirror = (currentMaterial == "Mirror");
            } else {
                std::cerr << "Material '" << currentMaterial << "' not found in palette." << std::endl;
                // 处理未找到材质的情况，例如使用默认颜色
                currentColour = Colour(255, 255, 255); // 或其他默认颜色
            } }else if (tokens[0] == "f") {
            std::array<glm::vec3, 3> faceVertices;
            std::array<TexturePoint, 3> faceTexturePoints;

            for (int i = 0; i < 3; i++) {
                std::vector<std::string> faceTokens = split(tokens[i + 1], '/');
                int vertexIndex = std::stoi(faceTokens[0]) - 1;
                faceVertices[i] = vertices[vertexIndex];

                if (faceTokens.size() > 1 && !faceTokens[1].empty()) {
                    int textureIndex = std::stoi(faceTokens[1]) - 1;
                    faceTexturePoints[i] = texturePoints[textureIndex];
                }
            }

            glm::vec3 normal = glm::normalize(glm::cross(faceVertices[1] - faceVertices[0], faceVertices[2] - faceVertices[0]));
            ModelTriangle triangle(faceVertices[0], faceVertices[1], faceVertices[2], currentColour);
            triangle.texturePoints = faceTexturePoints;
            triangle.normal = normal;
            triangle.isMirror = isMirror;
            triangles.push_back(triangle);
        }
    }

    file.close();
    return triangles;
}



std::map<std::string, Colour> loadMTL(const std::string& filename) {
    std::map<std::string, Colour> palette;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open the MTL file: " << filename << std::endl;
        return palette;
    }

    std::string currentMaterial;
    std::string line;
    while (std::getline(file, line)) {
        if (line.substr(0, 6) == "newmtl") {
            currentMaterial = line.substr(7);
        }
        else if (line.substr(0, 2) == "Kd" && !currentMaterial.empty()) {
            size_t pos1 = line.find(" ") + 1;
            size_t pos2 = line.find(" ", pos1) + 1;
            size_t pos3 = line.find(" ", pos2) + 1;

            float r = std::stof(line.substr(pos1, pos2 - pos1 - 1));
            float g = std::stof(line.substr(pos2, pos3 - pos2 - 1));
            float b = std::stof(line.substr(pos3));

            palette[currentMaterial] = Colour(r * 255, g * 255, b * 255);
        }
    }

    return palette;
}

struct Material {
    Colour colour;
    std::string texturePath;
};


std::map<std::string, Material> loadMTLWithTexture(const std::string& filename) {
    std::map<std::string, Material> materials;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open the MTL file: " << filename << std::endl;
        return materials;
    }

    std::string currentMaterial;
    std::string line;
    while (std::getline(file, line)) {
        if (line.substr(0, 6) == "newmtl") {
            currentMaterial = line.substr(7);
        }
        else if (line.substr(0, 2) == "Kd" && !currentMaterial.empty()) {
            size_t pos1 = line.find(" ") + 1;
            size_t pos2 = line.find(" ", pos1) + 1;
            size_t pos3 = line.find(" ", pos2) + 1;

            float r = std::stof(line.substr(pos1, pos2 - pos1 - 1));
            float g = std::stof(line.substr(pos2, pos3 - pos2 - 1));
            float b = std::stof(line.substr(pos3));

            materials[currentMaterial].colour = Colour(r * 255, g * 255, b * 255);
        }
        else if (line.substr(0, 6) == "map_Kd" && !currentMaterial.empty()) {
            size_t pos = line.find(" ") + 1;
            materials[currentMaterial].texturePath = line.substr(pos);
        }
    }

    return materials;
}






float interpolate(float from, float to, float alpha) {
    return from + alpha * (to - from);
}

void drawLineWithDepth(DrawingWindow &window, const CanvasPoint &start, const CanvasPoint &end, const Colour &color, std::vector<std::vector<float>>& depthBuffer) {
    int deltaX = end.x - start.x;
    int deltaY = end.y - start.y;
    int numberOfSteps = std::max(std::abs(deltaX), std::abs(deltaY));
    float xIncrement = static_cast<float>(deltaX) / numberOfSteps;
    float yIncrement = static_cast<float>(deltaY) / numberOfSteps;

    float currentX = start.x;
    float currentY = start.y;

    for (int i = 0; i <= numberOfSteps; i++) {
        float alpha = static_cast<float>(i) / numberOfSteps;
        float depth = 1.0f / interpolate(1.0f / start.depth, 1.0f / end.depth, alpha);

        int x = static_cast<int>(currentX);
        int y = static_cast<int>(currentY);

        if (x >= 0 && x < depthBuffer.size() && y >= 0 && y < depthBuffer[0].size() && depth < depthBuffer[x][y]) {
            uint32_t packedColor = (color.red << 16) | (color.green << 8) | color.blue;
            window.setPixelColour(x, y, packedColor);
            depthBuffer[x][y] = depth;
        }
        if (start.x < 0 || start.x >= window.width || start.y < 0 || start.y >= window.height ||
            end.x < 0 || end.x >= window.width || end.y < 0 || end.y >= window.height) {
            // 可以在这里添加代码来处理线段超出窗口的情况
            return;
        }

        currentX += xIncrement;
        currentY += yIncrement;
    }
}


glm::mat3 rotationY(float angle) {
    return glm::mat3(
            cos(angle), 0.0f, sin(angle),
            0.0f, 1.0f, 0.0f,
            -sin(angle), 0.0f, cos(angle)
    );
}

glm::mat3 rotationX (float angle) {
    return glm::mat3(
            1.0f, 0.0f, 0.0f,
            0.0f, cos(angle), -sin(angle),
            0.0f, sin(angle), cos(angle)
    );
}



glm::mat3 cameraOrientation = glm::mat3(
        1.0f, 0.0f, 0.0f,  // Right
        0.0f, 1.0f, 0.0f,  // Up
        0.0f, 0.0f, 1.0f   // Forward
);


// 应用旋转矩阵到相机位置


CanvasPoint getCanvasIntersectionPoint(const glm::vec3& cameraPosition, const glm::vec3& vertexPosition, float focalLength, int screenWidth, int screenHeight) {
    glm::vec3 relativeVertex = vertexPosition - cameraPosition;
    relativeVertex = cameraOrientation * relativeVertex;

    float scaleFactor = 240.0f;

    float x = -scaleFactor * (focalLength * (relativeVertex.x / relativeVertex.z)) + (screenWidth / 2);
    float y = scaleFactor * (focalLength * (relativeVertex.y / relativeVertex.z)) + (screenHeight / 2);
    std::cout << "Projected point: (" << x << ", " << y << ")" << std::endl;


    return CanvasPoint(x, y, relativeVertex.z);
}

//
//void handleEvent(SDL_Event event, DrawingWindow &window, glm::vec3 &cameraPosition,glm::vec3 lightPosition) {
//
//
//    if (event.type == SDL_KEYDOWN) {
//        float translationAmount = 1.0f; // 定义平移量
//        float rotationAmount = glm::radians(5.0f); // 定义旋转量（5度）
//        // 处理平移
//        if (event.key.keysym.sym == SDLK_LEFT) {
//            std::cout << "LEFT" << std::endl;
//            cameraPosition.x -= translationAmount;  // 左移
//        }
//        else if (event.key.keysym.sym == SDLK_RIGHT) {
//            std::cout << "RIGHT" << std::endl;
//            cameraPosition.x += translationAmount;  // 右移
//        }
//        else if (event.key.keysym.sym == SDLK_UP) {
//            std::cout << "UP" << std::endl;
//            cameraPosition.y += translationAmount;  // 上移
//        }
//        else if (event.key.keysym.sym == SDLK_DOWN) {
//            std::cout << "DOWN" << std::endl;
//            cameraPosition.y -= translationAmount;  // 下移
//        }
//        else if (event.key.keysym.sym == SDLK_w) {
//            cameraPosition.z -= translationAmount;  // 前进
//        }
//        else if (event.key.keysym.sym == SDLK_s) {
//            cameraPosition.z += translationAmount;  // 后退
//        }
//        if (event.key.keysym.sym == SDLK_a) {  // 向左旋转相机
//            glm::mat3 rotation = rotationY(-rotationAmount);
//            glm::mat3 rotationMatrix = rotationY(rotationAmount);
//            cameraPosition = rotation * cameraPosition;
//            cameraOrientation=rotationMatrix  * cameraOrientation;
//        }
//        if (event.key.keysym.sym == SDLK_d) {  // Rotate camera right around the origin (pan right)
//            glm::mat3 rotation = rotationY(rotationAmount); // 向右旋转
//            glm::mat3 rotationMatrix = rotationY(-rotationAmount);
//            cameraPosition = rotation * cameraPosition; // 更新相机位置
//            cameraOrientation=rotationMatrix  * cameraOrientation;
//
//
//        }
//        if (event.key.keysym.sym == SDLK_q) {  // 假设D键为绕Y轴顺时针旋转
//            glm::mat3 rotation = rotationX(rotationAmount);
//            glm::mat3 rotationMatrix = rotationX(-rotationAmount);
//            cameraPosition = rotation * cameraPosition; // 更新相机位置
//            cameraOrientation=rotationMatrix  * cameraOrientation;
//        }
//        if (event.key.keysym.sym == SDLK_e) {  // 假设D键为绕Y轴顺时针旋转
//            glm::mat3 rotation = rotationX(-rotationAmount);
//            glm::mat3 rotationMatrix = rotationX(rotationAmount);
//            cameraPosition = rotation * cameraPosition; // 更新相机位置
//            cameraOrientation=rotationMatrix  * cameraOrientation;
//
//        }
//        else if (event.key.keysym.sym == SDLK_f) {
//            // Generate three random vertices
////            CanvasPoint p1(rand() % window.width, rand() % window.height);
////            CanvasPoint p2(rand() % window.width, rand() % window.height);
////            CanvasPoint p3(rand() % window.width, rand() % window.height);
//
//            CanvasPoint p1(160, 10);
//            p1.texturePoint = TexturePoint(195, 5);
//            CanvasPoint p2(300, 230);
//            p2.texturePoint = TexturePoint(395, 380);
//            CanvasPoint p3(10, 150);
//            p3.texturePoint = TexturePoint(65, 330);
//            // 创建三角形
//            CanvasTriangle triangle(p1, p2, p3);
//
//            // Generate a random color
//            Colour randomColor(rand() % 256, rand() % 256, rand() % 256);
//            fillTriangle(window, triangle, randomColor);
//            // Draw the triangle
//            Colour whiteColor(255, 255, 255);
//            drawTriangle(window, triangle, whiteColor);
//        }
//        else if (event.key.keysym.sym == SDLK_u) {
//            CanvasPoint p1(rand() % window.width, rand() % window.height);
//            CanvasPoint p2(rand() % window.width, rand() % window.height);
//            CanvasPoint p3(rand() % window.width, rand() % window.height);
//            CanvasTriangle triangle(p1, p2, p3);
//            Colour randomColor(rand() % 256, rand() % 256, rand() % 256);
//            drawTriangle(window, triangle, randomColor);
//        }
//        if (event.key.keysym.sym == SDLK_i) {
//            std::cout << "Light UP" << std::endl;
//            lightPosition.y += translationAmount;  // 光源上移
//        }
//        else if (event.key.keysym.sym == SDLK_k) {
//            std::cout << "Light DOWN" << std::endl;
//            lightPosition.y -= translationAmount;  // 光源下移
//        }
//        else if (event.key.keysym.sym == SDLK_j) {
//            std::cout << "Light LEFT" << std::endl;
//            lightPosition.x -= translationAmount;  // 光源左移
//        }
//        else if (event.key.keysym.sym == SDLK_l) {
//            std::cout << "Light RIGHT" << std::endl;
//            lightPosition.x += translationAmount;  // 光源右移
//        }
////        if (event.key.keysym.sym == SDLK_1) {
////            fillTriangle(window,models,colour);
////            std::cout << "Switched to Wireframe mode." << std::endl;
////        }
////        else if (event.key.keysym.sym == SDLK_2) {
////
////            std::cout << "Switched to Rasterization mode." << std::endl;
////        }
////        else if (event.key.keysym.sym == SDLK_3) {
////
////            std::cout << "Switched to RayTracing mode." << std::endl;
////        }
//    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
//        window.savePPM("output.ppm");
//        window.saveBMP("output.bmp");
//    }
//}


void fillTriangle(DrawingWindow &window, const CanvasTriangle &triangle, const Colour &color, std::vector<std::vector<float>>& depthBuffer) {
    std::array<CanvasPoint, 3> sortedVertices = {triangle.vertices[0], triangle.vertices[1], triangle.vertices[2]};
    std::sort(sortedVertices.begin(), sortedVertices.end(), [](const CanvasPoint &a, const CanvasPoint &b) -> bool {
        return a.y < b.y;
    });

    auto computeIntersection = [](const CanvasPoint &a, const CanvasPoint &b, float y) {
        if (std::abs(b.y - a.y) < 1e-5) {
            // 如果两个顶点的 y 坐标几乎相同，则可以直接取其中一个顶点的 x 和 depth
            return CanvasPoint(a.x, y,abs(a.depth));
        } else {
            float slope = (y - a.y) / (b.y - a.y);
            float x = a.x + slope * (b.x - a.x);
            float depth = std::abs(a.depth + slope * (b.depth - a.depth));
            return CanvasPoint(x, y, depth);
        };
    };



    for (float y = std::round(sortedVertices[0].y); y <= sortedVertices[1].y; y += 1.0f) {
        int yInt = std::round(y); // 舍入到最接近的整数行
        CanvasPoint start = computeIntersection(sortedVertices[0], sortedVertices[2], yInt);
        CanvasPoint end = computeIntersection(sortedVertices[0], sortedVertices[1], yInt);
        if (start.x > end.x) std::swap(start, end);
        drawLineWithDepth(window, start, end, color, depthBuffer);
    }

    for (float y = std::round(sortedVertices[1].y); y <= sortedVertices[2].y; y += 1.0f) {
        int yInt = std::round(y); // 舍入到最接近的整数行
        CanvasPoint start = computeIntersection(sortedVertices[1], sortedVertices[2], yInt);
        CanvasPoint end = computeIntersection(sortedVertices[0], sortedVertices[2], yInt);
        if (start.x > end.x) std::swap(start, end);
        drawLineWithDepth(window, start, end, color, depthBuffer);
    }
}


void animateCameraOrbit(glm::vec3 &cameraPosition) {
    float rotationAmount = glm::radians(0.01f);
    glm::mat3 rotation = rotationY(rotationAmount);
    cameraPosition = rotation * cameraPosition; // 更新相机位置
//    glm::mat3 rotationMatriY = rotationY(-rotationAmount);
//    cameraOrientation= rotationMatriY * cameraOrientation;
}

void lookAt(glm::vec3 &cameraPosition) {
    glm::vec3 lookAtTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 forward = glm::normalize(lookAtTarget - cameraPosition);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(upVector, forward));
    glm::vec3 up = glm::cross(forward, right);
    cameraOrientation = glm::mat3(right, up, -forward);
}

bool isOrbiting = false;
void toggleOrbit() {
    isOrbiting = !isOrbiting;
}


void drawTexturedLineWithDepth(DrawingWindow &window, const CanvasPoint &start, const CanvasPoint &end, const TextureMap &texture, std::vector<std::vector<float>> &depthBuffer) {
    float deltaX = abs(end.x - start.x);
    float deltaY = abs(end.y - start.y);
    int steps = std::max(deltaX, deltaY);
    float xStep = deltaX / steps;
    float yStep = deltaY / steps;
    float uStep = (end.texturePoint.x - start.texturePoint.x) / steps;
    float vStep = (end.texturePoint.y - start.texturePoint.y) / steps;
    float depthStep = (end.depth - start.depth) / steps;  // 深度步进值

    float x = start.x;
    float y = start.y;
    float u = start.texturePoint.x;
    float v = start.texturePoint.y;
    float depth = start.depth;  // 当前深度值

    for (int i = 0; i <= steps; i++) {
        int xInt = round(x);
        int yInt = round(y);

        // 确保我们不会试图绘制超出窗口范围的像素
        if (xInt < 0 || xInt >= window.width || yInt < 0 || yInt >= window.height) {
            x += xStep;
            y += yStep;
            u += uStep;
            v += vStep;
            depth += depthStep;
            continue;
        }

        // 检查深度缓冲区，确定是否应该绘制该像素
        if (depth < depthBuffer[xInt][yInt]) {
            depthBuffer[xInt][yInt] = depth;

            if (u >= 0 && u < texture.width && v >= 0 && v < texture.height) {
                uint32_t color = texture.pixels[int(v) * texture.width + int(u)];
                window.setPixelColour(xInt, yInt, color);
            } else {
                // 如果需要，您可以在此处处理纹理坐标超出范围的情况
                // window.setPixelColour(xInt, yInt, fallbackColor);
            }
        }

        x += xStep;
        y += yStep;
        u += uStep;
        v += vStep;
        depth += depthStep;  // 更新深度值
    }
}


void fillTexturedTriangleWithDepth(DrawingWindow &window, const CanvasTriangle &triangle, const TextureMap &texture,std::vector<std::vector<float>> &depthBuffer) {
    std::array<CanvasPoint, 3> sortedVertices = getSortedVertices(triangle);
    auto computeIntersection = [](const CanvasPoint &a, const CanvasPoint &b, float y) -> CanvasPoint {
        float alpha = (y - a.y) / (b.y - a.y);
        float x = a.x + alpha * (b.x - a.x);
        float u = a.texturePoint.x + alpha * (b.texturePoint.x - a.texturePoint.x);
        float v = a.texturePoint.y + alpha * (b.texturePoint.y - a.texturePoint.y);
        float depth =std::abs(a.depth + alpha * (b.depth - a.depth));


        CanvasPoint resultPoint(x, y);
        resultPoint.texturePoint = TexturePoint(u, v);
        resultPoint.depth = depth;

        return resultPoint;
    };



    for (int y = static_cast<int>(sortedVertices[0].y); y < static_cast<int>(sortedVertices[1].y); y++) {
        CanvasPoint start = computeIntersection(sortedVertices[0], sortedVertices[2], y);
        CanvasPoint end = computeIntersection(sortedVertices[0], sortedVertices[1], y);

        // 检查并交换 start 和 end，确保 start 在左边
        if (start.x > end.x) {
            std::swap(start, end);
        }

        drawTexturedLine(window, start, end, texture);
    }

    for (int y = static_cast<int>(sortedVertices[1].y); y <= static_cast<int>(sortedVertices[2].y); y++) {
        CanvasPoint start = computeIntersection(sortedVertices[0], sortedVertices[2], y);
        CanvasPoint end = computeIntersection(sortedVertices[1], sortedVertices[2], y);

        // 同样，检查并交换 start 和 end
        if (start.x > end.x) {
            std::swap(start, end);
        }

        drawTexturedLineWithDepth(window, start, end, texture,depthBuffer);
    }


}


glm::vec3 getRayDirectionFromPixel(int x, int y, int screenWidth, int screenHeight, float focalLength, const glm::vec3& cameraPosition) {
    // 将像素坐标转换为相对于图像平面中心的坐标
    glm::vec3 Point;
    float scaleFactor = 60.0f;
    Point.x = (x - screenWidth / 2) /(focalLength * scaleFactor);
    Point.y = -(y - screenHeight / 2) / (focalLength * scaleFactor);
    Point.z =-focalLength;
    glm::vec3 rayDirection = Point - cameraPosition;
    glm::vec3 NormalizeRayDirection = normalize(Point - cameraPosition);
    return NormalizeRayDirection;
}


RayTriangleIntersection getClosestValidIntersection(
        const glm::vec3 &rayOrigin,
        const glm::vec3 &rayDirection,
        const std::vector<ModelTriangle> &triangles
) {
    RayTriangleIntersection closestIntersection;
    float closestDistance = std::numeric_limits<float>::infinity();
    size_t closestIndex = -1; // Initialized with an invalid index

    for (size_t i = 0; i < triangles.size(); ++i) {
        const ModelTriangle& triangle = triangles[i];
        glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
        glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
        glm::vec3 SPVector = rayOrigin - triangle.vertices[0];
        glm::mat3 DEMatrix(-rayDirection, e0, e1);
        glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

        float t = possibleSolution.x;
        float u = possibleSolution.y;
        float v = possibleSolution.z;

        if (u >= 0.0f && u <= 1.0f && v >= 0.0f && v <= 1.0f && (u + v) <= 1.0f && t > 0) {
            if (t < closestDistance) {
                closestDistance = t;
                closestIndex = i;
                closestIntersection = RayTriangleIntersection(
                        rayOrigin + rayDirection * t,
                        t,
                        triangle,
                        closestIndex // Set the triangleIndex to the current index
                );
            }
        }
    }

    if (closestDistance == std::numeric_limits<float>::infinity()) {
        return RayTriangleIntersection(glm::vec3(), std::numeric_limits<float>::infinity(), ModelTriangle(), -1);
    }

    return closestIntersection;
}

RayTriangleIntersection getReflectionIntersection(
        const glm::vec3 &rayOrigin,
        const glm::vec3 &rayDirection,
        const std::vector<ModelTriangle> &triangles,
        int depth = 0,
        const int maxDepth = 20
) {
    RayTriangleIntersection closestIntersection;
    closestIntersection.distanceFromCamera = std::numeric_limits<float>::infinity();
    closestIntersection.triangleIndex = -1;

    for (size_t i = 0; i < triangles.size(); ++i) {
        const ModelTriangle& triangle = triangles[i];
        glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
        glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
        glm::vec3 SPVector = rayOrigin - triangle.vertices[0];
        glm::mat3 DEMatrix(-rayDirection, e0, e1);
        glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

        float t = possibleSolution.x, u = possibleSolution.y, v = possibleSolution.z;

        if (u >= 0.0f && u <= 1.0f && v >= 0.0f && v <= 1.0f && (u + v) <= 1.0f && t > 0 && t < closestIntersection.distanceFromCamera) {
            closestIntersection.distanceFromCamera = t;
            closestIntersection.intersectedTriangle = triangle;
            closestIntersection.triangleIndex = i;
            closestIntersection.intersectionPoint = rayOrigin + rayDirection * t;
        }
    }

    if (depth < maxDepth && closestIntersection.triangleIndex != -1 && triangles[closestIntersection.triangleIndex].isMirror) {
        glm::vec3 reflectionDirection = glm::reflect(rayDirection, closestIntersection.intersectedTriangle.normal);
        glm::vec3 reflectionOrigin = closestIntersection.intersectionPoint + reflectionDirection * 0.001f; // 防止自我相交
        return getReflectionIntersection(reflectionOrigin, reflectionDirection, triangles, depth + 1, maxDepth);
    }

    return closestIntersection;
}


RayTriangleIntersection getClosestValidIntersectionWithReflection(
        const glm::vec3 &rayOrigin,
        const glm::vec3 &rayDirection,
        const std::vector<ModelTriangle> &triangles,
//        const TextureMap &textureMap, // 假设您有一个纹理映射
        int depth = 0,
        const int maxDepth = 5
) {
    RayTriangleIntersection closestIntersection;
    float closestDistance = std::numeric_limits<float>::infinity();
    size_t closestIndex = -1;

    // 交点计算
    for (size_t i = 0; i < triangles.size(); ++i) {
        const ModelTriangle& triangle = triangles[i];
        glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
        glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
        glm::vec3 SPVector = rayOrigin - triangle.vertices[0];
        glm::mat3 DEMatrix(-rayDirection, e0, e1);
        glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

        float t = possibleSolution.x;
        float u = possibleSolution.y;
        float v = possibleSolution.z;

        // 检查交点是否在三角形内部
        if (u >= 0.0f && u <= 1.0f && v >= 0.0f && v <= 1.0f && (u + v) <= 1.0f && t > 0 && t < closestDistance) {
            closestDistance = t;
            closestIndex = i;
            closestIntersection = RayTriangleIntersection(
                    rayOrigin + rayDirection * t,
                    t,
                    triangle,
                    closestIndex
            );
        }
    }

    // 反射处理
    if (depth < maxDepth && closestIndex != -1 && triangles[closestIndex].isMirror) {
        glm::vec3 reflectionDirection = glm::reflect(rayDirection, triangles[closestIndex].normal);
        glm::vec3 reflectionOrigin = closestIntersection.intersectionPoint + reflectionDirection * 0.001f; // 防止自我相交
        RayTriangleIntersection reflectedIntersection = getClosestValidIntersectionWithReflection(
                reflectionOrigin, reflectionDirection, triangles,  depth + 1, maxDepth);

//        // 纹理处理
//        if (reflectedIntersection.intersectedTriangle.hasTexture) {
//            glm::vec2 uv = reflectedIntersection.getUVCoordinates(); // 假设有获取UV坐标的方法
//            Colour textureColour = textureMap.getColourAt(uv); // 获取纹理映射中对应的颜色
//            reflectedIntersection.intersectedTriangle.colour = textureColour; // 设置纹理颜色
//        }

        return reflectedIntersection;
    }

    return closestIntersection;
}



bool isPointInShadow(
        const glm::vec3 &intersectionPoint,
        size_t intersectedTriangleIndex,
        const std::vector<ModelTriangle> &modelTriangles
) {
    glm::vec3 lightPosition = glm::vec3(0, 1, 1.5f);
    glm::vec3 shadowRayDirection = glm::normalize(intersectionPoint-lightPosition);
    RayTriangleIntersection Intersection = getClosestValidIntersection(lightPosition, shadowRayDirection, modelTriangles);
    return Intersection.triangleIndex == intersectedTriangleIndex;
}

bool isPointInShadow(
        const glm::vec3 &intersectionPoint,
        size_t intersectedTriangleIndex,
        const std::vector<ModelTriangle> &modelTriangles,
        const glm::vec3 &lightPosition
) {
    glm::vec3 shadowRayDirection = glm::normalize(intersectionPoint - lightPosition);
    RayTriangleIntersection Intersection = getClosestValidIntersection(lightPosition, shadowRayDirection, modelTriangles);
    return Intersection.triangleIndex == intersectedTriangleIndex;
}

bool isPointInShadow_fix(
        const glm::vec3 &intersectionPoint,
        size_t intersectedTriangleIndex,
        const std::vector<ModelTriangle> &modelTriangles
) {
    glm::vec3 lightPosition = glm::vec3(-0.2f, 0.8, 1.5f); // 光源位置
    glm::vec3 shadowRayDirection = glm::normalize(lightPosition - intersectionPoint);

    // 添加阴影偏移，避免自相交
    glm::vec3 shadowRayOrigin = intersectionPoint + 0.001f * shadowRayDirection; // shadow bias

    RayTriangleIntersection shadowIntersection = getClosestValidIntersection(
            shadowRayOrigin, shadowRayDirection, modelTriangles
    );

    // 如果交点与光源之间没有其他交点，则不在阴影中
    bool shadowCheck = (shadowIntersection.triangleIndex != intersectedTriangleIndex);
    // 确保检查的交点不是自身交点，并且是在光源与交点之间的有效交点
    bool isInShadow = shadowCheck && shadowIntersection.distanceFromCamera < glm::length(lightPosition - shadowRayOrigin);

    return isInShadow;
}


void drawRasterisedScene_fix(DrawingWindow &window, glm::vec3 cameraPosition){
    const std::string filepath = "/home/merlin/CG2023/Weekly Workbooks/04 Wireframes and Rasterising/models/cornell-box.obj";
    const std::map<std::string, Colour> palette = loadMTL("/home/merlin/CG2023/Weekly Workbooks/04 Wireframes and Rasterising/models/cornell-box.mtl");
    std::vector<ModelTriangle> models = loadOBJ(filepath, palette);
    uint32_t colour;
    for (size_t y = 0; y < window.height; y++) {
        for (size_t x = 0; x < window.width; x++) {
            glm::vec3 rayDirection = getRayDirectionFromPixel(x , y, window.width, window.height,1.0f,cameraPosition);
            RayTriangleIntersection rayIntersection = getClosestValidIntersection(cameraPosition, rayDirection, models);
            if (rayIntersection.distanceFromCamera != std::numeric_limits<float>::infinity()) {
                if (isPointInShadow_fix(rayIntersection.intersectionPoint, rayIntersection.triangleIndex, models)) {
                    uint32_t Black = (255 << 24) + (int(0) << 16) + (int(0) << 8) + int(0);
                    colour=Black;
                } else {
                    colour = (255 << 24) + (rayIntersection.intersectedTriangle.colour.red << 16) + (rayIntersection.intersectedTriangle.colour.green << 8) + rayIntersection.intersectedTriangle.colour.blue;
                }
                window.setPixelColour(x, y,colour);
            }
        }
    }

}

// 调整颜色亮度
Colour adjustBrightness(const Colour &originalColour, float brightness) {
    return Colour(
            int(originalColour.red * brightness),
            int(originalColour.green * brightness),
            int(originalColour.blue * brightness)
    );

}

// 计算三角形法线
Colour multiplyColour(const Colour &colour, float factor) {
    int red = std::min(static_cast<int>(colour.red * factor), 255);
    int green = std::min(static_cast<int>(colour.green * factor), 255);
    int blue = std::min(static_cast<int>(colour.blue * factor), 255);
    return Colour(red, green, blue);
}

Colour addColours(const Colour &colour1, const Colour &colour2) {
    int red = std::min(colour1.red + colour2.red, 255);
    int green = std::min(colour1.green + colour2.green, 255);
    int blue = std::min(colour1.blue + colour2.blue, 255);
    return Colour(red, green, blue);
}

void drawRasterisedScene_A(DrawingWindow &window, glm::vec3 cameraPosition, glm::vec3 lightPosition){
    const std::string filepath = "/home/merlin/CG2023/Weekly Workbooks/04 Wireframes and Rasterising/models/cornell-box.obj";
//    const std::string filepath = "/home/merlin/CG2023/Weekly Workbooks/07 Lighting and Shading (external lecture)/resources/sphere.obj";
    const std::map<std::string, Colour> palette = loadMTL("/home/merlin/CG2023/Weekly Workbooks/04 Wireframes and Rasterising/models/cornell-box.mtl");
//    const std::map<std::string, Colour> palette;
    std::vector<ModelTriangle> models = loadOBJ(filepath, palette);
    uint32_t colour;
    for (size_t y = 0; y < window.height; y++) {
        for (size_t x = 0; x < window.width; x++) {
            glm::vec3 rayDirection = getRayDirectionFromPixel(x, y, window.width, window.height, 1.0f, cameraPosition);
            RayTriangleIntersection rayIntersection = getClosestValidIntersection(cameraPosition, rayDirection, models);

            if (rayIntersection.distanceFromCamera != std::numeric_limits<float>::infinity()) {
                // 距离衰减

                float distance = glm::length(lightPosition - rayIntersection.intersectionPoint);
                float distanceAttenuation = 100.0f / (5.0f * M_PI * distance * distance);

                // 法线照明
                glm::vec3 lightDir = glm::normalize(lightPosition - rayIntersection.intersectionPoint);
                float dotProduct = glm::dot(rayIntersection.intersectedTriangle.normal, lightDir);
                float normalBrightness = std::max(dotProduct, 0.0f);

                float ambientLightThreshold = 0.2f;
                // 结合两种效果
                float calculatedBrightness = std::min(normalBrightness * distanceAttenuation, 1.0f);
                float combinedBrightness = std::max(ambientLightThreshold, calculatedBrightness);

                // 计算视线方向
                glm::vec3 viewDir = glm::normalize(cameraPosition - rayIntersection.intersectionPoint);

                // 计算反射向量
                glm::vec3 reflectDir = glm::reflect(-lightDir, rayIntersection.intersectedTriangle.normal);


                // 计算镜面反射强度
                float specIntensity = std::pow(std::max(glm::dot(viewDir, reflectDir), 0.0f), 256);
                float specIntensity2 = std::pow(std::max(glm::dot(viewDir, reflectDir), 0.0f), 128);
                float specIntensity3 = std::pow(std::max(glm::dot(viewDir, reflectDir), 0.0f), 64);


                // 调整颜色亮度
                Colour originalColour = rayIntersection.intersectedTriangle.colour;

                Colour adjustedColour = adjustBrightness(originalColour, combinedBrightness);

                Colour specularColor = multiplyColour(Colour(255, 255, 255), specIntensity);
                Colour finalColor = addColours(adjustBrightness(originalColour, combinedBrightness), specularColor);

                if (isPointInShadow(rayIntersection.intersectionPoint, rayIntersection.triangleIndex, models)) {
                    uint32_t packedColour =
                            (255 << 24) + (finalColor.red << 16) + (finalColor.green << 8) + finalColor.blue;
                    window.setPixelColour(x, y, packedColour);
                }
                else{
                    finalColor = adjustBrightness(rayIntersection.intersectedTriangle.colour, 0.2f);
                    uint32_t packedColour =
                            (255 << 24) + (finalColor.red/2 << 16) + (finalColor.green << 8) + finalColor.blue/2;
                    window.setPixelColour(x, y, packedColour);
                }
            }
        }
    }


}


void drawRasterisedScene_M(DrawingWindow &window, glm::vec3 cameraPosition, glm::vec3 lightPosition) {
    const std::string filepath = "/home/merlin/CG2023/Weekly Workbooks/04 Wireframes and Rasterising/models/cornell-box.obj";
    const std::map<std::string, Colour> palette = loadMTL("/home/merlin/CG2023/Weekly Workbooks/04 Wireframes and Rasterising/models/cornell-box.mtl");
    std::vector<ModelTriangle> models = loadOBJWithTexture(filepath, palette);
    for (size_t y = 0; y < window.height; y++) {
        for (size_t x = 0; x < window.width; x++) {
            glm::vec3 rayDirection = getRayDirectionFromPixel(x, y, window.width, window.height, 1.0f, cameraPosition);
            // 获取最近的有效交点
            RayTriangleIntersection rayIntersection = getClosestValidIntersectionWithReflection(cameraPosition, rayDirection, models);

            if (rayIntersection.distanceFromCamera != std::numeric_limits<float>::infinity()) {
                if (rayIntersection.intersectedTriangle.isMirror) {
                    // 处理镜面反射
                    RayTriangleIntersection reflectedIntersection = getReflectionIntersection(
                            rayIntersection.intersectionPoint,
                            glm::reflect(rayDirection, rayIntersection.intersectedTriangle.normal),
                            models
                    );

                    // 使用反射交点的颜色
                    Colour reflectedColour = reflectedIntersection.intersectedTriangle.colour;
                    uint32_t packedReflectedColour = (255 << 24) + (reflectedColour.red << 16) + (reflectedColour.green << 8) + reflectedColour.blue;
                    window.setPixelColour(x, y, packedReflectedColour);
                } else {
                    // 处理非镜面交点
                    // 距离衰减
                    float distance = glm::length(lightPosition - rayIntersection.intersectionPoint);
                    float distanceAttenuation = 100.0f / (5.0f * M_PI * distance * distance);

                    // 法线照明
                    glm::vec3 lightDir = glm::normalize(lightPosition - rayIntersection.intersectionPoint);
                    float dotProduct = glm::dot(rayIntersection.intersectedTriangle.normal, lightDir);
                    float normalBrightness = std::max(dotProduct, 0.0f);

                    float ambientLightThreshold = 0.2f;
                    // 结合两种效果
                    float calculatedBrightness = std::min(normalBrightness * distanceAttenuation, 1.0f);
                    float combinedBrightness = std::max(ambientLightThreshold, calculatedBrightness);

                    // 计算视线方向
                    glm::vec3 viewDir = glm::normalize(cameraPosition - rayIntersection.intersectionPoint);

                    // 计算反射向量
                    glm::vec3 reflectDir = glm::reflect(-lightDir, rayIntersection.intersectedTriangle.normal);

                    // 计算镜面反射强度
                    float specIntensity = std::pow(std::max(glm::dot(viewDir, reflectDir), 0.0f), 256);

                    // 调整颜色亮度
                    Colour originalColour = rayIntersection.intersectedTriangle.colour;
                    Colour adjustedColour = adjustBrightness(originalColour, combinedBrightness);
                    Colour specularColor = multiplyColour(Colour(255, 255, 255), specIntensity);
                    Colour finalColor = addColours(adjustedColour, specularColor);

                    if (isPointInShadow(rayIntersection.intersectionPoint, rayIntersection.triangleIndex, models)) {
                        uint32_t packedColour = (255 << 24) + (finalColor.red << 16) + (finalColor.green << 8) + finalColor.blue;
                        window.setPixelColour(x, y, packedColour);
                    } else {
                        finalColor = adjustBrightness(rayIntersection.intersectedTriangle.colour, 0.2f);
                        uint32_t packedColour = (255 << 24) + (finalColor.red/2 << 16) + (finalColor.green << 8) + finalColor.blue/2;
                        window.setPixelColour(x, y, packedColour);
                    }
                }
            }
        }
    }
}


//bool isPointInShadow_fix(
//        const glm::vec3 &intersectionPoint,
//        size_t intersectedTriangleIndex,
//        const std::vector<ModelTriangle> &modelTriangles,
//        const std::vector<glm::vec3> &lightPositions
////        ,
////        float &shadowIntensity
//){
//    float shadowIntensity = 1.0f;
//    for (const auto& lightPosition : lightPositions) {
//        glm::vec3 shadowRayDirection = glm::normalize(lightPosition - intersectionPoint);
//        glm::vec3 shadowRayOrigin = intersectionPoint + 0.001f * shadowRayDirection;
//        RayTriangleIntersection shadowIntersection = getClosestValidIntersection(shadowRayOrigin, shadowRayDirection, modelTriangles);
//
//        bool shadowCheck = (shadowIntersection.triangleIndex != intersectedTriangleIndex);
//        bool isShadowed = shadowCheck && shadowIntersection.distanceFromCamera < glm::length(lightPosition - shadowRayOrigin);
//
//        if (isShadowed) {
//            // 根据与光源的距离计算阴影强度
//            float shadowDistance = glm::length(lightPosition - intersectionPoint);
//            shadowIntensity = glm::min(shadowIntensity, 1.0f / (shadowDistance * shadowDistance));
//        }
//    }
//
//    return shadowIntensity < 1.0f; // 如果阴影强度小于1，认为处于阴影中
//}

//bool isPointInShadow_fix(
//        const glm::vec3 &intersectionPoint,
//        size_t intersectedTriangleIndex,
//        const std::vector<ModelTriangle> &modelTriangles,
//        const std::vector<glm::vec3> &lightPositions,
//        float &shadowFactor
//) {
//    int shadowCount = 0;
//    int totalRays = lightPositions.size(); // 假设每个光源位置发射一个阴影射线
//
//    for (const auto& lightPosition : lightPositions) {
//        glm::vec3 shadowRayDirection = glm::normalize(lightPosition - intersectionPoint);
//        glm::vec3 shadowRayOrigin = intersectionPoint + 0.001f * shadowRayDirection;
//
//        RayTriangleIntersection shadowIntersection = getClosestValidIntersection(shadowRayOrigin, shadowRayDirection, modelTriangles);
//
//        if (shadowIntersection.triangleIndex != intersectedTriangleIndex &&
//            shadowIntersection.distanceFromCamera < glm::length(lightPosition - shadowRayOrigin)) {
//            shadowCount++;
//        }
//    }
//
//    // 计算阴影因子，阴影数量越多，阴影越深
//    shadowFactor = static_cast<float>(shadowCount) / totalRays;
//    return shadowCount > 0; // 如果有任何阴影，返回true
//}
//

bool isPointInShadow_fix(
        const glm::vec3 &intersectionPoint,
        size_t intersectedTriangleIndex,
        const std::vector<ModelTriangle> &modelTriangles,
        const std::vector<glm::vec3> &lightPositions,
        float &shadowFactor
) {
    float shadowSum = 0.0f;
    int totalRays = lightPositions.size(); // 每个光源位置发射一个阴影射线

    for (const auto& lightPosition : lightPositions) {
        glm::vec3 shadowRayDirection = glm::normalize(lightPosition - intersectionPoint);
        glm::vec3 shadowRayOrigin = intersectionPoint + 0.001f * shadowRayDirection;

        RayTriangleIntersection shadowIntersection = getClosestValidIntersection(shadowRayOrigin, shadowRayDirection, modelTriangles);

        if (shadowIntersection.triangleIndex != intersectedTriangleIndex &&
            shadowIntersection.distanceFromCamera < glm::length(lightPosition - shadowRayOrigin)) {
            float shadowDistance = glm::length(lightPosition - intersectionPoint);
            float distanceFactor = std::min(100.0f / (shadowDistance * shadowDistance), 1.0f); // 限制衰减因子的最大值
            shadowSum += distanceFactor;
        }
    }

    float shadowScalingFactor = 1.0f; // 阴影缩放因子，可以调整以控制阴影的深浅
    shadowFactor = (shadowSum / totalRays) * shadowScalingFactor;
    return shadowFactor > 0.0f; // 如果有任何阴影，返回true
}


void drawRasterisedScene_M(DrawingWindow &window, glm::vec3 cameraPosition, std::vector<glm::vec3> lightPositions) {
    const std::string filepath = "/home/merlin/CG2023/Weekly Workbooks/04 Wireframes and Rasterising/models/cornell-box.obj";
//    const std::string filepath = "/home/merlin/CG2023/Weekly Workbooks/07 Lighting and Shading (external lecture)/resources/sphere.obj";
    const std::map<std::string, Colour> palette = loadMTL("/home/merlin/CG2023/Weekly Workbooks/04 Wireframes and Rasterising/models/cornell-box.mtl");
//    const std::map<std::string, Colour> palette;
    std::vector<ModelTriangle> models = loadOBJWithTexture(filepath, palette);
    for (size_t y = 0; y < window.height; y++) {
        for (size_t x = 0; x < window.width; x++) {
            glm::vec3 rayDirection = getRayDirectionFromPixel(x, y, window.width, window.height, 1.0f, cameraPosition);
            RayTriangleIntersection rayIntersection = getClosestValidIntersection(cameraPosition, rayDirection, models);

            if (rayIntersection.distanceFromCamera != std::numeric_limits<float>::infinity()) {
                // 首先检测是否在阴影中
                float shadowFactor=0.5f;
                bool inShadow = isPointInShadow_fix(rayIntersection.intersectionPoint, rayIntersection.triangleIndex, models, lightPositions, shadowFactor);

                // 阴影内的颜色计算
                if (inShadow) {
//
//                    Colour shadowColour = adjustBrightness(rayIntersection.intersectedTriangle.colour, 1.0f - shadowFactor);
//                    Colour shadowColour = adjustBrightness(shadowColours, 0.2f);
                    float minBrightness = 0.3f;  // 设置一个最小亮度值，防止阴影过于黑暗
                    float shadowBrightness = std::max(1.0f - shadowFactor, minBrightness);  // 确保亮度不低于最小亮度
                    Colour shadowColour = adjustBrightness(rayIntersection.intersectedTriangle.colour, shadowBrightness);

                    uint32_t packedColour = (255 << 24) + (shadowColour.red/2<< 16) + (shadowColour.green/2 << 8) + shadowColour.blue/2;
                    window.setPixelColour(x, y, packedColour);
                    continue;
                }

                // 阴影外的光照计算
                Colour finalColour = rayIntersection.intersectedTriangle.colour;
                glm::vec3 normal = rayIntersection.intersectedTriangle.normal;
                glm::vec3 viewDirection = glm::normalize(cameraPosition - rayIntersection.intersectionPoint);
                float totalDiffuse = 0.0f;
                float totalSpecular = 0.0f;

                for (const auto &lightPosition: lightPositions) {
                    glm::vec3 lightDirection = glm::normalize(lightPosition - rayIntersection.intersectionPoint);
                    float lightDistance = glm::length(lightPosition - rayIntersection.intersectionPoint);
                    float attenuation = 1.0f / (lightDistance * lightDistance);

                    // 漫反射计算
                    float diffuse = std::max(glm::dot(normal, lightDirection), 0.0f);
                    totalDiffuse += diffuse * attenuation;

                    // 镜面反射计算
                    glm::vec3 reflectDirection = glm::reflect(-lightDirection, normal);
                    float specular = std::pow(std::max(glm::dot(viewDirection, reflectDirection), 0.0f), 32);
                    totalSpecular += specular * attenuation;
                }

                // 组合漫反射和镜面反射的影响
                finalColour = adjustBrightness(finalColour, totalDiffuse);
                Colour specularColour(255, 255, 255); // 镜面高光颜色（白色）
                finalColour = addColours(finalColour, multiplyColour(specularColour, totalSpecular));

                uint32_t packedColour = (255 << 24) + (finalColour.red << 16) + (finalColour.green << 8) + finalColour.blue;
                window.setPixelColour(x, y, packedColour);
            }
        }
    }
}




//glm::vec3 vertexNormalCalculator(glm::vec3 vertex, const std::vector<ModelTriangle>& sphereModel) {
//    glm::vec3 vertexNormal;
//    float faceNumber = 0.0f;
//    for (ModelTriangle triangle : sphereModel) {
//     if (triangle.vertices[0] == vertex || triangle.vertices[1] == vertex || triangle.vertices[2] == vertex) {
//     faceNumber++;
//     vertexNormal += triangle.normal;
//        }
//    }
//    vertexNormal = vertexNormal * (1 / faceNumber);
//    return glm::normalize(vertexNormal);
//}


float triangleArea(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c) {
    return glm::length(glm::cross(b - a, c - a)) / 2.0f;
}

glm::vec3 vertexNormalCalculator(glm::vec3 vertex, const std::vector<ModelTriangle>& sphereModel) {
    glm::vec3 vertexNormal = glm::vec3(0.0f, 0.0f, 0.0f);
    float totalArea = 0.0f;

    for (const ModelTriangle &triangle : sphereModel) {
        if (triangle.vertices[0] == vertex || triangle.vertices[1] == vertex || triangle.vertices[2] == vertex) {
            float area = triangleArea(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2]);
            totalArea += area;
            vertexNormal += area * triangle.normal; // 使用面积作为权重
        }
    }
    if (totalArea > 0.0f) {
        vertexNormal /= totalArea;
    }
    return glm::normalize(vertexNormal);
}



//float calculateVertexBrightness(const glm::vec3& vertex, const glm::vec3& vertexNormal,
//                                const glm::vec3& lightPosition, const glm::vec3& cameraPosition,
//                                float lightPower, float glossiness, float ambientLight) {
//    glm::vec3 lightDirection = glm::normalize(lightPosition-vertex );
//    glm::vec3 vectorToLight = lightPosition - vertex;
//    float distanceToLight = glm::length(vectorToLight);
//    float proximityLighting = lightPower / (4 * M_PI * distanceToLight * distanceToLight);
//
//    float incidenceAngle = std::max(0.0f, glm::dot(lightDirection, vertexNormal));
//    proximityLighting *= incidenceAngle;
//
//    glm::vec3 viewDirection = glm::normalize(vertex - cameraPosition);
//    glm::vec3 reflectionVector = lightDirection - (2.0f * vertexNormal * glm::dot(vertexNormal, lightDirection));
//    float specularLighting = 1.5f*glm::pow(glm::dot(reflectionVector, viewDirection), 256);
//
//    return proximityLighting + specularLighting + ambientLight;
//}

float calculateVertexBrightness(const glm::vec3& vertex, const glm::vec3& vertexNormal,
                                const glm::vec3& lightPosition, const glm::vec3& cameraPosition,
                                float lightPower, float glossiness, float ambientLight) {
    glm::vec3 vectorToLight = lightPosition - vertex;
    glm::vec3 lightDirection = glm::normalize(vectorToLight);
    float distanceToLight = glm::length(vectorToLight);
    // 距离衰减
    float distanceAttenuation = 100.0f / (4.0f * M_PI * distanceToLight * distanceToLight);
    // 漫反射
    float incidenceAngle = std::max(0.0f, glm::dot(lightDirection, vertexNormal));
    float diffuseLighting = lightPower * incidenceAngle * distanceAttenuation;

    // 镜面反射
    glm::vec3 viewDirection = glm::normalize(cameraPosition - vertex);
    glm::vec3 reflectionVector = glm::reflect(-lightDirection, vertexNormal);
//    float specularLighting = 5 * std::pow(std::max(glm::dot(reflectionVector, viewDirection), glossiness));
    float specIntensity = std::pow(std::max(glm::dot(viewDirection, reflectionVector), 0.0f), 256);
    float combinedLighting = std::max(0.05f, diffuseLighting + specIntensity);

    return combinedLighting;
}




void drawSphereWithGourandShading(DrawingWindow &window, const std::vector<ModelTriangle>& sphereModel, glm::vec3 cameraPosition, glm::vec3 lightPosition, float focalLength, float lightPower, float ambient) {
    for(int y = 0; y < window.height; y++) {
        for(int x = 0; x < window.width; x++) {
            CanvasPoint canvasPoint = CanvasPoint(float(x), float(y));
            glm::vec3 rayDirection = getRayDirectionFromPixel(x, y, window.width, window.height, 2.0f, cameraPosition);

            RayTriangleIntersection closestIntersection = getClosestValidIntersection(cameraPosition, rayDirection, sphereModel);
            if(closestIntersection.intersectionPoint == glm::vec3(0, 0, 0)) {
                uint32_t c = (255 << 24) + (0 << 16) + (0 << 8) + (0);
                window.setPixelColour(x, y, c);
                continue;
            }

            glm::vec3 v0 = closestIntersection.intersectedTriangle.vertices[0];
            glm::vec3 v1 = closestIntersection.intersectedTriangle.vertices[1];
            glm::vec3 v2 = closestIntersection.intersectedTriangle.vertices[2];
            glm::vec3 point = closestIntersection.intersectionPoint;

            glm::vec3 vn0 = vertexNormalCalculator(v0, sphereModel);
            glm::vec3 vn1 = vertexNormalCalculator(v1, sphereModel);
            glm::vec3 vn2 = vertexNormalCalculator(v2, sphereModel);

            float brightness0 = calculateVertexBrightness(v0, vn0, lightPosition, cameraPosition, lightPower, 256, 0.1);
            float brightness1 = calculateVertexBrightness(v1, vn1, lightPosition, cameraPosition, lightPower, 256, 0.1);
            float brightness2 = calculateVertexBrightness(v2, vn2, lightPosition, cameraPosition, lightPower, 256, 0.1);

            // Calculate Barycentric coordinates
            float areaTotal = triangleArea(v0, v1, v2);
            float u = triangleArea(point, v1, v2) / areaTotal;
            float v = triangleArea(point, v2, v0) / areaTotal;
            float w = 1.0f - u - v;
            // Interpolation
            canvasPoint.brightness = u * brightness0 +v * brightness1 + w * brightness2;
            std::cout << "canvasPoint.brightness: " << canvasPoint.brightness << std::endl;

            // Draw colour
            Colour originalColour(255,0,0);
            Colour adjustedColour = adjustBrightness(originalColour, canvasPoint.brightness);

            // 设置像素颜色
            uint32_t c = (255 << 24) + (adjustedColour.red << 16) + (adjustedColour.green << 8) + adjustedColour.blue;
            window.setPixelColour(x, y, c);
        }
    }
}

//glm::vec3 interpolateNormalAtPoint(const glm::vec3& point,
//                                   const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
//                                   const glm::vec3& vn0, const glm::vec3& vn1, const glm::vec3& vn2) {
//    float u = (-(point.x - v1.x) * (v2.y - v1.y) + (point.y - v1.y) * (v2.x - v1.x)) /
//                  (-(v0.x - v1.x) * (v2.y - v1.y) + (v0.y - v1.y) * (v2.x - v1.x));
//    float v = (-(point.x - v2.x) * (v0.y - v2.y) + (point.y - v2.y) * (v0.x - v2.x)) /
//                 (-(v1.x - v2.x) * (v0.y - v2.y) + (v1.y - v2.y) * (v0.x - v2.x));
//    float w= 1 - u - v;
//
//    return glm::normalize(u * vn0 + v * vn1 + w * vn2);
//}



glm::vec3 interpolateNormalAtPoint(const glm::vec3& targetPoint,
                                   const glm::vec3& vertex0, const glm::vec3& vertex1, const glm::vec3& vertex2,
                                   const glm::vec3& normal0, const glm::vec3& normal1, const glm::vec3& normal2) {
    // Pre-calculate shared terms for barycentric coordinates
    float sharedTermForU = -(vertex0.x - vertex1.x) * (vertex2.y - vertex1.y) + (vertex0.y - vertex1.y) * (vertex2.x - vertex1.x);
    float sharedTermForV = -(vertex1.x - vertex2.x) * (vertex0.y - vertex2.y) + (vertex1.y - vertex2.y) * (vertex0.x - vertex2.x);

    // Calculate barycentric coordinates for the target point
    float uNumerator = -(targetPoint.x - vertex1.x) * (vertex2.y - vertex1.y) + (targetPoint.y - vertex1.y) * (vertex2.x - vertex1.x);
    float u = uNumerator / sharedTermForU;

    float vNumerator = -(targetPoint.x - vertex2.x) * (vertex0.y - vertex2.y) + (targetPoint.y - vertex2.y) * (vertex0.x - vertex2.x);
    float v = vNumerator / sharedTermForV;

    float w = 1 - u - v;

    // Interpolate the normal at the target point
    glm::vec3 interpolatedNormal = u * normal0 + v * normal1 + w * normal2;

    // Normalize the result to ensure it's a unit vector
    return glm::normalize(interpolatedNormal);
}


void drawRaytracingPhongCameraView(DrawingWindow &window, glm::vec3 campos, std::vector<ModelTriangle> sphereModel, glm::vec3 lightPosition){
    for (int y = 0; y < window.height; y++) {
        for (int x = 0; x < window.width; x++) {
            glm::vec3 rayDirection = getRayDirectionFromPixel(x, y, window.width, window.height, 2.0f, campos);
            RayTriangleIntersection closestIntersection = getClosestValidIntersection(campos, rayDirection,
                                                                                      sphereModel);

            if (closestIntersection.triangleIndex != -1) {
                glm::vec3 v0 = closestIntersection.intersectedTriangle.vertices[0];
                glm::vec3 v1 = closestIntersection.intersectedTriangle.vertices[1];
                glm::vec3 v2 = closestIntersection.intersectedTriangle.vertices[2];
                glm::vec3 vn0 = vertexNormalCalculator(v0, sphereModel);
                glm::vec3 vn1 = vertexNormalCalculator(v1, sphereModel);
                glm::vec3 vn2 = vertexNormalCalculator(v2, sphereModel);

                glm::vec3 point = closestIntersection.intersectionPoint;
                glm::vec3 Normal = interpolateNormalAtPoint(point, v0, v1, v2, vn0, vn1, vn2);
                // 光源方向
                glm::vec3 lightDirection = glm::normalize(lightPosition - point);
                // 漫反射强度
                float diffuseIntensity = std::max(glm::dot(Normal, lightDirection), 0.0f);
                // 视线方向
                glm::vec3 viewDir = glm::normalize(campos - point);
                // 反射向量
                glm::vec3 reflectDir = glm::reflect(-lightDirection, Normal);
                // 镜面反射强度
                float specularIntensity = 1.0f * pow(std::max(glm::dot(reflectDir, viewDir), 0.0f), 64);
                // 环境光
                float ambient = 0.1f;
                float combinedBrightness = std::max(ambient, diffuseIntensity);
                Colour originalColour(255, 0, 0);
                Colour adjustedColour = adjustBrightness(originalColour, diffuseIntensity);

                Colour specularColor = multiplyColour(Colour(255, 255, 255), specularIntensity);
                Colour finalColor = addColours(adjustBrightness(originalColour, combinedBrightness), specularColor);

                uint32_t packedColour =
                        (255 << 24) + (finalColor.red << 16) + (finalColor.green << 8) + finalColor.blue;
                window.setPixelColour(x, y, packedColour);


            }
        }
    }
}





enum class RenderMode {
    Wireframe,
    Rasterization,
    RayTracing,
    light,
    ball,
    ball2,
    depth,
    SoftShadows,
    Mirror
};

RenderMode currentRenderMode = RenderMode::Wireframe;

void renderScene(DrawingWindow &window, glm::vec3 cameraPosition, glm::vec3 lightPosition,glm::vec3 lightPosition1, glm::vec3 lightPosition2) {
    const std::string filepath = "/home/merlin/CG2023/Weekly Workbooks/07 Lighting and Shading (external lecture)/resources/sphere.obj";
    const std::map<std::string, Colour> palette;
    std::vector<ModelTriangle> sphereModel = loadOBJ(filepath, palette);

    const std::string filepath2 = "/home/merlin/CG2023/Weekly Workbooks/04 Wireframes and Rasterising/models/cornell-box.obj";
    const std::map<std::string, Colour> palette2 = loadMTL(
            "/home/merlin/CG2023/Weekly Workbooks/04 Wireframes and Rasterising/models/cornell-box.mtl");
    std::vector<ModelTriangle> models = loadOBJ(filepath2, palette2);

    Colour white(255, 255, 255);
    glm::vec3 cameraForGouraud(0.0f, 1, 10000);
    Colour colour;
    std::vector<glm::vec3> lightPositions = {
            glm::vec3(0.1, 1.01f ,0.91f),
            glm::vec3(0.01, 1.02f,0.92f),
            glm::vec3(0.02, 1.03f,0.93f),
            glm::vec3(0.03, 1.04f,0.95f),
            glm::vec3(0.04, 1.05f,0.96f),
            glm::vec3(0.05, 1.06f,0.97f),
            glm::vec3(0.06, 1.07f,0.98f),
            glm::vec3(0.07, 1.08f,0.99f),
            glm::vec3(0.08, 1.09f,1.05f),
            glm::vec3(0.09, 1.10f,1.01f),
            glm::vec3(0.13, 1.10f,1.01f),
            glm::vec3(0.11, 1.10f,1.01f),
            glm::vec3(0.1, 1.1f ,1.15f),
            glm::vec3(0.1, 1.01f ,1.14f),
            glm::vec3(0.1, 1.01f ,1.21f),
            glm::vec3(0.1, 1.01f ,1.22f),
            glm::vec3(0.1, 1.01f ,1.15f),



    };


    std::vector<std::vector<float>> depthBuffer(3 * WIDTH, std::vector<float>(3 * HEIGHT, std::numeric_limits<float>::infinity()));

    switch (currentRenderMode) {
        case RenderMode::Wireframe:
            for (const ModelTriangle &triangle: models) {
                CanvasPoint points[3];
                for (int i = 0; i < 3; i++) {
                    const glm::vec3 &vertex = triangle.vertices[i];
                    points[i] = getCanvasIntersectionPoint(cameraPosition, vertex, 2.0, 3 * WIDTH, 3 * HEIGHT);
                    points[i].texturePoint = triangle.texturePoints[i];
                }

                CanvasTriangle canvasTriangle(points[0], points[1], points[2]);
                colour=triangle.colour;
                drawTriangle(window,canvasTriangle , white);
            }
            std::cout << "Switched to Wireframe mode." << std::endl;
            break;
        case RenderMode::Rasterization:
            for (const ModelTriangle &triangle: models) {
                CanvasPoint points[3];
                for (int i = 0; i < 3; i++) {
                    const glm::vec3 &vertex = triangle.vertices[i];
                    points[i] = getCanvasIntersectionPoint(cameraPosition, vertex, 2.0, 3 * WIDTH, 3 * HEIGHT);
                    points[i].texturePoint = triangle.texturePoints[i];
                }

                CanvasTriangle canvasTriangle(points[0], points[1], points[2]);
                colour=triangle.colour;
                fillTriangle(window,canvasTriangle , colour,depthBuffer);
            }
            std::cout << "Switched to Rasterization mode." << std::endl;
            break;
        case RenderMode::RayTracing: {
            drawRasterisedScene_fix(window, cameraPosition);
            break;
        }
        case RenderMode::light: {
            drawRasterisedScene_A(window, cameraPosition,lightPosition2);
            break;
        }
        case RenderMode::ball: {
            drawSphereWithGourandShading(window, sphereModel, cameraForGouraud, lightPosition, 2.0, 1, 0);
            break;
        }
        case RenderMode::ball2: {
            drawRaytracingPhongCameraView(window, cameraForGouraud, sphereModel, lightPosition1);
            break;
        }
        case RenderMode::SoftShadows: {
            drawRasterisedScene_M(window, cameraPosition,lightPositions);
            break;
        }
        case RenderMode::Mirror: {
            drawRasterisedScene_M(window, cameraPosition,lightPosition2);
            break;
        }
            std::cout << "Switched to RayTracing mode." << std::endl;
            break;
    }

    window.renderFrame(); // 更新窗口显示
}




void handleEvent_week7(SDL_Event event, DrawingWindow &window) {

    glm::vec3 cameraPosition(0, 0, 8.0f);
    glm::vec3 lightPosition(0, 5.1f,5);
    glm::vec3 lightPosition1(0.4f, 1.8f, 2.4f);
    glm::vec3 lightPosition2(0, 0, 1);


    float focalLength = 2.0f;
    if (event.type == SDL_KEYDOWN) {
        float translationAmount = 0.5f; // 定义平移量
        float rotationAmount = glm::radians(5.0f); // 定义旋转量（5度）
        // 处理平移
        if (event.key.keysym.sym == SDLK_LEFT) {
            std::cout << "LEFT" << std::endl;
            cameraPosition.x -= translationAmount;  // 左移
        } else if (event.key.keysym.sym == SDLK_RIGHT) {
            std::cout << "RIGHT" << std::endl;
            cameraPosition.x += translationAmount;  // 右移
        } else if (event.key.keysym.sym == SDLK_UP) {
            std::cout << "UP" << std::endl;
            cameraPosition.y += translationAmount;  // 上移
        } else if (event.key.keysym.sym == SDLK_DOWN) {
            std::cout << "DOWN" << std::endl;
            cameraPosition.y -= translationAmount;  // 下移
        } else if (event.key.keysym.sym == SDLK_w) {
            cameraPosition.z -= translationAmount;  // 前进
        } else if (event.key.keysym.sym == SDLK_s) {
            cameraPosition.z += translationAmount;  // 后退
        }
        if (event.key.keysym.sym == SDLK_a) {  // 向左旋转相机
            glm::mat3 rotation = rotationY(-rotationAmount);
            glm::mat3 rotationMatrix = rotationY(rotationAmount);
            cameraPosition = rotation * cameraPosition;
            cameraOrientation = rotationMatrix * cameraOrientation;
        }
        if (event.key.keysym.sym == SDLK_d) {  // Rotate camera right around the origin (pan right)
            glm::mat3 rotation = rotationY(rotationAmount); // 向右旋转
            glm::mat3 rotationMatrix = rotationY(-rotationAmount);
            cameraPosition = rotation * cameraPosition; // 更新相机位置
            cameraOrientation = rotationMatrix * cameraOrientation;


        }
        if (event.key.keysym.sym == SDLK_q) {  // 假设D键为绕Y轴顺时针旋转
            glm::mat3 rotation = rotationX(rotationAmount);
            glm::mat3 rotationMatrix = rotationX(-rotationAmount);
            cameraPosition = rotation * cameraPosition; // 更新相机位置
            cameraOrientation = rotationMatrix * cameraOrientation;
        }
        if (event.key.keysym.sym == SDLK_e) {  // 假设D键为绕Y轴顺时针旋转
            glm::mat3 rotation = rotationX(-rotationAmount);
            glm::mat3 rotationMatrix = rotationX(rotationAmount);
            cameraPosition = rotation * cameraPosition; // 更新相机位置
            cameraOrientation = rotationMatrix * cameraOrientation;

        }
        if (event.key.keysym.sym == SDLK_x) { // 如果按下的是X键
            toggleOrbit(); // 切换轨道状态
        } else if (event.key.keysym.sym == SDLK_f) {
            // Generate three random vertices
//            CanvasPoint p1(rand() % window.width, rand() % window.height);
//            CanvasPoint p2(rand() % window.width, rand() % window.height);
//            CanvasPoint p3(rand() % window.width, rand() % window.height);

            CanvasPoint p1(160, 10);
            p1.texturePoint = TexturePoint(195, 5);
            CanvasPoint p2(300, 230);
            p2.texturePoint = TexturePoint(395, 380);
            CanvasPoint p3(10, 150);
            p3.texturePoint = TexturePoint(65, 330);
            // 创建三角形
            CanvasTriangle triangle(p1, p2, p3);

            // Generate a random color
            Colour randomColor(rand() % 256, rand() % 256, rand() % 256);
            fillTriangle(window, triangle, randomColor);
            // Draw the triangle
            Colour whiteColor(255, 255, 255);
            drawTriangle(window, triangle, whiteColor);
        } else if (event.key.keysym.sym == SDLK_u) {
            CanvasPoint p1(rand() % window.width, rand() % window.height);
            CanvasPoint p2(rand() % window.width, rand() % window.height);
            CanvasPoint p3(rand() % window.width, rand() % window.height);
            CanvasTriangle triangle(p1, p2, p3);
            Colour randomColor(rand() % 256, rand() % 256, rand() % 256);
            drawTriangle(window, triangle, randomColor);
        }
        if (event.key.keysym.sym == SDLK_1) {
            window.clearPixels();
            currentRenderMode = RenderMode::Wireframe;
        } else if (event.key.keysym.sym == SDLK_2) {
            window.clearPixels();
            currentRenderMode = RenderMode::Rasterization;
        } else if (event.key.keysym.sym == SDLK_3) {
            window.clearPixels();
            currentRenderMode = RenderMode::RayTracing;
        } else if (event.key.keysym.sym == SDLK_4) {
            window.clearPixels();
            currentRenderMode = RenderMode::light;
        } else if (event.key.keysym.sym == SDLK_5) {
            window.clearPixels();
            currentRenderMode = RenderMode::ball;
        } else if (event.key.keysym.sym == SDLK_6) {
            window.clearPixels();
            currentRenderMode = RenderMode::ball2;
        }else if (event.key.keysym.sym == SDLK_7) {
            window.clearPixels();
            currentRenderMode = RenderMode::SoftShadows;}
        else if (event.key.keysym.sym == SDLK_8) {
            window.clearPixels();
            currentRenderMode = RenderMode::Mirror;
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            window.savePPM("output.ppm");
            window.saveBMP("output.bmp");
        }else if (event.key.keysym.sym == SDLK_i) {
            std::cout << "Light UP" << std::endl;
            lightPosition2.y += translationAmount;
            lightPosition1.y += translationAmount;
            lightPosition.y += translationAmount;// 光源上移
        }
        else if (event.key.keysym.sym == SDLK_k) {
            std::cout << "Light DOWN" << std::endl;
            lightPosition2.y -= translationAmount;
            lightPosition1.y -= translationAmount;
            lightPosition.y -= translationAmount;// 光源下移
        }
        else if (event.key.keysym.sym == SDLK_j) {
            std::cout << "Light LEFT" << std::endl;
            lightPosition2.x -= translationAmount;  // 光源左移
            lightPosition1.x -= translationAmount;
            lightPosition.x -= translationAmount;
        }
        else if (event.key.keysym.sym == SDLK_l) {
            std::cout << "Light RIGHT" << std::endl;
            lightPosition2.x += translationAmount;  // 光源右移
            lightPosition1.x += translationAmount;
            lightPosition.x += translationAmount;

        }

        window.clearPixels();
        renderScene(window,cameraPosition,lightPosition,lightPosition1,lightPosition2);
    }
}





int main() {

    const std::string filepath = "/home/merlin/CG2023/Weekly Workbooks/07 Lighting and Shading (external lecture)/resources/sphere.obj";
    const std::map<std::string, Colour> palette;
    std::vector<ModelTriangle> model = loadOBJ(filepath, palette);
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return -1;
    }
    DrawingWindow window(3 * WIDTH, 3 * HEIGHT, false);
    bool running = true;
    SDL_Event event;
    glm::vec3 cameraPosition(0, 0, 8.0f);
    glm::vec3 cameraForGouraud(0.0f,1,100);
    float focalLength = 2.0f;

    glm::vec3 lightPosition(0, 1.9f, 1.4f); // 光源位置
    glm::vec3 lightPosition1(0.4f, 1.8f, 2.4f); // 光源位置

    while (running) {
        while (SDL_PollEvent(&event)) {
            // 检查事件类型
            handleEvent_week7(event,window);
            if (event.type == SDL_QUIT) { // 如果是退出事件，如点击窗口的关闭按钮
                running = false;
            } else if (event.type == SDL_KEYDOWN) { // 如果是键盘按下事件
            }
        }
//        drawRasterisedScene_M(window, cameraPosition,lightPosition);
        window.renderFrame();
    }


    SDL_Quit();
    return 0;

}