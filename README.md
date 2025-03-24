# cg_code
# Ray Tracing Assignment

## ✅ Description
A basic ray tracer using OpenGL and C++ (Visual Studio).

## 🛠️ Compilation & Run Instructions

1. Open `RayTracingProject.sln` with Visual Studio.
2. Build the solution (`Ctrl+Shift+B`)
3. Run the program (`F5`)
4. Make sure to install GLEW and GLFW if not installed.

## 📦 Requirements
- Visual Studio 2019 or later
- GLEW
- GLFW
- GLM

## 🖼️ Screenshots
See `/Result.png`



## 🧩 Code Structure

The project is structured into the following components:

- **Ray**: Represents a ray with an origin and direction.
- **Surface (abstract)**: Base class for all renderable objects (Sphere, Plane).
- **Sphere**: Inherits from Surface. Implements ray-sphere intersection using the **geometric method** learned in class, which calculates the closest approach from the ray to the sphere center and uses the discriminant to check for intersection.
- **Plane**: Inherits from Surface. Checks ray-plane intersection using normal and offset.
- **Camera**: Generates rays from the eye point through the image plane for each pixel.
- **Scene**: Holds all objects in the scene and traces rays to find the closest intersection.
- **render()**: Core rendering loop. For each pixel, casts a ray using the camera and shades the pixel based on the intersected object.
- **main()**: Initializes the OpenGL window, sets up callbacks, and launches the render loop.

