#undef GLFW_DLL
#include <iostream>
#include <vector>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Libs/Shader.h"
#include "Libs/Window.h"
#include "Libs/Mesh.h"
#include "Libs/stb_image.h"

const GLint WIDTH = 800, HEIGHT = 600;

Window mainWindow;
std::vector<Mesh *> meshList;
std::vector<Mesh *> meshList2;
std::vector<Mesh *> meshList3;
std::vector<Mesh *> meshList4;
std::vector<Shader *> shaderList;

Mesh* light;
static const char* lightVShader = "Shaders/lightShader.vert";
static const char* lightFShader = "Shaders/lightShader.frag";

Mesh *obj2;
Mesh *obj3;
Mesh *obj4;

// Camera
glm::vec3 cameraPos(60.0f, 0.5f, 30.5f);
glm::vec3 cameraDirection(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight(1.0f, 0.0f, 0.0f);
float yaw =  -135.0f, pitch = 0.0f;

glm::vec3 lightColour = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 lightPos = glm::vec3(-10.0f, 0.0f, 10.0f);

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Shaders
static const char *vShader = "Shaders/shader.vert";
static const char *fShader = "Shaders/shader.frag";

void CreateTriangle()
{
    GLfloat vertices[] = {
        // pos              // tex
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 1.0f, 0.5f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.5f, 1.0f};
    unsigned int indices[] = {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2};

    Mesh *obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 20, 12); // 20 floats, 12 indices
    for (int i = 0; i < 1; i++)
        meshList.push_back(obj1);
}

void CreateShaders()
{
    Shader *shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(shader1);

    Shader *shader2 = new Shader();
    shader2->CreateFromFiles(lightVShader, lightFShader);
    shaderList.push_back(shader2);
}

void CreateOBJ()
{
    Mesh *obj1 = new Mesh();
    bool loaded = obj1->CreateMeshFromOBJ("Models/Jellyfish.obj");
    if (loaded)
    {
        for (int i = 0; i < 10; i++)
        {
            meshList.push_back(obj1);
        }
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
    }

    obj2 = new Mesh();
    loaded = obj2->CreateMeshFromOBJ("Models/Ground.obj");
    if (loaded)
    {
        for (int i = 0; i < 1; i++)
        {
            meshList2.push_back(obj2);
        }
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
    }

    obj3 = new Mesh();
    loaded = obj3->CreateMeshFromOBJ("Models/Coral_Small.obj");
    if (loaded)
    {
        for (int i = 0; i < 10; i++)
        {
            meshList3.push_back(obj3);
        }
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
    }

    obj4 = new Mesh();
    loaded = obj4->CreateMeshFromOBJ("Models/crystal.obj");
    if (loaded)
    {
        for (int i = 0; i < 10; i++)
        {
            meshList4.push_back(obj4);
        }
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
    }


    light = new Mesh();
    loaded = light->CreateMeshFromOBJ("Models/cube.obj");
    if (!loaded)
    {
        std::cout << "Failed to load model" << std::endl;
    }
   

    
}

// Mouse movement
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    static double lastX = WIDTH / 2.0, lastY = HEIGHT / 2.0;
    static bool firstMouse = true;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = (float)(xpos - lastX);
    float yoffset = (float)(lastY - ypos); // reversed
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 dir;
    dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.y = sin(glm::radians(pitch));
    dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraDirection = glm::normalize(dir);
}

void processInput(GLFWwindow *window)
{
    float speed = 15.0f * deltaTime;
    cameraRight = glm::normalize(glm::cross(cameraDirection, cameraUp));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraDirection * speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraDirection * speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= cameraRight * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += cameraRight * speed;
}

