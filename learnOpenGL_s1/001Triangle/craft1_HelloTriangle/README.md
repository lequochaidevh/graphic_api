## Hello Triangle

The process of transforming 3D coordinates to 2D pixels is managed by the **graphic pipeline** of OpenGL.

Vertex DATA -> Vertex Shader -> Geometry Shader -> Shape Assembly -> Test and Blending
-> Fragment Shader -> Rasterization

g++ main.cpp -o demo -lglfw -lGL && ./demo

g++ -std=c++17 main.cpp  -o demo -lglfw -lGL -ldl