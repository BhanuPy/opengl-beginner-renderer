// ===============================
// OpenGL function loader
// ===============================
#include <glad/glad.h>

// ===============================
// Windowing + input + context
// ===============================
#include <GLFW/glfw3.h>

#include <iostream>

// ===============================
// Forward declarations
// ===============================
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// ===============================
// Window settings
// ===============================
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// ===============================
// GPU SHADERS (run on GPU)
// ===============================

// Vertex Shader
// - Receives vertex position
// - Outputs clip-space position
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    // OpenGL clip space is [-1, +1]
    gl_Position = vec4(aPos, 1.0);
}
)";

// Fragment Shader
// - Runs per pixel
// - Outputs final color
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.5, 0.6, 1.0);
}
)";

int main()
{
    // ===============================
    // 1. Initialize GLFW
    // ===============================
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // ===============================
    // 2. Create window + context
    // ===============================
    GLFWwindow* window = glfwCreateWindow(
        SCR_WIDTH,
        SCR_HEIGHT,
        "OpenGL Beginner Renderer : My OpenGL RECTANGLE using indexed vertices",
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ===============================
    // 3. Load OpenGL functions (GLAD)
    // ===============================
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    // ===============================
    // 4. Compile Shaders
    // ===============================
    int success;
    char infoLog[512];

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "Vertex Shader Error:\n" << infoLog << "\n";
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "Fragment Shader Error:\n" << infoLog << "\n";
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "Shader Link Error:\n" << infoLog << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ===============================
    // 5. Vertex Data (CPU)
    // ===============================
    float vertices[] =
    {
         0.5f,  0.5f,  0.0f,   // top right
         0.5f, -0.5f,  0.0f,  // bottom right
        -0.5f, -0.5f,  0.0f,  //bottom left
        -0.5f,  0.5f,  0.0f   //top left


    };

    unsigned int indices[] = { //note that we start from 0

        0,1,3,  //first triangle
        1,2,3   //second triangle
    };

    // ====================================
    // 6. VAO + VBO (GPU) + EBO (indexing)
    // ====================================

    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's
    //bound vertex buffer object so afterwards we can safely unbind.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //We can unbind the VAO afterwards so ther VAO calls won't accidentally modify this VAO, but this rarely happens.
    //Modifying other VAOs requires a call to glBindVertexArrays anyways so we generally dont unbind 
    //VA0s (nor VBOs) when its not directly necessary.
    glBindVertexArray(0);

    // ===============================
    // 7. Render Loop
    // ===============================
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ===============================
    // 8. Cleanup
    // ===============================
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

// ===============================
// Input handling
// ===============================
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// ===============================
// Resize callback
// ===============================
void framebuffer_size_callback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}