int main()
{
    glEnable(GL_DEPTH_TEST);
    mainWindow = Window(WIDTH, HEIGHT, 3, 3);
    mainWindow.initialise();

    glfwSetInputMode(mainWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(mainWindow.getWindow(), mouse_callback);

    CreateOBJ();
    CreateShaders();

    // Load texture
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char *data1 = stbi_load("Textures/Jellyfish.png", &width, &height, &nrChannels, 0);
    if (data1)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data1);
    }
    else
    {
        std::cerr << "Failed to load texture\n";
    }

    // Load texture
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    unsigned char *data2 = stbi_load("Textures/Ground.png", &width, &height, &nrChannels, 0);
    if (data2)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data2);
    }
    else
    {
        std::cerr << "Failed to load texture\n";
    }

    // Load texture
    unsigned int texture3;
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    unsigned char *data3 = stbi_load("Textures/Mushie.png", &width, &height, &nrChannels, 0);
    if (data3)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data3);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data3);
    }
    else
    {
        std::cerr << "Failed to load texture\n";
    }

        // Load texture
    unsigned int texture4;
    glGenTextures(1, &texture4);
    glBindTexture(GL_TEXTURE_2D, texture4);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char *data4 = stbi_load("Textures/Mushie_Crystal.png", &width, &height, &nrChannels, 0);
    if (data4)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data4);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data4);
    }
    else
    {
        std::cerr << "Failed to load texture\n";
    }


    // --- Jellyfish ---
    glm::vec3 JellyfishPositions[] = {
        {-30.0f, -6.5f, -25.0f},
        {-95.0f, -25.0f, -85.0f}, // Big one
        {-10.0f, -6.8f, -30.0f},
        {-25.0f, -6.3f, -55.0f},
        {-15.0f, -5.7f, -25.0f}
    };

    glm::vec3 JellyfishScales[] = {
        {1.5f, 1.5f, 1.5f},
        {4.0f, 4.0f, 4.0f},
        {1.3f, 1.3f, 1.3f},
        {1.8f, 1.8f, 1.8f},
        {1.0f, 1.0f, 1.0f}
    };

        glm::vec3 GroundPositions[] = {
        {-10.0f, -7.5f, -15.0f}};

        glm::vec3 GroundScale[] = {
        {1.0f, 1.0f, 1.0f}
        };

 glm::vec3 CoralPositions[] = {
    // --- Group 1: Main coral cluster (near camera) ---
    {-2.0f, -5.5f, -55.0f},   // Big central coral
    {9.0f, -7.5f, -18.0f},   // Slightly to the right
    {18.0f, -7.5f, -15.0f},   // Slightly to the left

    // --- Group 2: Mid cluster (center-left) ---
    {12.0f, -6.5f, -25.0f},  // Big coral
    {14.0f,  -7.5f, -30.0f},  // Right neighbor
    {22.0f, -5.5f, -20.5f},  // Left neighbor

    // --- Group 3: Scattered corals (background) ---
    {30.0f, -5.5f, -20.0f},   // Far right
    {-20.0f, -5.5f, -20.0f},  // Far left
    {5.0f,  -5.5f, -20.0f}    // Distant single coral
};

glm::vec3 CoralScales[] = {
    // Group 1 (bigger cluster)
    {8.0f, 8.0f, 8.0f},       // Big main coral
    {3.0f, 3.0f, 3.0f},       // Medium
    {1.5f, 1.5f, 1.5f},       // Small

    // Group 2 (mid cluster)
    {3.0f, 5.0f, 4.0f},       // Big
    {2.5f, 4.0f, 4.0f},       // Medium
    {1.0f, 3.0f, 3.0f},       // Small

    // Group 3 (scattered)
    {5.5f, 5.5f, 5.5f},       // Large, far back
    {3.5f, 4.5f, 4.5f},       // Medium
    {1.5f, 3.5f, 3.5f}        // Small
};

 glm::vec3 CrystalPositions[] = {
    // --- Group 1: Main coral cluster (near camera) ---
    {10.0f, -7.5f, -30.0f},
    {3.0f, -7.5f, -10.0f},
    {6.0f, -7.5f, -5.0f},
    {15.0f, -7.5f, -22.0f},    
};

