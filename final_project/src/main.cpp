
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vmath.h>
#include <vector>
#include <Windows.h>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// 日地月系统的日期
static GLfloat day = 0.0;

// 窗口尺寸参数
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 900;

// 投影矩阵参数
const float fovy = 60;
float aspact = (float)SCR_WIDTH / (float)SCR_HEIGHT;
const float znear = 1;
const float zfar = 800;

// 句柄参数
GLuint vertex_array_object;		// VAO句柄
GLuint vertex_buffer_object;	// VBO句柄
GLuint element_buffer_object;	// EBO句柄
GLuint texture_buffer_object_sun;	// 太阳纹理对象句柄
GLuint texture_buffer_object_earth;	// 地球纹理对象句柄
GLuint texture_buffer_object_moon;	// 月球纹理对象句柄
int shader_program_tex;				// 纹理着色器程序句柄
int shader_program_light;          //太阳光源着色器程序句柄

// 球面顶点数据
std::vector<float> sphereVertices;
std::vector<int> sphereIndices;
const int Y_SEGMENTS = 50;
const int X_SEGMENTS = 50;
const float Radio = 2.0;
const GLfloat  PI = 3.14159265358979323846f;


GLuint lightVertexArrayObject;
GLuint lightVertexBufferObject;

// 创建光源几何体的函数
void createLightGeometry() {
	// 光源的顶点数据，表示一个简单的立方体
	float lightVertices[] = {
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

	// 生成并绑定 VAO
	glGenVertexArrays(1, &lightVertexArrayObject);
	glBindVertexArray(lightVertexArrayObject);

	// 生成并绑定 VBO
	glGenBuffers(1, &lightVertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, lightVertexBufferObject);

	// 将光源的顶点数据绑定到 VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), lightVertices, GL_STATIC_DRAW);

	// 设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// 解绑 VAO
	glBindVertexArray(0);
}


// 生成球的顶点和纹理顶点
void generateBallVerticles(std::vector<float>& sphereVertices) {
	for (int y = 0; y <= Y_SEGMENTS; y++)
	{
		for (int x = 0; x <= X_SEGMENTS; x++)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = std::cos(xSegment * Radio * PI) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = std::sin(xSegment * Radio * PI) * std::sin(ySegment * PI);
			// 球的顶点
			sphereVertices.push_back(xPos);
			sphereVertices.push_back(yPos);
			sphereVertices.push_back(zPos);
			// 纹理顶点，映射到经纬
			sphereVertices.push_back(xSegment);
			sphereVertices.push_back(ySegment);
		}
	}
}

// 生成球的顶点索引
void generateBallIndices(std::vector<int>& sphereIndices) {
	for (int i = 0; i < Y_SEGMENTS; i++)
	{
		for (int j = 0; j < X_SEGMENTS; j++)
		{
			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);

			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1);
		}
	}
}

// 创建纹理对象并加载纹理
void loadTexture(GLuint& texture_buffer_object, const char* filename) {
	glGenTextures(1, &texture_buffer_object);
	glBindTexture(GL_TEXTURE_2D, texture_buffer_object);

	// 指定纹理的参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	int width, height, nrchannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filename, &width, &height, &nrchannels, 0);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture: " << filename << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
}

// 创建纹理对象并加载所有纹理
void loadAllTextures() {
	loadTexture(texture_buffer_object_sun, "res/sun.jpg");
	loadTexture(texture_buffer_object_earth, "res/earth.jpg");
	loadTexture(texture_buffer_object_moon, "res/moon.jpg");
}


// 从文件中读取着色器代码
std::string readShaderFromFile(const char* filePath) {
	std::string shaderCode;
	std::ifstream shaderFile;

	// 保证 ifstream 对象可以抛出异常：
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		// 打开文件
		shaderFile.open(filePath);
		std::stringstream shaderStream;

		// 读取文件的缓冲内容到数据流中
		shaderStream << shaderFile.rdbuf();

		// 关闭文件处理器
		shaderFile.close();

		// 转换数据流到string
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure& e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	return shaderCode;
}
// 编写并编译纹理着色器程序
void editAndCompileShaderProgramForTexture() {
	// 顶点着色器和片段着色器源码
	const char* vertex_shader_source =
		"#version 330 core\n"
		"layout (location = 0) in vec3 vPos;\n"			// 位置变量的属性位置值为0
		"layout (location = 1) in vec2 vTexture;\n"		// 纹理变量的属性位置值为1
		"out vec4 vColor;\n"							// 输出4维颜色向量
		"out vec2 myTexture;\n"							// 输出2维纹理向量

		"uniform mat4 transform;\n"
		"uniform vec4 color;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = transform * vec4(vPos, 1.0);\n"
		"    vColor = color;\n"
		"    myTexture = vTexture;\n"
		"}\n\0";
	const char* fragment_shader_source =
		"#version 330 core\n"
		"in vec4 vColor;\n"			// 输入的颜色向量
		"in vec2 myTexture;\n"		// 输入的纹理向量
		"out vec4 FragColor;\n"		// 输出的颜色向量
		"uniform sampler2D tex;\n" 
		"void main()\n"
		"{\n"
		"    FragColor = texture(tex, myTexture) * vColor;\n"	// 顶点颜色和纹理混合
		"}\n\0";

	// 生成并编译着色器
	// 顶点着色器
	int success;
	char info_log[512];
	int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	// 检查着色器是否成功编译，如果编译失败，打印错误信息
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
	}
	// 片段着色器
	int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	// 检查着色器是否成功编译，如果编译失败，打印错误信息
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
	}
	// 链接顶点和片段着色器至一个着色器程序
	shader_program_tex = glCreateProgram();
	glAttachShader(shader_program_tex, vertex_shader);
	glAttachShader(shader_program_tex, fragment_shader);
	glLinkProgram(shader_program_tex);
	// 检查着色器是否成功链接，如果链接失败，打印错误信息
	glGetProgramiv(shader_program_tex, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_program_tex, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
	}

	// 删除着色器
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// 使用着色器程序
	glUseProgram(shader_program_tex);
}

