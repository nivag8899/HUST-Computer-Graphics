#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"

#include <iostream>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

GLuint vertex_array_object; // VAO句柄
GLuint vertex_buffer_object; // VBO句柄

//指定摄像机
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// 指定时间参数
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 指定光源位置
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

void initial(void)
{
	// 定义图形对象的顶点数据和纹理坐标
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};


	// 生成并绑定VAO和VBO
	glGenVertexArrays(1, &vertex_array_object);
	glBindVertexArray(vertex_array_object);

	glGenBuffers(1, &vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);

	// 将顶点数据绑定至当前默认的缓冲中
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	glEnable(GL_DEPTH_TEST);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}


void reshaper(GLFWwindow* window, int width, int height)
{
	// 指定当前视口尺寸(前两个参数为左下角位置，后两个参数是渲染窗口宽、高)
	glViewport(0, 0, width, height);

}

//鼠标滚轮回调函数
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//对摄像机进行操作
	camera.ProcessMouseScroll((float)yoffset);
}

// 鼠标移动回调函数
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; // y坐标系进行反转

	lastX = (float)xpos;
	lastY = (float)ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

int main()
{
	glfwInit(); // 初始化GLFW

	// OpenGL版本为3.3，主次版本号均设为3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// 使用核心模式(无需向后兼容性)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 创建窗口(宽、高、窗口名称)
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Base Lighting", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to Create OpenGL Context" << std::endl;
		glfwTerminate();
		return -1;
	}

	// 将窗口的上下文设置为当前线程的主上下文
	glfwMakeContextCurrent(window);

	// 初始化GLAD，加载OpenGL函数指针地址的函数
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	initial();

	//指定窗口大小改变时调用的函数reshaper
	glfwSetFramebufferSizeCallback(window, reshaper);

	//鼠标回调函数
	glfwSetCursorPosCallback(window, mouse_callback);
	//当鼠标滚轮滚动时调用函数scroll_callback
	glfwSetScrollCallback(window, scroll_callback);

	//让GLFW捕捉鼠标事件
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//指定顶点和片段着色器
	Shader lightingShader = Shader("shader/colors.vs", "shader/colors.fs");
	Shader lightCubeShader = Shader("shader/light_cube.vs", "shader/light_cube.fs");

	while (!glfwWindowShouldClose(window))
	{
		//处理时间
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 输入处理
		processInput(window);

		// 清空颜色缓冲
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//激活光照立方体并设置参数
		lightingShader.use();
		lightingShader.setVec3("light.position", lightPos);
		lightingShader.setVec3("viewPos", camera.Position);

		//光照属性
		glm::vec3 lightColor;
		lightColor.x = sin(glfwGetTime() * 1.6f);
		lightColor.y = sin(glfwGetTime() * 0.8f);
		lightColor.z = sin(glfwGetTime() * 0.1f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); 
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
		lightingShader.setVec3("light.ambient", ambientColor);
		lightingShader.setVec3("light.diffuse", diffuseColor);
		lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		//定义材质属性
		lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		lightingShader.setFloat("material.shininess", 32.0f);

		// 指定变换
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("projection", projection);//定义投影变换矩阵
		lightingShader.setMat4("view", view);		          //定义视图变换矩阵
		lightingShader.setMat4("model", model);            //定义模型变换矩阵


		// 绘制立方体
		glBindVertexArray(vertex_array_object);                                    // 绑定VAO
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//激活光源立方体并设置参数
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // 缩小的立方体
		lightCubeShader.setMat4("model", model);

		glBindVertexArray(vertex_array_object);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);                                                      // 解除绑定


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// 解绑VAO和VBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// 删除VAO和VBO
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteBuffers(1, &vertex_buffer_object);

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}





