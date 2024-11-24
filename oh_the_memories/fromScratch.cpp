#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#define SPEED 0.015

const unsigned int SCR_WIDTH = 640; 
const unsigned int SCR_HEIGHT = 480;

// Structure to store and output figure arrays
struct Figure {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

// initialize the openGL window: specify the GLFW & glad versions and viewpoint
GLFWwindow* initialization(int width, int height) {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL Window", NULL, NULL);
    if (!window) {
        std::cout << "ERROR: Failed window initialization" << std::endl;
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "ERROR: Failed to load glad" << std::endl;
        glfwTerminate();
        return NULL;
    }

    glViewport(0,0, width, height);

    return window;
}

// adjust window size: when the window is resized, edit the window parameters
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// process user input: query GLFW if a key is pressed on the current frame
void processInput(GLFWwindow *window, float *x_direction, float *y_direction) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        *y_direction += SPEED;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        *y_direction -= SPEED;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        *x_direction += SPEED;    
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        *x_direction -= SPEED;    
}

void shaderErrLogger(unsigned int shaderType, const char* name) {
    int status;
    char errLog[1024];

    if (name == "shader") {
        glGetShaderiv(shaderType, GL_COMPILE_STATUS, &status);

        if (!status) {
            glGetShaderInfoLog(shaderType, 1024, NULL, errLog);
            std::cout << "ERROR: shader compilation unsuccessful\n" <<
                errLog << std::endl;
        }
    }
    else if (name == "program") {
        glGetProgramiv(shaderType, GL_LINK_STATUS, &status);

        if (!status) {
            glGetProgramInfoLog(shaderType, 1024, NULL, errLog);
            std::cout << "ERROR: shader link unsuccessful\n" <<
                errLog << std::endl;
        }
    }
}

// complete shader generation: generate a program from vertex and fragment shaders
// both of these shaders are minimal requirements for the program
unsigned int programGeneration() {
    // vertex shader pipeline: calculate the position of vertices
    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 vertexPos;\n"
        "uniform float x_dir;\n"
        "uniform float y_dir;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(vertexPos.x + x_dir, vertexPos.y + y_dir, vertexPos.z, 1.0);\n"
        "}\0";
    
    // fragment shader pipeline: calculate the output color
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.6f, 0.1f, 1.0f);\n"
        "}\0";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    shaderErrLogger(vertexShader, "shader");

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    shaderErrLogger(fragmentShader, "shader");

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    shaderErrLogger(program, "program");

    // deprecated shader clease: delete the linked shaders for realocation
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

Figure letterN() {
    Figure n;
    n.vertices = {
        // Positions        // Texture Coords
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // Bottom left
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, // Top left
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // Top right
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Bottom right
         0.0f,  0.0f, 0.0f, 0.5f, 0.5f  // Diagonal point
    };

    n.indices = {
        0, 1, 4, // Left triangle
        1, 2, 4, // Top triangle
        4, 3, 0  // Bottom triangle
    };

    return n;
}

Figure decagonFig() {
    Figure decagon;
    decagon.vertices = {
        // Center point for triangles
         0.0f, 0.0f, 0.0f,
        // Vertices of the decagon
        0.500, 0.000, 0.0,
        0.405, 0.294, 0.0,
        0.155, 0.476, 0.0,
        -0.155, 0.476, 0.0,
        -0.405, 0.294, 0.0,
        -0.500, 0.000, 0.0,
        -0.405, -0.294, 0.0,
        -0.155, -0.476, 0.0,
        0.155, -0.476, 0.0,
        0.405, -0.294, 0.0
    };

    decagon.indices = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
        0, 5, 6,
        0, 6, 7,
        0, 7, 8,
        0, 8, 9,
        0, 9, 10,
        0, 10, 1
    };

    return decagon;
}

int main() {
    GLFWwindow* window = initialization(SCR_WIDTH, SCR_HEIGHT);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    // generate vertex & fragment shaders, combine into a complete shader
    unsigned int shader = programGeneration();

    // initialize figures: letter N and letter K
    Figure n = letterN();
    Figure decagon = decagonFig();

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);  
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, decagon.vertices.size() * sizeof(float), 
        decagon.vertices.data(), GL_DYNAMIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, decagon.indices.size() * sizeof(unsigned int), 
        decagon.indices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(shader);

    float x_dir = 0.0f, y_dir = 0.0f;

    // main loop: take action until the window is terminated
    while (!glfwWindowShouldClose(window)) {
        // user input
        processInput(window, &x_dir, &y_dir);

        // frame generation: generate the colored frame after frame clear
        glClearColor(0.05f, 0.008f, 0.004f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // check for the screen size change
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        glUseProgram(shader);
        glUniform1f(glGetUniformLocation(shader, "x_dir"), x_dir);
        glUniform1f(glGetUniformLocation(shader, "y_dir"), y_dir);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, decagon.indices.size(), GL_UNSIGNED_INT, 0);

        // register events: button click, mouse drag etc.
        glfwPollEvents();
        // frame buffering: swap finished frame to process the next
        glfwSwapBuffers(window);
    }
    
    // buffer cleanse: delete deprecated buffers before termination
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    // shader cleanse: delete the program/shader before termination
    glDeleteProgram(shader);
    // program is terminated: program resources are freed and realocated
    glfwTerminate();
    return 0;
}