// 创建并编译着色器程序
int createShaderProgram(const char* vertex_shader_source, const char* fragment_shader_source) {
	int success;
	char info_log[512];

	// 顶点着色器
	int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		std::cout << "lightERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
	}

	// 片段着色器
	int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		std::cout << "lightERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
	}

	// 链接着色器程序
	int shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_program, 512, NULL, info_log);
		std::cout << "lightERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
	}

	// 删除着色器
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return shader_program;
}



void initial(void)
{
	// 生成球的顶点
	generateBallVerticles(sphereVertices);

	// 生成球的顶点索引
	generateBallIndices(sphereIndices);

	// 生成并绑定球体的VAO和VBO
	glGenVertexArrays(1, &vertex_array_object);
	glGenBuffers(1, &vertex_buffer_object);
	glBindVertexArray(vertex_array_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);

	// 将顶点数据绑定至当前默认的缓冲中
	glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);

	// 生成并绑定EBO
	glGenBuffers(1, &element_buffer_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);

	// 将数据绑定至缓冲
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);
	
	// 设置顶点属性指针 <ID>, <num>, GL_FLOAT, GL_FALSE, <offset>, <begin>
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	// 创建纹理对象并加载纹理
	loadAllTextures();

	// 编写并编译着色器程序
	editAndCompileShaderProgramForTexture();

	//编译光照着色器程序
	// 
	createLightGeometry();
	//读light.vs 读light.fs
	std::string lightVertexShaderSource = readShaderFromFile("light.vs");
	std::string lightFragmentShaderSource = readShaderFromFile("light.fs");
	shader_program_light = createShaderProgram(lightVertexShaderSource.c_str(), lightFragmentShaderSource.c_str());

	// 设定点线面的属性
	glPointSize(3);	// 设置点的大小
	glLineWidth(1);	// 设置线宽

	// opengl属性
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// 指定多边形模式为填充
	glEnable(GL_DEPTH_TEST);	// 启用深度测试

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GL_TRUE);	// 关闭窗口
		break;
	case GLFW_KEY_1:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// 线框模式
		break;
	case GLFW_KEY_2:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// 填充模式
		break;
	case GLFW_KEY_3:
		glEnable(GL_CULL_FACE);						// 打开背面剔除
		glCullFace(GL_BACK);						// 剔除多边形的背面
		break;
	case GLFW_KEY_4:
		glDisable(GL_CULL_FACE);					// 关闭背面剔除
		break;
	default:
		break;
	}
}

