
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vmath.h>
#include <vector>
#include <Windows.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// �յ���ϵͳ������
static GLfloat day = 0.0;

// ���ڳߴ����
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 900;

// ͶӰ�������
const float fovy = 60;
float aspact = (float)SCR_WIDTH / (float)SCR_HEIGHT;
const float znear = 1;
const float zfar = 800;

// �������
GLuint vertex_array_object;		// VAO���
GLuint vertex_buffer_object;	// VBO���
GLuint element_buffer_object;	// EBO���
GLuint texture_buffer_object;	// ���������
int shader_program;				// ��ɫ��������

// ���涥������
std::vector<float> sphereVertices;
std::vector<int> sphereIndices;
const int Y_SEGMENTS = 50;
const int X_SEGMENTS = 50;
const float Radio = 2.0;
const GLfloat  PI = 3.14159265358979323846f;

// ������Ķ����������
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
			// ��Ķ���
			sphereVertices.push_back(xPos);
			sphereVertices.push_back(yPos);
			sphereVertices.push_back(zPos);
			// �����㣬ӳ�䵽��γ
			sphereVertices.push_back(xSegment);
			sphereVertices.push_back(ySegment);
		}
	}
}

// ������Ķ�������
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

// ����������󲢼�������
void loadTexture() {
	//�����������
	glGenTextures(1, &texture_buffer_object);
	glBindTexture(GL_TEXTURE_2D, texture_buffer_object);
	//ָ������Ĳ���
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//��������
	int width, height, nrchannels;//������ͨ����
	stbi_set_flip_vertically_on_load(true);
	//��������ͼƬ
	unsigned char* data = stbi_load("res/rock.jpg", &width, &height, &nrchannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);	//����Mipmap����
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	stbi_image_free(data);//�ͷ���Դ
}

// ��д��������ɫ������
void editAndCompileShaderProgram() {
	// ������ɫ����Ƭ����ɫ��Դ��
	const char* vertex_shader_source =
		"#version 330 core\n"
		"layout (location = 0) in vec3 vPos;\n"			// λ�ñ���������λ��ֵΪ0
		"layout (location = 1) in vec2 vTexture;\n"		// �������������λ��ֵΪ1
		"out vec4 vColor;\n"							// ���4ά��ɫ����
		"out vec2 myTexture;\n"							// ���2ά��������

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
		"in vec4 vColor;\n"			// �������ɫ����
		"in vec2 myTexture;\n"		// �������������
		"out vec4 FragColor;\n"		// �������ɫ����
		"uniform sampler2D tex;\n"
		"void main()\n"
		"{\n"
		"    FragColor = texture(tex, myTexture) * vColor;\n"	// ������ɫ��������
		"}\n\0";

	// ���ɲ�������ɫ��
	// ������ɫ��
	int success;
	char info_log[512];
	int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	// �����ɫ���Ƿ�ɹ����룬�������ʧ�ܣ���ӡ������Ϣ
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
	}
	// Ƭ����ɫ��
	int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	// �����ɫ���Ƿ�ɹ����룬�������ʧ�ܣ���ӡ������Ϣ
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
	}
	// ���Ӷ����Ƭ����ɫ����һ����ɫ������
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	// �����ɫ���Ƿ�ɹ����ӣ��������ʧ�ܣ���ӡ������Ϣ
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_program, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
	}

	// ɾ����ɫ��
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// ʹ����ɫ������
	glUseProgram(shader_program);
}

void initial(void)
{
	// ������Ķ���
	generateBallVerticles(sphereVertices);

	// ������Ķ�������
	generateBallIndices(sphereIndices);

	// ���ɲ��������VAO��VBO
	glGenVertexArrays(1, &vertex_array_object);
	glGenBuffers(1, &vertex_buffer_object);
	glBindVertexArray(vertex_array_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);

	// ���������ݰ�����ǰĬ�ϵĻ�����
	glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);

	// ���ɲ���EBO
	glGenBuffers(1, &element_buffer_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);

	// �����ݰ�������
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);
	
	// ���ö�������ָ�� <ID>, <num>, GL_FLOAT, GL_FALSE, <offset>, <begin>
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	// ����������󲢼�������
	loadTexture();

	// ��д��������ɫ������
	editAndCompileShaderProgram();

	// �趨�����������
	glPointSize(3);	// ���õ�Ĵ�С
	glLineWidth(1);	// �����߿�

	// opengl����
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// ָ�������ģʽΪ���
	glEnable(GL_DEPTH_TEST);	// ������Ȳ���

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GL_TRUE);	// �رմ���
		break;
	case GLFW_KEY_1:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// �߿�ģʽ
		break;
	case GLFW_KEY_2:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// ���ģʽ
		break;
	case GLFW_KEY_3:
		glEnable(GL_CULL_FACE);						// �򿪱����޳�
		glCullFace(GL_BACK);						// �޳�����εı���
		break;
	case GLFW_KEY_4:
		glDisable(GL_CULL_FACE);					// �رձ����޳�
		break;
	default:
		break;
	}
}

