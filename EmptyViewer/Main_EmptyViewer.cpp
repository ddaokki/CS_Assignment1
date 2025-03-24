#include <Windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>

#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <vector>
#include <limits>
#include <cmath>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace glm;

// -------------------------------------------------
// Global Variables
// -------------------------------------------------
int Width = 512;
int Height = 512;
std::vector<float> OutputImage;

// -------------------------------------------------
// Ray
// -------------------------------------------------
struct Ray {
    vec3 origin;
    vec3 direction;
    Ray(const vec3& o, const vec3& d) : origin(o), direction(normalize(d)) {}
};

// -------------------------------------------------
// Surface (abstract)
// -------------------------------------------------
class Surface {
public:
    virtual bool intersect(const Ray& ray, float& t) const = 0;
    virtual vec3 getColor() const = 0;
    virtual ~Surface() {}
};

// -------------------------------------------------
// Sphere
// -------------------------------------------------
class Sphere : public Surface {
public:
    vec3 center;
    float radius;
    vec3 color;

    Sphere(const vec3& c, float r, const vec3& col) : center(c), radius(r), color(col) {}

    bool intersect(const Ray& ray, float& t) const override {
        vec3 p = ray.origin - center; // 구 중심을 원점으로 옮긴 좌표계
        vec3 d = ray.direction;       // 단위벡터라면 dot(d, d) = 1

        float t_m = -dot(p, d);
        float p_dot = dot(p, d);
        float p_len2 = dot(p, p);
        float delta2 = p_dot * p_dot - p_len2 + radius * radius;

        if (delta2 < 0.0f) return false; // 교차 없음

        float delta = std::sqrt(delta2);
        float t0 = t_m - delta;
        float t1 = t_m + delta;

        if (t0 > 0.001f) {
            t = t0;
            return true;
        }
        if (t1 > 0.001f) {
            t = t1;
            return true;
        }
        return false;
    }


    vec3 getColor() const override {
        return color;
    }
};
class Plane : public Surface {
public:
    vec3 normal;   // 법선 벡터
    float d;       // 평면 방정식의 상수항
    vec3 color;

    Plane(const vec3& n, float dVal, const vec3& col)
        : normal(normalize(n)), d(dVal), color(col) {}

    // intersect: (n · (o + t*d)) + d = 0 → t = -(n·o + d)/(n·dir)
    bool intersect(const Ray& ray, float& t) const override {
        float denom = dot(normal, ray.direction);
        if (abs(denom) < 1e-5f) return false; // 광선이 평면과 평행

        t = -(dot(normal, ray.origin) + d) / denom;
        return t > 0.001f;
    }

    vec3 getColor() const override {
        return color;
    }
};


// -------------------------------------------------
// Camera
// -------------------------------------------------
class Camera {
public:
    vec3 e = vec3(0, 0, 0);              
    vec3 u = vec3(1, 0, 0);
    vec3 v = vec3(0, 1, 0);
    vec3 w = vec3(0, 0, 1);
    float l = -0.1f, r = 0.1f, b = -0.1f, t = 0.1f, d = 0.1f;
    int nx = 512, ny = 512;

    Ray getRay(int ix, int iy) const {
        float u_s = l + (r - l) * (ix + 0.5f) / nx;
        float v_s = b + (t - b) * (iy + 0.5f) / ny;
        vec3 dir = normalize(u_s * u + v_s * v - d * w);
        return Ray(e, dir);
    }
};  

// -------------------------------------------------
// Scene
// -------------------------------------------------
class Scene {
public:
    std::vector<Surface*> objects;

    void addObject(Surface* obj) {
        objects.push_back(obj);
    }

    vec3 trace(const Ray& ray) const {
        float closestT = FLT_MAX;
        Surface* hit = nullptr;

        for (auto obj : objects) {
            float t;
            if (obj->intersect(ray, t) && t < closestT) {
                closestT = t;
                hit = obj;
            }
        }

        return hit ? hit->getColor() : vec3(0); // black background
    }

    ~Scene() {
        for (auto obj : objects) delete obj;
    }
};

// -------------------------------------------------
// Render
// -------------------------------------------------
void render()
{
    Camera camera;
    Scene scene;

    // Add objects
    scene.addObject(new Plane(vec3(0, 1, 0), 2.0f, vec3(0.2f))); // y = -2 평면
    scene.addObject(new Sphere(vec3(-4, 0, -7), 1.0f, vec3(1, 0, 0)));       // S1 - Red
    scene.addObject(new Sphere(vec3(0, 0, -7), 2.0f, vec3(0, 1, 0)));        // S2 - Green
    scene.addObject(new Sphere(vec3(4, 0, -7), 1.0f, vec3(0, 0, 1)));        // S3 - Blue

    OutputImage.clear();
    for (int j = 0; j < Height; ++j) {
        for (int i = 0; i < Width; ++i) {
            Ray ray = camera.getRay(i, j);
            vec3 color = scene.trace(ray);
            OutputImage.push_back(color.r);
            OutputImage.push_back(color.g);
            OutputImage.push_back(color.b);
        }
    }
}

// -------------------------------------------------
// Resize Callback
// -------------------------------------------------
void resize_callback(GLFWwindow*, int nw, int nh)
{
    Width = nw;
    Height = nh;
    glViewport(0, 0, nw, nh);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (double)Width, 0.0, (double)Height, 1.0, -1.0);
    OutputImage.reserve(Width * Height * 3);
    render();
}

// -------------------------------------------------
// Main
// -------------------------------------------------
int main(int argc, char* argv[])
{
    GLFWwindow* window;
    if (!glfwInit()) return -1;

    window = glfwCreateWindow(Width, Height, "Ray Tracing", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glfwSetFramebufferSizeCallback(window, resize_callback);
    resize_callback(NULL, Width, Height);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawPixels(Width, Height, GL_RGB, GL_FLOAT, &OutputImage[0]);
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