void Draw(void)
{
	// 清空颜色缓冲
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	unsigned int transformLoc = glGetUniformLocation(shader_program_tex, "transform");
	unsigned int colorLoc = glGetUniformLocation(shader_program_tex, "color");
	// 设置纹理单元的值
	
	GLfloat vColor[3][4] = {
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f } };
	

	// 绑定VAO
	glBindVertexArray(vertex_array_object);
	
	vmath::mat4 view, projection, trans;

	// 创建观察矩阵、投影矩阵
	{
		view = vmath::lookat(vmath::vec3(0.0, 3.0, 0.0), vmath::vec3(0.0, 0.0, -10.0), vmath::vec3(0.0, 1.0, 0.0));
		/*
		* 投影矩阵参数：
		* perspective(Fovy, Aspact, ZNear, ZFar)：投影改变
		* Fovy是眼睛上下睁开的幅度，角度值，值越小，视野范围越狭小；值越大，视野范围越宽阔；
		* Aspact表示裁剪面的宽w高h比，这个影响到视野的截面有多大（这里设置成和显示区域的宽高比一致即可，比如800*600，则设置成4/3）；
		* ZNear表示近裁剪面到眼睛的距离，ZFar表示远裁剪面到眼睛的距离。注意zNear和zFar不能设置设置为负值（你怎么看到眼睛后面的东西）。
		*/
		projection = vmath::perspective(fovy, aspact, znear, zfar);
		trans = projection * view;
	}
	
	// 画太阳
		{
		glUniform1i(glGetUniformLocation(shader_program_tex, "tex"), 0);  // 太阳纹理单元为0
		GLfloat angle_sun_self = day * (360.0f / 25.05f);	// 自转角
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_buffer_object_sun);
		/*
		* translate(x, y, z)：图形平移（相对于显示区域而言）
		* 此处x，y=0表示在屏幕中间，z=-10表示图形在屏幕里面（离摄像机）10个单位距离
		*/
		trans *= vmath::translate(0.0f, 0.0f, -10.0f);
		vmath::mat4 trans_sun = trans * vmath::rotate(angle_sun_self, vmath::vec3(0.0f, 1.0f, 0.0f));	// 自转
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, trans_sun);
		glUniform4fv(colorLoc, 1, vColor[0]);
		glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);	// 绘制三角形
	}
	
	

	
	// 画地球
	{
		glUniform1i(glGetUniformLocation(shader_program_tex, "tex"), 1);  // 地球纹理单元为1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture_buffer_object_earth);
		float a_earth = 9.0f;	// 椭圆长轴
		float b_earth = 3.0f;	// 椭圆短轴
		GLfloat angle_earth = day * (360.0f / 365.00f);	// 公转角
		float x_earth = a_earth * cosf(angle_earth * (float)PI / 180.0f);
		float y_earth = b_earth * sinf(angle_earth * (float)PI / 180.0f);

		GLfloat angle_earth_self = day * (360.0f / 1.00f);	// 自转角

		trans *= vmath::translate(-x_earth, 0.0f, y_earth);	// 3.公转椭圆轨道
		vmath::mat4 trans_earth = trans * vmath::rotate(angle_earth_self, vmath::vec3(0.0f, 1.0f, 0.0f));	// 2.自转
		trans_earth *= vmath::scale(0.6f);	// 1.缩放
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, trans_earth);
		glUniform4fv(colorLoc, 1, vColor[1]);
		glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
	}
	
	// 画月球
	{
		glUniform1i(glGetUniformLocation(shader_program_tex, "tex"), 2);  // 月球纹理单元为2
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture_buffer_object_moon);
		GLfloat angle_moon = day * (360.0f / (365.00f / 12.00f));	// 公转角
		GLfloat angle_moon_self = day * (360.0f / 27.32f);			// 自转角

		trans *= vmath::rotate(angle_moon, vmath::vec3(sqrtf(2.0) / 2.0f, sqrtf(2.0) / 2.0f, 0.0f));	// 4.倾斜45度角公转
		trans *= vmath::translate(0.0f, 0.0f, 1.5f);	// 3.设置公转半径
		vmath::mat4 trans_moon = trans * vmath::rotate(angle_moon_self, vmath::vec3(0.0f, 1.0f, 0.0f));	// 2.自转
		trans_moon *= vmath::scale(0.6f * 0.5f);	// 1.缩放
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, trans_moon);
		glUniform4fv(colorLoc, 1, vColor[2]);
		glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
	}
	
	// 解除绑定
	glBindVertexArray(0);


	// 渲染光源
	glUseProgram(shader_program_light);

	// 使用太阳的位置信息作为光源的位置
	vmath::vec3 lightPosition(0.0f, 0.0f, -10.0f); // 你可以根据太阳的位置进行调整

	// 设置光源的模型矩阵
	vmath::mat4 trans_light = vmath::translate(lightPosition);
	glUniformMatrix4fv(glGetUniformLocation(shader_program_light, "transform"), 1, GL_FALSE, trans_light);

	// 设置光源的颜色
	glUniform3f(glGetUniformLocation(shader_program_light, "lightColor"), 1.0f, 1.0f, 1.0f);

	// 绑定VAO和绘制光源（使用简化的几何体，如立方体表示光源）
	glBindVertexArray(lightVertexArrayObject);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	// 恢复使用原始的着色器程序
	glUseProgram(shader_program_tex);

}

void reshaper(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	if (height == 0)
	{
		aspact = (float)width;
	}
	else
	{
		aspact = (float)width / (float)height;
	}
	
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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HUST-ComputerGraphics-CSEE2101 U202115285", NULL, NULL);

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

	//窗口大小改变时调用reshaper函数
	glfwSetFramebufferSizeCallback(window, reshaper);

	//窗口中有键盘操作时调用key_callback函数
	glfwSetKeyCallback(window, key_callback);

	std::cout << "数字键1，2设置多边形模式为线模式和填充模式。" << std::endl;
	std::cout << "数字键3打开剔除模式并且剔除多边形的背面。" << std::endl;
	std::cout << "数字键4关闭剔除模式。" << std::endl;

	while (!glfwWindowShouldClose(window))
	{
		day+=0.2;
		if (day >= 365)
			day = 0;
		Draw();
		Sleep(33.3);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// 解绑和删除VAO和VBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteBuffers(1, &vertex_buffer_object);

	//解绑并删除纹理
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &texture_buffer_object_sun);

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}