void Draw(void)
{
	// �����ɫ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	unsigned int transformLoc = glGetUniformLocation(shader_program, "transform");
	unsigned int colorLoc = glGetUniformLocation(shader_program, "color");

	// ����ͼ�ε���ɫ
	GLfloat vColor[3][4] = {
		{ 1.0f, 0.3f, 0.3f, 1.0f },
		{ 0.6f, 0.6f, 1.0f, 1.0f },
		{ 0.8f, 0.8f, 0.8f, 1.0f } };

	// ��VAO
	glBindVertexArray(vertex_array_object);
	
	vmath::mat4 view, projection, trans;

	// �����۲����ͶӰ����
	{
		view = vmath::lookat(vmath::vec3(0.0, 3.0, 0.0), vmath::vec3(0.0, 0.0, -10.0), vmath::vec3(0.0, 1.0, 0.0));
		/*
		* ͶӰ���������
		* perspective(Fovy, Aspact, ZNear, ZFar)��ͶӰ�ı�
		* Fovy���۾����������ķ��ȣ��Ƕ�ֵ��ֵԽС����Ұ��ΧԽ��С��ֵԽ����Ұ��ΧԽ������
		* Aspact��ʾ�ü���Ŀ�w��h�ȣ����Ӱ�쵽��Ұ�Ľ����ж���������óɺ���ʾ����Ŀ�߱�һ�¼��ɣ�����800*600�������ó�4/3����
		* ZNear��ʾ���ü��浽�۾��ľ��룬ZFar��ʾԶ�ü��浽�۾��ľ��롣ע��zNear��zFar������������Ϊ��ֵ������ô�����۾�����Ķ�������
		*/
		projection = vmath::perspective(fovy, aspact, znear, zfar);
		trans = projection * view;
	}
	
	// ��̫��
	{
		GLfloat angle_sun_self = day * (360.0f / 25.05f);	// ��ת��

		/*
		* translate(x, y, z)��ͼ��ƽ�ƣ��������ʾ������ԣ�
		* �˴�x��y=0��ʾ����Ļ�м䣬z=-10��ʾͼ������Ļ���棨���������10����λ����
		*/
		trans *= vmath::translate(0.0f, 0.0f, -10.0f);
		vmath::mat4 trans_sun = trans * vmath::rotate(angle_sun_self, vmath::vec3(0.0f, 1.0f, 0.0f));	// ��ת
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, trans_sun);
		glUniform4fv(colorLoc, 1, vColor[0]);
		glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);	// ����������
	}
	
	// ������
	{
		float a_earth = 9.0f;	// ��Բ����
		float b_earth = 3.0f;	// ��Բ����
		GLfloat angle_earth = day * (360.0f / 365.00f);	// ��ת��
		float x_earth = a_earth * cosf(angle_earth * (float)PI / 180.0f);
		float y_earth = b_earth * sinf(angle_earth * (float)PI / 180.0f);

		GLfloat angle_earth_self = day * (360.0f / 1.00f);	// ��ת��

		trans *= vmath::translate(-x_earth, 0.0f, y_earth);	// 3.��ת��Բ���
		vmath::mat4 trans_earth = trans * vmath::rotate(angle_earth_self, vmath::vec3(0.0f, 1.0f, 0.0f));	// 2.��ת
		trans_earth *= vmath::scale(0.6f);	// 1.����
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, trans_earth);
		glUniform4fv(colorLoc, 1, vColor[1]);
		glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
	}
	
	// ������
	{
		GLfloat angle_moon = day * (360.0f / (365.00f / 12.00f));	// ��ת��
		GLfloat angle_moon_self = day * (360.0f / 27.32f);			// ��ת��

		trans *= vmath::rotate(angle_moon, vmath::vec3(sqrtf(2.0) / 2.0f, sqrtf(2.0) / 2.0f, 0.0f));	// 4.��б45�Ƚǹ�ת
		trans *= vmath::translate(0.0f, 0.0f, 1.5f);	// 3.���ù�ת�뾶
		vmath::mat4 trans_moon = trans * vmath::rotate(angle_moon_self, vmath::vec3(0.0f, 1.0f, 0.0f));	// 2.��ת
		trans_moon *= vmath::scale(0.6f * 0.5f);	// 1.����
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, trans_moon);
		glUniform4fv(colorLoc, 1, vColor[2]);
		glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
	}
	
	// �����
	glBindVertexArray(0);

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
	glfwInit(); // ��ʼ��GLFW

	// OpenGL�汾Ϊ3.3�����ΰ汾�ž���Ϊ3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// ʹ�ú���ģʽ(������������)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// ��������(���ߡ���������)
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HUST-ComputerGraphics-CSEE2101 U202115285", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to Create OpenGL Context" << std::endl;
		glfwTerminate();
		return -1;
	}

	// �����ڵ�����������Ϊ��ǰ�̵߳���������
	glfwMakeContextCurrent(window);

	// ��ʼ��GLAD������OpenGL����ָ���ַ�ĺ���
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	initial();

	//���ڴ�С�ı�ʱ����reshaper����
	glfwSetFramebufferSizeCallback(window, reshaper);

	//�������м��̲���ʱ����key_callback����
	glfwSetKeyCallback(window, key_callback);

	std::cout << "���ּ�1��2���ö����ģʽΪ��ģʽ�����ģʽ��" << std::endl;
	std::cout << "���ּ�3���޳�ģʽ�����޳�����εı��档" << std::endl;
	std::cout << "���ּ�4�ر��޳�ģʽ��" << std::endl;

	while (!glfwWindowShouldClose(window))
	{
		day+=1.2;
		if (day >= 365)
			day = 0;
		Draw();
		Sleep(33.3);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// ����ɾ��VAO��VBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteBuffers(1, &vertex_buffer_object);

	//���ɾ������
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &texture_buffer_object);

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}
