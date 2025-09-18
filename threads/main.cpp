#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

// Config
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Câmera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.f, pitch = 0.f;
float lastX = SCR_WIDTH/2.0f, lastY = SCR_HEIGHT/2.0f;
bool firstMouse = true;
float deltaTime = 0.f, lastFrame = 0.f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height){ glViewport(0,0,width,height); }
void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(firstMouse){ lastX=xpos; lastY=ypos; firstMouse=false; }
    float xoffset=xpos-lastX, yoffset=lastY-ypos;
    lastX=xpos; lastY=ypos;
    float sensitivity=0.1f;
    xoffset*=sensitivity; yoffset*=sensitivity;
    yaw+=xoffset; pitch+=yoffset;
    if(pitch>89.f) pitch=89.f; if(pitch<-89.f) pitch=-89.f;
    glm::vec3 front;
    front.x = cos(glm::radians(yaw))*cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw))*cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}
void processInput(GLFWwindow* window){
    float speed = 2.5f * deltaTime;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += speed * cameraFront;
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= speed * cameraFront;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= glm::normalize(glm::cross(cameraFront,cameraUp)) * speed;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += glm::normalize(glm::cross(cameraFront,cameraUp)) * speed;
}

// Shader simples (inline)
const char* vertexShaderSource = R"(
#version 330 core
layout(location=0) in vec3 aPos;
uniform mat4 view;
uniform mat4 projection;
void main(){ gl_Position = projection * view * vec4(aPos,1.0); }
)";
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main(){ FragColor = vec4(1.0,1.0,1.0,1.0); }
)";

unsigned int compileShader(unsigned int type,const char* source){
    unsigned int shader=glCreateShader(type);
    glShaderSource(shader,1,&source,nullptr);
    glCompileShader(shader);
    int success; char info[512];
    glGetShaderiv(shader,GL_COMPILE_STATUS,&success);
    if(!success){ glGetShaderInfoLog(shader,512,nullptr,info); std::cerr<<info<<std::endl; }
    return shader;
}

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT,"3D Mesh Navigation",nullptr,nullptr);
    if(!window){ std::cerr<<"Window failed\n"; return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
    glfwSetCursorPosCallback(window,mouse_callback);
    glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){ std::cerr<<"Failed GLAD\n"; return -1; }
    glEnable(GL_DEPTH_TEST);

    // Shader program
    unsigned int vertexShader=compileShader(GL_VERTEX_SHADER,vertexShaderSource);
    unsigned int fragmentShader=compileShader(GL_FRAGMENT_SHADER,fragmentShaderSource);
    unsigned int shaderProgram=glCreateProgram();
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Malha simples (vértices de um cubo wireframe)
    float vertices[] = {
        -1,-1,-1,  1,-1,-1,  1,1,-1,  -1,1,-1, // back face
        -1,-1,1,   1,-1,1,   1,1,1,   -1,1,1   // front face
    };
    unsigned int indices[] = {
        0,1, 1,2, 2,3, 3,0, // back
        4,5, 5,6, 6,7, 7,4, // front
        0,4, 1,5, 2,6, 3,7  // sides
    };

    unsigned int VAO,VBO,EBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    while(!glfwWindowShouldClose(window)){
        float currentFrame = glfwGetTime();
        deltaTime=currentFrame-lastFrame;
        lastFrame=currentFrame;

        processInput(window);
        glClearColor(0.1f,0.1f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glm::mat4 view = glm::lookAt(cameraPos,cameraPos+cameraFront,cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.f),(float)SCR_WIDTH/(float)SCR_HEIGHT,0.1f,100.f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"view"),1,GL_FALSE,glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"projection"),1,GL_FALSE,glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawElements(GL_LINES,24,GL_UNSIGNED_INT,0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
    glDeleteBuffers(1,&EBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
