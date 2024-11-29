#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

const unsigned int SCR_WIDTH = 640;
const unsigned int SCR_HEIGHT = 480;

glm::vec3 translation(0.0f, 0.0f, 0.0f); // movement speed object one
glm::vec3 translation2(0.0f, 0.0f, 0.0f); // movement speed object two

float rotationSpeed = 0.0f; // rotation speed object one
float rotationSpeed2 = 0.0f; // rotation speed object two

float scaleSpeed = 0.0f; // scale speed (factor) object one
float scaleSpeed2 = 0.0f; // scale speed (factor) object two

bool isDragging = false; // Is the mouse currently dragging?
int draggedObject = -1; // -1: none, 0: object one, 1: object two
double lastX, lastY;

// structure to store and output figure vectors
struct Figure {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

struct ObjectData {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
};

// vertex shader pipeline: calculate the position of vertices
const char *vertexShaderSource = "#version 330 core\n"
    "uniform mat4 transform;"
    "layout (location = 0) in vec3 vertexPos;\n"
    "layout (location = 1) in vec3 vertexColor;\n"
    "out vec3 fragmentColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = transform * vec4(vertexPos, 1.0f);\n"
    "   fragmentColor = vertexColor;\n"
    "}\0";

// fragment shader pipeline: calculate the output color
const char* fragmentShaderSource = "#version 330 core\n"
    "in vec3 fragmentColor;\n"
    "out vec4 finalColor;\n"
    "void main()\n"
    "{\n"
    "   finalColor = vec4 (fragmentColor, 1.0);\n"
    "}\0";

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
void processInput(GLFWwindow *window) {
    // exit the program on "escape" press
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

        // OBJECT ONE
    // translation (Press "up" to move up, "down" to move down, "left" to move left, 
    // and "right" to move right)
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        translation.y += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) 
        translation.y -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        translation.x -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        translation.x += 0.01f;

    // rotation (Press "r")
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        rotationSpeed += 0.01f;

    // scaling (Press "," to scale down, "." to scale up)
    if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS)
        scaleSpeed -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS)
        scaleSpeed += 0.01f;

        // OBJECT TWO
    // translation (Press "W" to move up, "S" to move down, "A" to move left, 
    // and "D" to move right)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        translation2.y += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        translation2.y -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        translation2.x -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        translation2.x += 0.01f;

    // rotation (Press "t")
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        rotationSpeed2 += 0.01f;

    // scaling (Press "[" to scale down, "]" to scale up)
    if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
        scaleSpeed2 -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
        scaleSpeed2 += 0.01f;
}

// mouse button function: tracks mouse click and release
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            // mouse press: store the mouse position and determine which object is being dragged
            isDragging = true;
            glfwGetCursorPos(window, &lastX, &lastY);

            // Check if the mouse is over one of the objects
            if (lastX < SCR_WIDTH / 2) {
                draggedObject = 0;  // Object 1
            } else {
                draggedObject = 1;  // Object 2
            }
        } 
        else if (action == GLFW_RELEASE) {
            // mouse release: stop dragging
            isDragging = false;
            draggedObject = -1;
        }
    }
}

// mouse position function: tracks mouse movement during drag
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos) {
    if (isDragging) {
        // position calculation: how much the mouse has moved since the last position
        double deltaX = xpos - lastX;
        double deltaY = ypos - lastY;

        // update the last mouse position
        lastX = xpos;
        lastY = ypos;

        // move the selected object
        if (draggedObject == 0) {
            // update translation for object 1 (decagon)
            translation.x += deltaX / SCR_WIDTH * 2.0f;  // Scale the movement by screen width
            translation.y -= deltaY / SCR_HEIGHT * 2.0f;  // Scale the movement by screen height
        }
        else if (draggedObject == 1) {
            // update translation for object 2 (house)
            translation2.x += deltaX / SCR_WIDTH * 2.0f;
            translation2.y -= deltaY / SCR_HEIGHT * 2.0f;
        }
    }
}

// mouse wheel function: tracks mouse wheel scrolling
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset > 0) {
        // scrolled up: increase scale speed
        scaleSpeed += 0.05f;
        scaleSpeed2 += 0.05f;
    } else {
        // scrolled down: decrease scale speed
        scaleSpeed -= 0.05f;
        scaleSpeed2 -= 0.05f;
    }

    // artificial borders: scaleSpeed doesn't become negative or too large
    if (scaleSpeed < -0.5f) scaleSpeed = -0.5f;  // Min scale factor
    if (scaleSpeed > 2.0f) scaleSpeed = 2.0f;   // Max scale factor
}

// error generation for wrong shader compilation
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

