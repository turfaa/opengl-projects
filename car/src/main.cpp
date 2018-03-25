#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <commons/shader.h>
#include <commons/drawer.h>

using namespace std;

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000
#define SIDES_OF_CIRCLE 100
#define VERTEX_SHADER_FILE_PATH  "car/shaders/vertex_shader.glsl"
#define FRAGMENT_SHADER_FILE_PATH "car/shaders/fragment_shader.glsl"

void error_callback(int error, const char * description) {
    cout << description << endl;
}

int main() {
    glfwSetErrorCallback(error_callback);
    if(!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Car", nullptr, nullptr);
    if (window == nullptr) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = static_cast<GLboolean>(true);
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    GLuint shader_program = LoadShaders(VERTEX_SHADER_FILE_PATH, FRAGMENT_SHADER_FILE_PATH);

    vector<int> start_of_section;

    Vertices vertices = createCircle(0, 0, 1, 0, 0, 0.3, SIDES_OF_CIRCLE);
    start_of_section.push_back(0);
    int num_of_points = SIDES_OF_CIRCLE + 2;

    vertices = combineVertices(vertices, num_of_points,
                               createTriangle(
                                    static_cast<GLfloat>(-0.3), static_cast<GLfloat>(-0.35),
                                    0.3, static_cast<GLfloat>(-0.35),
                                    0.3, 0.15,

                                    1, 1, 1),
                               3);
    start_of_section.push_back(num_of_points);
    num_of_points += 3;

    GLuint vertex_buffer;

    glGenBuffers(1, &vertex_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 5 * num_of_points, vertices, GL_STATIC_DRAW);

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    GLint posAttrib = glGetAttribLocation(shader_program, "position");
    glEnableVertexAttribArray(static_cast<GLuint>(posAttrib));
    glVertexAttribPointer(static_cast<GLuint>(posAttrib), 2, GL_FLOAT, GL_FALSE,
                          5*sizeof(float), 0);

    auto colAttrib = glGetAttribLocation(shader_program, "in_color");
    glEnableVertexAttribArray(static_cast<GLuint>(colAttrib));
    glVertexAttribPointer(static_cast<GLuint>(colAttrib), 3, GL_FLOAT, GL_FALSE,
                          5*sizeof(float), (void*)(2*sizeof(float)));

    glBindVertexArray(0);

    do {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);

        glBindVertexArray(vertex_array);

        glDrawArrays(GL_TRIANGLE_FAN, start_of_section[0], SIDES_OF_CIRCLE + 2);
        glDrawArrays(GL_TRIANGLES, start_of_section[1], 3);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while( glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    glDeleteBuffers(1, &vertex_buffer);
    glDeleteVertexArrays(1, &vertex_array);
    glDeleteProgram(shader_program);

    glfwTerminate();
    return 0;
}