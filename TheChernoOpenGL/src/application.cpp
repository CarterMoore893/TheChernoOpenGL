#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

static int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();   // OpenGL expects a 'raw' string, so we are going to 
                                        // assign the pointer of the std::string to `char* src
    glShaderSource(id, 1, &src, nullptr); // Be sure to check this out on <docs.gl>
    glCompileShader(id);

    // Error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);  // GL get shader 'eye-vee
    // i: we want an integer, v: we want an array (a ptr in this case)
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));   // A hackey way for us to dynamically allocate space on the stack
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "(EE) Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader:" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        
        return 0;
    }

    return id;
}

/* Write the code to complie the vertex shader and the fragment shader
 * There are many different ways for us source our shaders. We could write a string in C++,
 * we can fetch the string from a file, fetch it online, etc.
 * Today, we'll just be writing it in ourselves                                         */
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    // Note the inconsistent API usage. glCreateProgram returns an 
    // unsigned int while we passed in a buffer for glGenBuffer()
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    // Okay, so now we have a program, and our two necessary shaders compiled.
    // Let's attach the shaders to our program
    // Imagine that the rest of this function operate similarly to the compilation pipeline
    glAttachShader(program, vs);        // Our compiled shaders are associated with our program
    glAttachShader(program, fs);

    glLinkProgram(program);             // The program and shaders are then fully linked
    glValidateProgram(program);         // and validated

    glDeleteShader(vs);                 // We no longer need the compiled shaders
    glDeleteShader(fs);                 // as they now exist within our program

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glewInit();

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glewInit();

    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[6] = {
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f
    };

    // Create our first vertex buffer
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    // Select, or *bind* the buffer we created
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // Fill the bound buffer with data: six float datapoints, from positions, to be infrequently used, and drawn to the screen
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    // Friendly reminder that OpenGL is a state machine. The following code can be ordered in any which way
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    /* Shader code, and shader function call
     * Note that in C++ you can concatenate strings by placing quotes one after
     * another as seen below                                                    */
    std::string vertexShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = position;\n"
        "}\n";
    
    std::string fragmentShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers. This prevents flickering */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}