## Hello Triangle

The process of transforming 3D coordinates to 2D pixels is managed by the **graphic pipeline** of OpenGL.

Vertex DATA -> Vertex Shader -> Geometry Shader -> Shape Assembly -> Test and Blending
-> Fragment Shader -> Rasterization

g++ main.cpp -o demo -lglfw -lGL && ./demo

g++ -std=c++17 main.cpp  -o demo -lglfw -lGL -ldl


***1. vertices [1]***
use: supper basic
***2. vertices [2]***
use:
```cpp
glBindBuffer(GL_ARRAY_BUFFER, 0); // if have more 1 VBO // unbind
glBindVertexArray(0);// if have more 1 VBO // unbind

glBindVertexArray(VAO[1]) before draw
```

***3. Only one vertices [1] but have 2 triangle***
use:
```cpp
vertices [] = {
    //posA  //posB
    0,0,    0,0,
    0,1,    0,-1,
    1,0,    -1,0
}

    glVertexAttribPointer(0,        // location
                        2,        // vec2: x y
                        GL_FLOAT, // type of vertices
                        GL_FALSE,
                        4 * sizeof(float), // mod: 4vertices //
                        (void *)0);// index 0

    glVertexAttribPointer(0,        // location
                        2,        // vec2: x y
                        GL_FLOAT, // type of vertices
                        GL_FALSE,
                        4 * sizeof(float), // mod: 4vertices //
                        (void*) 2 * sizeof(float); // index Position B
    glBindVertexArray(VAO[0]) //before draw
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(VAO[1]) //before draw
    glDrawArrays(GL_TRIANGLES, 0, 3);
```

***4. SAVE DATA: vertices [1] contain vertex of all triangle***
```cpp
float vertices[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left 
};
unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};
unsigned int EBO;
glGenBuffers(1, &EBO);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
// draw
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
```

