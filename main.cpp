#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "headers/stb_image.h"
#include "headers/shader.h"
#include "headers/camera.h"
#include "headers/model.h"
#include "headers/Sphere.h"

#include <iostream>

enum CameraType
{
	STATIC,
	TRACKING,
	FREE
};

void init_glfw();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(std::string path);
void settingsKeyCallback(GLFWwindow* window, int key, int scancode, int action, int modes);
void setLights(Shader shader);
void setFog(Shader shader);
void changeCameraType();
glm::mat4 GetViewMatrix();
glm::mat4 GetProjectionMatrix();

// ustawienia ekranu
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// ustawienia kamery
CameraType activeCameraType = FREE;
// free
Camera freeCamera(glm::vec3(0.0f, 0.5f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
// static
Camera staticCamera(3.0f, 4.5f, 4.0f, 0.0f, 0.0f, 0.0f);
// tracking
Camera trackingCamera(glm::vec3(0.0f, 3.0f, 4.0f));

// ustawienia mg³y
bool fogOn = false;
float fogExpDensity = 2.0f;
float fogEnd = -200.0f;
glm::vec3 fogColor = glm::vec3(1.0f, 1.0f, 1.0f);

// Czas, aby poruszanie kamery nie zale¿a³o od szybkoœci wykonywania pêtli render loop
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// po³o¿enie oœwietlenia
glm::vec3 lightPos(1.2f, 1.0f, -3.0f);
glm::vec3 sunPos(0.2f, -1.0f, 0.3f);
glm::vec3 flashlightPos(0.0f, 0.0f, 0.0f);
glm::vec3 flashlightStartDir(0.0f, 0.0f, -1.0f);
glm::vec3 flashlightDir(0.0f, 0.0f, 0.0f);

// poruszanie siê obiektu po okrêgu
const float RADIUS = 0.5f;
const float CIRCURAL_SPEED = 1.0f;
const float Y_POSITION = 0.25f;
float theta = 0.0f;

// animacja flagi
const float windFrequency = 2.0f;
const float windApmlitude = 0.25f;
float windTimeElapsed = 0.0f;

int main()
{
	init_glfw();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GK Projekt 4", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetKeyCallback(window, settingsKeyCallback);

	unsigned int daySkyboxTexture = loadCubemap("resources/skyboxes/day/");
	unsigned int groundAlbedoMap = loadTexture("resources/ground/Ground037_4K-JPG_Color.jpg");
	unsigned int groundRoughnessMap = loadTexture("resources/ground/Ground037_4K-JPG_Roughness.jpg");
	unsigned int boxDiffuseMap = loadTexture("resources/container/container2.png");
	unsigned int boxSpecularMap = loadTexture("resources/container/container2_specular.png");

	float skyboxVertices[] = {
        
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);

	glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	float boxVertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	float floorVertices[] = {
	-10.0f, 0.0f, -10.0f,  0.0f,  1.0f, 0.0f,  0.0f,  0.0f,
	 10.0f, 0.0f, -10.0f,  0.0f,  1.0f, 0.0f,  10.0f, 0.0f,
	 10.0f, 0.0f,  10.0f,  0.0f,  1.0f, 0.0f,  10.0f, 10.0f,
	-10.0f, 0.0f, -10.0f,  0.0f,  1.0f, 0.0f,  0.0f,  0.0f,
	 10.0f, 0.0f,  10.0f,  0.0f,  1.0f, 0.0f,  10.0f, 10.0f,
	-10.0f, 0.0f,  10.0f,  0.0f,  1.0f, 0.0f,  0.0f,  10.0f,
	};

	float flagControlPoints[] = {
		0.0f, 0.0f, 0.0f,
		0.0f, 0.33f, 0.0f,
		0.0f, 0.66f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.53f, 0.0f, 0.0f,
		0.53f, 0.33f, 0.0f,
		0.53f, 0.66f, 0.0f,
		0.53f, 1.0f, 0.0f,
		1.06f, 0.0f, 0.0f,
		1.06f, 0.33f, 0.0f,
		1.06f, 0.66f, 0.0f,
		1.06f, 1.0f, 0.0f,
		1.6f, 0.0f, 0.0f,
		1.6f, 0.33f, 0.0f,
		1.6f, 0.66f, 0.0f,
		1.6f, 1.0f, 0.0f
	};

	unsigned int boxIndices[] = {
		0, 1, 3,
		1, 2, 3
	};

	Shader lightingShader("shaders/shader.vs", "shaders/shader.fs");
	Shader lightCubeShader("shaders/light_cube_shader.vs", "shaders/light_cube_shader.fs");
	Shader skyboxShader("shaders/skybox_shader.vs", "shaders/skybox_shader.fs");
	Shader floorShader("shaders/floor_shader.vs", "shaders/floor_shader.fs");
	Shader sphereShader("shaders/sphere_shader.vs", "shaders/sphere_shader.fs");
	Shader containerShader("shaders/container_shader.vs", "shaders/container_shader.fs");
	Shader flagShader("shaders/flag_shader.vs", "shaders/flag_shader.fs", "shaders/flag_shader.tcs", "shaders/flag_shader.tes");

	Model backpackModel("resources/backpack/backpack.obj");
	Sphere sphere;

	// box VAO
	unsigned int boxVAO, boxVBO;
	glGenVertexArrays(1, &boxVAO);
	glGenBuffers(1, &boxVBO);
	glBindVertexArray(boxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// light VAO
	unsigned int lightCubeVAO, lightCubeVBO;
	glGenVertexArrays(1, &lightCubeVAO);
	glGenBuffers(1, &lightCubeVBO);
	glBindVertexArray(lightCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// flag VAO
	unsigned int flagVAO, flagVBO;
	glGenVertexArrays(1, &flagVAO);
	glGenBuffers(1, &flagVBO);
	glBindVertexArray(flagVAO);
	glBindBuffer(GL_ARRAY_BUFFER, flagVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(flagControlPoints), flagControlPoints, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// floor VAO
	unsigned int floorVAO, floorVBO;
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glBindVertexArray(floorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	// sphere VAO
	unsigned int sphereVAO, sphereVBO, sphereEBO;
	glGenVertexArrays(1, &sphereVAO);
	glBindVertexArray(sphereVAO);
	glGenBuffers(1, &sphereVBO);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, sphere.getInterleavedVertexSize(), sphere.getInterleavedVertices(), GL_STATIC_DRAW);
	glGenBuffers(1, &sphereEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.getIndexSize(), sphere.getIndices(), GL_STATIC_DRAW);

	int sphereDataStride = sphere.getInterleavedStride();
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sphereDataStride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sphereDataStride, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sphereDataStride, (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	floorShader.use();
	floorShader.setInt("albedoMap", 0);
	floorShader.setInt("roughnessMap", 1);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	containerShader.use();
	containerShader.setInt("material.diffuse", 0);
	containerShader.setInt("material.specular", 1);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		windTimeElapsed += deltaTime;

		// update kamery œledz¹cej poruszaj¹cy siê obiekt
		theta += CIRCURAL_SPEED * deltaTime;
		float new_x = RADIUS * cos(theta), new_z = RADIUS * sin(theta);
		trackingCamera.UpdateTarget(glm::vec3(new_x, Y_POSITION, new_z));

		// input
		processInput(window);

		// render commands
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// container
		containerShader.use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(new_x, Y_POSITION, new_z));
		model = glm::rotate(model, (-1) * atan2(new_z, new_x), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		flashlightDir = glm::vec3(model * glm::vec4(flashlightStartDir, 0.0f));
		flashlightPos = glm::vec3(new_x, Y_POSITION, new_z);
		setLights(containerShader);
		setFog(containerShader);
		containerShader.setMat4("model", model);
		glm::mat4 projection = GetProjectionMatrix();
		glm::mat4 view = GetViewMatrix();
		containerShader.setMat4("projection", projection);
		containerShader.setMat4("view", view);
		containerShader.setFloat("material.shininess", 64.0f);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, boxDiffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, boxSpecularMap);
		glBindVertexArray(boxVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// plecak
		lightingShader.use();
		setLights(lightingShader);
		setFog(lightingShader);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0f, 0.4f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		lightingShader.setMat4("model", model);
		projection = GetProjectionMatrix();
		view = GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		backpackModel.Draw(lightingShader);
		
		// rysowanie sfery
		sphereShader.use();
		setFog(sphereShader);
		setLights(sphereShader);
		projection = GetProjectionMatrix();
		view = GetViewMatrix();
		sphereShader.setMat4("projection", projection);
		sphereShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.25f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f));
		sphereShader.setMat4("model", model);
		// materia³ br¹zu z tabelki
		sphereShader.setVec3("material.ambient", glm::vec3(0.2125f, 0.1275f, 0.054f));
		sphereShader.setVec3("material.diffuse", glm::vec3(0.714f, 0.4284f, 0.18144f));
		sphereShader.setVec3("material.specular", glm::vec3(0.393548f, 0.271906f, 0.166721f));
		sphereShader.setFloat("material.shininess", 25.6f);
		glBindVertexArray(sphereVAO);
		glDrawElements(GL_TRIANGLES, sphere.getIndexCount(), GL_UNSIGNED_INT, 0);

		// flaga
		flagShader.use();
		setFog(flagShader);
		setLights(flagShader);
		projection = GetProjectionMatrix();
		view = GetViewMatrix();
		flagShader.setMat4("projection", projection);
		flagShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
		model = glm::scale(model, glm::vec3(0.8f));
		flagShader.setMat4("model", model);
		// material
		flagShader.setVec3("material.ambient", glm::vec3(0.2125f, 0.1275f, 0.054f));
		flagShader.setVec3("material.diffuse", glm::vec3(0.714f, 0.4284f, 0.18144f));
		flagShader.setVec3("material.specular", glm::vec3(0.393548f, 0.271906f, 0.166721f));
		flagShader.setFloat("material.shininess", 25.6f);
		// update controlPoints
		for (int i = 1; i < 4; i++)
		{
			float diff = 2.0f * i;
			for (int j = 0; j < 4; j++)
			{
				flagControlPoints[(i * 4 + j) * 3 + 2] = sin(windFrequency * windTimeElapsed + diff) * windApmlitude;
				if (j != 0)
					flagControlPoints[(i + 4 * j) * 3 + 2] = cos(windFrequency * windTimeElapsed + (diff - 0.3f)) * windApmlitude;
			}
		}

		glUniform3fv(glGetUniformLocation(flagShader.ID, "controlPoints"), 16, (float*)flagControlPoints);
		glBindVertexArray(flagVAO);
		glPatchParameteri(GL_PATCH_VERTICES, 16);
		glDrawArrays(GL_PATCHES, 0, 16);

		// rysowanie pod³o¿a
		floorShader.use();
		setLights(floorShader);
		setFog(floorShader);
		projection = GetProjectionMatrix();
		view = GetViewMatrix();
		floorShader.setMat4("projection", projection);
		floorShader.setMat4("view", view);

		glBindVertexArray(floorVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, groundAlbedoMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, groundRoughnessMap);
		model = glm::mat4(1.0f);
		floorShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// rysowanie skyboxa
		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		view = glm::mat4(glm::mat3(GetViewMatrix()));
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, daySkyboxTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void init_glfw()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (activeCameraType == FREE)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			freeCamera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			freeCamera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			freeCamera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			freeCamera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (activeCameraType == FREE)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		freeCamera.ProcessMouseMovement(xoffset, yoffset);
	}
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int loadCubemap(std::string path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	vector<std::string> faces {
		"right.jpg",
			"left.jpg",
			"top.jpg",
			"bottom.jpg",
			"front.jpg",
			"back.jpg"
	};

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load((path + faces[i]).c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void settingsKeyCallback(GLFWwindow* window, int key, int scancode, int action, int modes)
{
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
		fogOn = !fogOn;
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
		changeCameraType();
}

void setLights(Shader shader)
{
	// point light 1
	shader.setVec3("lightPos[0]", lightPos);
	shader.setVec3("pointLights[0].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader.setVec3("pointLights[0].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setFloat("pointLights[0].constant", 1.0f);
	shader.setFloat("pointLights[0].linear", 0.07f);
	shader.setFloat("pointLights[0].quadratic", 0.017f);
	// directional light
	shader.setVec3("dirLightDirection", sunPos);
	shader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	shader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
	shader.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	// flashlight
	shader.setVec3("flashlightPos[0]", flashlightPos);
	shader.setVec3("flashlightDir[0]", flashlightDir);
	shader.setFloat("flashlights[0].constant", 1.0f);
	shader.setFloat("flashlights[0].linear", 0.035f);
	shader.setFloat("flashlights[0].quadratic", 0.44f);
	shader.setFloat("flashlights[0].cutOff", glm::cos(glm::radians(12.5f)));
	shader.setVec3("flashlights[0].ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	shader.setVec3("flashlights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
	shader.setVec3("flashlights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setFloat("flashlights[0].outerCutOff", glm::cos(glm::radians(17.5f)));
}

void setFog(Shader shader)
{
	shader.setBool("fog.IsOn", fogOn);
	shader.setFloat("fog.ExpDensity",fogExpDensity);
	shader.setFloat("fog.End", fogEnd);
	shader.setVec3("fog.Color", fogColor);
}

void changeCameraType()
{
	int aCTint = static_cast<int>(activeCameraType);
	aCTint = (aCTint + 1) % 3;
	activeCameraType = static_cast<CameraType>(aCTint);
	firstMouse = true;
}

glm::mat4 GetViewMatrix()
{
	switch (activeCameraType)
	{
		case FREE:
		{
			return freeCamera.GetViewMatrix();
		}
		case STATIC:
		{
			return staticCamera.GetViewMatrix();
		}
		case TRACKING:
		{
			return trackingCamera.GetViewMatrix();
		}
	}
}

glm::mat4 GetProjectionMatrix()
{
	switch (activeCameraType)
	{
		case FREE:
		{
			return glm::perspective(glm::radians(freeCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		}
		case STATIC:
		{
			return glm::perspective(glm::radians(staticCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		}
		case TRACKING:
		{
			return glm::perspective(glm::radians(trackingCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		}
	}
}