glm::vec3 CrystalScale[] = {
    // --- Group 1: Main coral cluster (near camera) ---
    {5.0f, 5.5f, 5.0f},   // Big central coral
    {4.0f, 4.5f, 4.0f},
    {3.0f, 3.0f, 3.0f},
    {3.0f, 3.0f, 3.0f},

};     

    while (!mainWindow.getShouldClose())
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        processInput(mainWindow.getWindow());

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderList[0]->UseShader();
        GLuint uniformModel = shaderList[0]->GetUniformLocation("model");
        GLuint uniformView = shaderList[0]->GetUniformLocation("view");
        GLuint uniformProjection = shaderList[0]->GetUniformLocation("projection");
        GLuint uniformTexture = shaderList[0]->GetUniformLocation("texture1");
        glUniform1i(uniformTexture, 0);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                                (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 500.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);

        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        
        // Light
        glUniform3fv(shaderList[0]->GetUniformLocation("lightColour"), 1, glm::value_ptr(lightColour));
        glUniform3fv(shaderList[0]->GetUniformLocation("lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(shaderList[0]->GetUniformLocation("viewPos"), 1, glm::value_ptr(cameraPos));

        lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
        lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;
        lightPos.z = 0.0f;

        //Jellyfish
        for (int i = 0; i < 5; i++)
        {
            glm::mat4 model(1.0f);
            model = glm::translate(model, JellyfishPositions[i]);
            // model = glm::rotate(model,glm::radians(2.0f*i),glm::vec3(1.0f,0.3f,0.5f));
            // model = glm::scale(model, glm::vec3(0.8f));
            model = glm::scale(model, JellyfishScales[i]);
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture1);


            meshList[i]->RenderMesh();
        }

        //Ground
        for (int i = 0; i < 1; i++)
        {
            glm::mat4 model(1.0f);
            model = glm::translate(model, GroundPositions[i]);
            // model = glm::rotate(model,glm::radians(2.0f*i),glm::vec3(1.0f,0.3f,0.5f));
            model = glm::scale(model, GroundScale[i]);
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture2);


            meshList2[i]->RenderMesh();
        }

        //Coral
        for (int i = 0; i < 5; i++)
        {
            glm::mat4 model(1.0f);
            model = glm::translate(model, CoralPositions[i]);
            // model = glm::rotate(model,glm::radians(2.0f*i),glm::vec3(1.0f,0.3f,0.5f));
            // model = glm::scale(model, glm::vec3(0.8f));
            model = glm::scale(model, CoralScales[i]);
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture3);


            meshList3[i]->RenderMesh();
        }

        // Crystal
        for (int i = 0; i < 4; i++)
        {
            glm::mat4 model(1.0f);
            model = glm::translate(model, CrystalPositions[i]);
            // model = glm::rotate(model,glm::radians(2.0f*i),glm::vec3(1.0f,0.3f,0.5f));
            // model = glm::scale(model, glm::vec3(0.8f));
            model = glm::scale(model, CrystalScale[i]);
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture4);


            meshList4[i]->RenderMesh();
        }

        // {
        //     std::cout << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << "\n" << "direction : " << cameraDirection.x << " "<< cameraDirection.y<< " "<< cameraDirection.z<<" "<<"\n";

        // }



        // Draw light mesh
        shaderList[1]->UseShader();
        uniformModel = shaderList[1]->GetUniformLocation("model");
        uniformView = shaderList[1]->GetUniformLocation("view");
        uniformProjection = shaderList[1]->GetUniformLocation("projection");

        glUniform3fv(shaderList[1]->GetUniformLocation("lightColour"), 1, glm::value_ptr(lightColour));
        light->RenderMesh();
        glm::mat4 model(1.0f);

        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // smaller cube

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

        glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}