Figure decagonFig() {
    Figure decagon;
    decagon.vertices = {
        // center point of decagon
         0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        // vertices of the decagon
        // positions       // colors
        0.500, 0.000, 0.0,   1.0f, 0.0f, 0.0f,
        0.405, 0.294, 0.0,   1.0f, 0.5f, 0.0f,
        0.155, 0.476, 0.0,   1.0f, 1.0f, 0.0f,
        -0.155, 0.476, 0.0,  0.5f, 1.0f, 0.0f,
        -0.405, 0.294, 0.0,  0.0f, 1.0f, 0.0f,
        -0.500, 0.000, 0.0,  0.0f, 1.0f, 0.5f,
        -0.405, -0.294, 0.0, 0.0f, 0.5f, 1.0f,
        -0.155, -0.476, 0.0, 0.0f, 0.0f, 1.0f,
        0.155, -0.476, 0.0,  0.5f, 0.0f, 1.0f,
        0.405, -0.294, 0.0,  1.0f, 0.0f, 1.0f
    };

    // decagon will be created from triangles
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

Figure houseFig() {
    Figure house;
    house.vertices = {
         // positions       // colors
         0.5f,  0.5f, 0.0f, 1.0f, 0.5f, 0.2f,  // top right
         0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.2f,  // bottom right
        -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.2f,  // bottom left
        -0.5f,  0.5f, 0.0f, 1.0f, 0.5f, 0.2f,  // top left
        0.0f,  0.75f, 0.0f, 1.0f, 0.5f, 0.2f  // roof
    };

    house.indices = {
        0, 1, 3,
        1, 2, 3,
        0, 3, 4
    };

    return house;
}

ObjectData createFigureObject(Figure fig) {
    ObjectData objectData;
    
    glGenVertexArrays(1, &objectData.VAO);  
    glGenBuffers(1, &objectData.VBO);
    glBindVertexArray(objectData.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, objectData.VBO);
    glBufferData(GL_ARRAY_BUFFER, fig.vertices.size() * sizeof(float), 
        fig.vertices.data(), GL_DYNAMIC_DRAW);

    glGenBuffers(1, &objectData.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objectData.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, fig.indices.size() * sizeof(unsigned int), 
        fig.indices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return objectData;
}

int main() {
    GLFWwindow* window = initialization(SCR_WIDTH, SCR_HEIGHT);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    // generate vertex & fragment shaders, combine into a complete shader
    unsigned int shader = programGeneration();

    // initialize figures
    Figure decagon = decagonFig();
    ObjectData decagonData = createFigureObject(decagon);

    Figure house = houseFig();
    ObjectData houseData = createFigureObject(house);

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_move_callback);
    // Set the scroll callback function
    glfwSetScrollCallback(window, scroll_callback);

    // main loop: take action until the window is terminated
    while (!glfwWindowShouldClose(window)) {
        // user input
        processInput(window);

        // frame generation: generate the colored frame after frame clear
        glClearColor(0.05f, 0.008f, 0.004f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // check for the screen size change
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        glUseProgram(shader);

        // updating the matrices: update transformation matrices of object one per each frame
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, 0.0f));
        transform = glm::translate(transform, translation);
        transform = glm::rotate(transform, rotationSpeed, glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, glm::vec3(1.0f + scaleSpeed, 1.0f + scaleSpeed, 1.0f));
        
        // pass the transformation matrix to the shader
        unsigned int transformLoc = glGetUniformLocation(shader, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &transform[0][0]);

        // drawing the first object: decagon
        glBindVertexArray(decagonData.VAO);
        glDrawElements(GL_TRIANGLES, decagon.indices.size(), GL_UNSIGNED_INT, 0);

        // updating the matrices: update transformation matrices of object two per each frame
        glm::mat4 transform2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.0f));;
        transform2 = glm::translate(transform2, translation2);
        transform2 = glm::rotate(transform2, rotationSpeed2, glm::vec3(0.0f, 0.0f, 1.0f));
        transform2 = glm::scale(transform2, glm::vec3(1.0f + scaleSpeed2, 1.0f + scaleSpeed2, 1.0f));

        // pass the transformation matrix to the shader
        unsigned int transform2Loc = glGetUniformLocation(shader, "transform");
        glUniformMatrix4fv(transform2Loc, 1, GL_FALSE, &transform2[0][0]);

        // drawing the second object: house
        glBindVertexArray(houseData.VAO);
        glDrawElements(GL_TRIANGLES, house.indices.size(), GL_UNSIGNED_INT, 0);

        // register events: button click, mouse drag etc.
        glfwPollEvents();
        // frame buffering: swap finished frame to process the next
        glfwSwapBuffers(window);
    }
    // buffer cleanse: delete deprecated buffers before termination
    glDeleteVertexArrays(1, &decagonData.VAO);
    glDeleteBuffers(1, &decagonData.VBO);
    glDeleteBuffers(1, &decagonData.EBO);
    // shader cleanse: delete the program/shader before termination
    glDeleteProgram(shader);
    // program is terminated: program resources are freed and realocated
    glfwTerminate();
    return 0;
}