//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

//glfw include
#include <GLFW/glfw3.h>

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// program include
#include "Headers/TimeManager.h"
#include "Headers/Shader.h"
// Geometry primitives
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
//Texture includes
#include "Headers/Texture.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

Sphere sphere(20, 20);
Sphere sphere2(20, 20);
Cylinder cylinder(20, 20, 0.5, 0.5);
Cylinder cylinder2(20, 20, 0.5, 0.5);
Box box;
Box box2;

Shader shaderColor;
Shader shaderTexture;
Shader shaderCubeTexture;
Shader shaderMateriales;
Shader shaderDirectionLight;
Shader shaderPointLight;
Shader shaderPointLight2;
Shader shaderSpotLight;
GLuint textureCubeTexture;
GLuint textureID1, textureID2, textureID3, textureID4, textureID5, textureID6, textureID7, textureID8, textureID9, textureID10, textureID11;
GLuint textureID12, textureID13, textureID14, textureID15, textureID16;
GLuint cubeTextureID;

GLenum types[6] = {
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

std::string fileNames[6] = { 
	
	"../../Textures/mp_deviltooth/devils-tooth_ft.tga",
	"../../Textures/mp_deviltooth/devils-tooth_bk.tga",
	"../../Textures/mp_deviltooth/devils-tooth_up.tga",
	"../../Textures/mp_deviltooth/devils-tooth_dn.tga",
	"../../Textures/mp_deviltooth/devils-tooth_rt.tga",
	"../../Textures/mp_deviltooth/devils-tooth_lf.tga"
};

int screenWidth;
int screenHeight;

GLFWwindow * window;

bool exitApp = false;
int lastMousePosX, offsetX;
int lastMousePosY, offsetY;

bool light = true;
double deltaTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroyWindow();
void destroy();
bool processInput(bool continueApplication = true);

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
			glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
			nullptr);

	if (window == nullptr) {
		std::cerr
			<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
			<< std::endl;
		destroyWindow();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
			exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	shaderColor.initialize("../../Shaders/transformaciones.vs", "../../Shaders/transformaciones.fs");
	shaderTexture.initialize("../../Shaders/texturizado_res.vs", "../../Shaders/texturizado_res.fs");
	shaderCubeTexture.initialize("../../Shaders/cubeTexture.vs", "../../Shaders/cubeTexture.fs");
	shaderMateriales.initialize("../../Shaders/iluminacion_materiales_res.vs", "../../Shaders/iluminacion_materiales_res.fs");
	shaderDirectionLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/directionalLight.fs");
	shaderPointLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/pointLight.fs");
	shaderPointLight2.initialize("../../Shaders/typeLight.vs", "../../Shaders/pointLight.fs");
	shaderSpotLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/spotLight.fs");

	sphere2.init();
	sphere.init();
	cylinder.init();
	cylinder2.init();
	box.init();
	box2.init();

	camera->setPosition(glm::vec3(0.0f, 0.0f, 0.4f));

			// Textura Pared de piedra
	int imageWidth, imageHeight;
	Texture texture("../../Textures/pared_piedra.png");
	FIBITMAP* bitmap = texture.loadImage(false);
	unsigned char * data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID1);
	glBindTexture(GL_TEXTURE_2D, textureID1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

			// Texture fachada interior
	texture = Texture("../../Textures/fachadar.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID2);
	glBindTexture(GL_TEXTURE_2D, textureID2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Textura cuadritos
	texture = Texture("../../Textures/test.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID3);
	glBindTexture(GL_TEXTURE_2D, textureID3);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Textura Madera mesa
	texture = Texture("../../Textures/madera_mesa.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID4);
	glBindTexture(GL_TEXTURE_2D, textureID4);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Textura Pared de ladrillos
	texture = Texture("../../Textures/pared_ladrillos.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID5);
	glBindTexture(GL_TEXTURE_2D, textureID5);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Textura Fachada
	texture = Texture("../../Textures/fachada.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID6);
	glBindTexture(GL_TEXTURE_2D, textureID6);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);


	// Textura Techo
	texture = Texture("../../Textures/techo.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID7);
	glBindTexture(GL_TEXTURE_2D, textureID7);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Textura de piel para sillon
	texture = Texture("../../Textures/sillon_piel.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID8);
	glBindTexture(GL_TEXTURE_2D, textureID8);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Textura gris
	texture = Texture("../../Textures/gris.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID9);
	glBindTexture(GL_TEXTURE_2D, textureID9);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Textura cuadro Beso
	texture = Texture("../../Textures/beso.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID10);
	glBindTexture(GL_TEXTURE_2D, textureID10);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Textura pantalla
	texture = Texture("../../Textures/pantalla.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID11);
	glBindTexture(GL_TEXTURE_2D, textureID11);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Textura refri
	texture = Texture("../../Textures/refri.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID12);
	glBindTexture(GL_TEXTURE_2D, textureID12);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Textura estufa
	texture = Texture("../../Textures/estufa.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID13);
	glBindTexture(GL_TEXTURE_2D, textureID13);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Textura librero
	texture = Texture("../../Textures/librero.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID14);
	glBindTexture(GL_TEXTURE_2D, textureID14);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Textura cocina integral
	texture = Texture("../../Textures/cocina_integral.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID15);
	glBindTexture(GL_TEXTURE_2D, textureID15);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Textura Madera mesa
	texture = Texture("../../Textures/madera_clara.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID16);
	glBindTexture(GL_TEXTURE_2D, textureID16);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	glGenTextures(1, &cubeTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		texture = Texture(fileNames[i]);
		FIBITMAP* bitmap = texture.loadImage(true);
		data = texture.convertToData(bitmap, imageWidth, imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		}
		else
			std::cout << "Failed to load texture" << std::endl;
		texture.freeImage(bitmap);
	}

}

void destroyWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void destroy() {
	destroyWindow();

	shaderColor.destroy();
	shaderTexture.destroy();
	shaderCubeTexture.destroy();
	shaderMateriales.destroy();
	shaderDirectionLight.destroy();
	shaderPointLight.destroy();
	shaderPointLight2.destroy();
	shaderSpotLight.destroy();
	sphere.destroy();
	cylinder.destroy();
	box.destroy();
	box2.destroy();
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		case GLFW_KEY_L:
			if (light == true) {
				light = false;
			}
			else
				light = true;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, offsetY, deltaTime);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}
	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->moveFrontCamera(true, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->moveFrontCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->moveRightCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->moveRightCamera(true, deltaTime);
	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	double lastTime = TimeManager::Instance().GetTime();

	glm::vec3 objPosition = glm::vec3(0.0f, 0.0f, -3.0f);
	float angle = 0.0;
	float ratio = 20.0;

	while (psi) {
		psi = processInput(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Matrix de proyeccion en perspectiva
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / screenWidth, 0.01f, 100.0f);
		// matrix de vista
		glm::mat4 view = camera->getViewMatrix();

		glm::mat4 cubeModelMatrix = glm::translate(glm::mat4(1.0f), objPosition);

										
		glm::mat4 lightModelmatrix = glm::rotate(cubeModelMatrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		lightModelmatrix = glm::translate(lightModelmatrix, glm::vec3(0.0f, 0.0f, -ratio));


		if (light) {
													//CREACION DE LAS FIGURAS
			// Iluminación
			shaderSpotLight.turnOn();
			glUniform3fv(shaderSpotLight.getUniformLocation("light.position"), 1, glm::value_ptr(camera->getPosition()));
			glUniform3fv(shaderSpotLight.getUniformLocation("light.direction"), 1, glm::value_ptr(camera->getFront()));
			glUniform3fv(shaderSpotLight.getUniformLocation("viewPos"), 1, glm::value_ptr(camera->getPosition()));
			glUniform3f(shaderSpotLight.getUniformLocation("light.ambient"), 0.2, 0.2, 0.2);
			glUniform3f(shaderSpotLight.getUniformLocation("light.diffuse"), 0.2, 0.3, 0.6);
			glUniform3f(shaderSpotLight.getUniformLocation("light.specular"), 0.5, 0.3, 0.2);
			glUniform3f(shaderSpotLight.getUniformLocation("material.ambient"), 1.0, 0.2, 0.6);
			glUniform3f(shaderSpotLight.getUniformLocation("material.diffuse"), 0.4, 0.5, 0.8);
			glUniform3f(shaderSpotLight.getUniformLocation("material.specular"), 0.5, 0.3, 0.2);
			glUniform1f(shaderSpotLight.getUniformLocation("light.cutOff"), glm::cos(glm::radians(12.5f)));
			glUniform1f(shaderSpotLight.getUniformLocation("light.outerCutOff"), glm::cos(glm::radians(15.0f)));
			glUniform1f(shaderSpotLight.getUniformLocation("light.constant"), 1.0f);
			glUniform1f(shaderSpotLight.getUniformLocation("light.linear"), 0.7f);
			glUniform1f(shaderSpotLight.getUniformLocation("light.quadratics"), 1.8f);
			glUniform1f(shaderSpotLight.getUniformLocation("material.shininess"), 128.0);

															//RENDERIZACION DE LAS FIGURAS

			glBindTexture(GL_TEXTURE_2D, textureID4);		//BASE
			box.setShader(&shaderTexture);
			box.setProjectionMatrix(projection);
			box.setViewMatrix(view);
			box.setPosition(glm::vec3(0.0, -0.8, -20.0));
			box.setScale(glm::vec3(10.0, 0.4, 20.0));
			box.render();		

			box.setShader(&shaderSpotLight);				//pared delantera
			glBindTexture(GL_TEXTURE_2D, textureID2);
			box.setPosition(glm::vec3(0.0, 1.0, -10.0));
			box.setScale(glm::vec3(10.0, 4.0, 0.4));
			box.render(6, 12);
			glBindTexture(GL_TEXTURE_2D, textureID6);
			box.render(0,6);
			box.render(12,36);

			glBindTexture(GL_TEXTURE_2D, textureID1);		//pared izquierda
			box.setPosition(glm::vec3(-5.0, 1.0, -20.0));
			box.setScale(glm::vec3(0.4, 4.0, 20.0));
			box.render();

			glBindTexture(GL_TEXTURE_2D, textureID1);		//pared derecha
			box.setPosition(glm::vec3(5.0, 1.0, -20.0));
			box.setScale(glm::vec3(0.4, 4.0, 20.0));
			box.render();

			glBindTexture(GL_TEXTURE_2D, textureID1);		//pared trasera
			box.setPosition(glm::vec3(0.0, 1.0, -30.0));
			box.setScale(glm::vec3(10.0, 4.0, 0.4));
			box.render();

			glBindTexture(GL_TEXTURE_2D, textureID5);		//pared separacion
			box.setPosition(glm::vec3(-1.0, 1.0, -23.0));
			box.setScale(glm::vec3(8.0, 4.0, 0.4));
			box.render();

			glBindTexture(GL_TEXTURE_2D, textureID7);		//techo
			box.setPosition(glm::vec3(0.0, 3.0, -20.0));
			box.setScale(glm::vec3(12.0, 0.4, 22.0));
			box.render();

			glBindTexture(GL_TEXTURE_2D, textureID16);			//mesa de centro
			cylinder2.setShader(&shaderSpotLight);
			cylinder2.setProjectionMatrix(projection);
			cylinder2.setViewMatrix(view);
			cylinder2.setPosition(glm::vec3(-1.0, -0.3, -18.0));
			cylinder2.setScale(glm::vec3(0.6, 0.6, 0.6));
			cylinder2.render(0, cylinder.getSlices() * cylinder.getStacks() * 2 * 3);
			glBindTexture(GL_TEXTURE_2D, textureID16);
			cylinder2.render(cylinder2.getSlices() * cylinder2.getStacks() * 2 * 3, cylinder2.getSlices() * 3);
			glBindTexture(GL_TEXTURE_2D, textureID16);
			cylinder2.render(cylinder2.getSlices() * cylinder2.getStacks() * 2 * 3 + cylinder2.getSlices() * 3, cylinder2.getSlices() * 3);
		

			glBindTexture(GL_TEXTURE_2D, textureID8);		//sillon grande
			box.setPosition(glm::vec3(-2.5, -0.35, -18.0));
			box.setScale(glm::vec3(1.0, 0.5, 3.0));
			box.render();
			box.setPosition(glm::vec3(-2.85, 0.15, -18.0));
			box.setScale(glm::vec3(0.3, 0.5, 3.0));
			box.render();

			box.setPosition(glm::vec3(0.5, -0.35, -18.0));
			box.setScale(glm::vec3(1.0, 0.5, 3.0));
			box.render();
			box.setPosition(glm::vec3(0.85, 0.15, -18.0));
			box.setScale(glm::vec3(0.3, 0.5, 3.0));
			box.render();

			glBindTexture(GL_TEXTURE_2D, textureID8);		//sillon pequeño
			box.setPosition(glm::vec3(-1.0, -0.35, -16.5));
			box.setScale(glm::vec3(1.0, 0.5, 1.0));
			box.render();

			glBindTexture(GL_TEXTURE_2D, textureID16);		//mueble pantalla
			box.setPosition(glm::vec3(-1.0, 0.15, -22.3));
			box.setScale(glm::vec3(3.0, 1.5, 1.0));
			box.render();

			glBindTexture(GL_TEXTURE_2D, textureID11);		//pantalla
			box.setPosition(glm::vec3(-1.0, 1.7, -22.3));
			box.setScale(glm::vec3(2.5, 1.5, 0.05));
			box.render();

						//librero
			glBindTexture(GL_TEXTURE_2D, textureID14);
			box.setPosition(glm::vec3(-4.2, 1.0, -13.5));
			box.setScale(glm::vec3(1.0, 3.0, 4.0));
			box.render(6, 12);
			glBindTexture(GL_TEXTURE_2D, textureID16);
			box.render(0, 6);
			box.render(12, 36);

			glBindTexture(GL_TEXTURE_2D, textureID10);		//cuadro
			box.setPosition(glm::vec3(-4.8, 1.5, -19.0));
			box.setScale(glm::vec3(0.1, 2.0, 1.5));
			box.render();

			glBindTexture(GL_TEXTURE_2D, textureID15);			//cocina integral
			box.setPosition(glm::vec3(-3.7, 1.0, -26.5));
			box.setScale(glm::vec3(2.0, 3.0, 5.0));
			box.render(6, 12);
			glBindTexture(GL_TEXTURE_2D, textureID9);
			box.render(0, 6);
			box.render(12, 36);

			glBindTexture(GL_TEXTURE_2D, textureID12);		//refri
			box.setPosition(glm::vec3(0.0, 0.6, -23.8));
			box.setScale(glm::vec3(2.0, 2.5, 1.5));
			box.render(12, 18);
			glBindTexture(GL_TEXTURE_2D, textureID9);
			box.render(0, 12);
			box.render(18, 36);

			glBindTexture(GL_TEXTURE_2D, textureID13);		//estufa
			box.setPosition(glm::vec3(0.0, 0.15, -29.1));
			box.setScale(glm::vec3(1.2, 1.5, 1.5));
			box.render(0, 6);
			glBindTexture(GL_TEXTURE_2D, textureID9);
			box.render(6, 36);


			/*	//cubo de prueba
			box.setPosition(glm::vec3(0.0, 0.0, 0.0));
			box.setScale(glm::vec3(1.0, 1.0, 1.0));
			glBindTexture(GL_TEXTURE_2D, textureID1);
			box.render(0,6);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindTexture(GL_TEXTURE_2D, textureID2);
			box.render(6,9);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindTexture(GL_TEXTURE_2D, textureID3);
			box.render(12,18);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindTexture(GL_TEXTURE_2D, textureID4);
			box.render(18,24);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindTexture(GL_TEXTURE_2D, textureID5);
			box.render(24,30);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindTexture(GL_TEXTURE_2D, textureID6);
			box.render(30,36);
			glBindTexture(GL_TEXTURE_2D, 0);
			*/
	
			shaderSpotLight.turnOff();
			glBindTexture(GL_TEXTURE_2D, 0);

		}else{						
													//LUZ POSICIONAL
			
			// Iluminación
			shaderPointLight.turnOn();
			glUniform3fv(shaderPointLight.getUniformLocation("light.position"), 1, glm::value_ptr(glm::vec3(0.0, 5.0, -25.0)));
			glUniform3fv(shaderPointLight.getUniformLocation("light.direction"), 1, glm::value_ptr(glm::vec3(-0.3f, -1.0f, -0.2f)));
			glUniform3fv(shaderPointLight.getUniformLocation("viewPos"), 1, glm::value_ptr(camera->getPosition()));
			glUniform3f(shaderPointLight.getUniformLocation("light.ambient"), 0.2, 0.2, 0.2);
			glUniform3f(shaderPointLight.getUniformLocation("light.diffuse"), 0.2, 0.3, 0.6);
			glUniform3f(shaderPointLight.getUniformLocation("light.specular"), 0.5, 0.3, 0.2);
			glUniform3f(shaderPointLight.getUniformLocation("material.ambient"), 1.0, 0.2, 0.6);
			glUniform3f(shaderPointLight.getUniformLocation("material.diffuse"), 0.4, 0.5, 0.8);
			glUniform3f(shaderPointLight.getUniformLocation("material.specular"), 0.5, 0.3, 0.2);
			glUniform1f(shaderPointLight.getUniformLocation("light.constant"), 1.0f);
			glUniform1f(shaderPointLight.getUniformLocation("light.linear"), 0.7f);
			glUniform1f(shaderPointLight.getUniformLocation("light.quadratics"), 1.8f);
			glUniform1f(shaderPointLight.getUniformLocation("material.shininess"), 32.0);

			//LUZ EXTERNA
			shaderPointLight2.turnOn();
			glUniform3fv(shaderPointLight2.getUniformLocation("light.position"), 1, glm::value_ptr(glm::vec3(0.0, 5.0, -9.0)));
			glUniform3fv(shaderPointLight2.getUniformLocation("light.direction"), 1, glm::value_ptr(glm::vec3(-0.3f, -1.0f, -0.2f)));
			glUniform3fv(shaderPointLight2.getUniformLocation("viewPos"), 1, glm::value_ptr(camera->getPosition()));
			glUniform3f(shaderPointLight2.getUniformLocation("light.ambient"), 0.2, 0.2, 0.2);
			glUniform3f(shaderPointLight2.getUniformLocation("light.diffuse"), 0.2, 0.3, 0.6);
			glUniform3f(shaderPointLight2.getUniformLocation("light.specular"), 0.5, 0.3, 0.2);
			glUniform3f(shaderPointLight2.getUniformLocation("material.ambient"), 1.0, 0.2, 0.6);
			glUniform3f(shaderPointLight2.getUniformLocation("material.diffuse"), 0.4, 0.5, 0.8);
			glUniform3f(shaderPointLight2.getUniformLocation("material.specular"), 0.5, 0.3, 0.2);
			glUniform1f(shaderPointLight2.getUniformLocation("light.constant"), 1.0f);
			glUniform1f(shaderPointLight2.getUniformLocation("light.linear"), 0.7f);
			glUniform1f(shaderPointLight2.getUniformLocation("light.quadratics"), 1.8f);
			glUniform1f(shaderPointLight2.getUniformLocation("material.shininess"), 32.0);


										//RENDERIZACION DE LAS FIGURAS


			glBindTexture(GL_TEXTURE_2D, textureID4);		//BASE
			box.setShader(&shaderTexture);
			box.setProjectionMatrix(projection);
			box.setViewMatrix(view);
			box.setPosition(glm::vec3(0.0, -0.8, -20.0));
			box.setScale(glm::vec3(10.0, 0.4, 20.0));
			box.render();

			box.setShader(&shaderPointLight2);				//pared delantera
			glBindTexture(GL_TEXTURE_2D, textureID2);
			box.setPosition(glm::vec3(0.0, 1.0, -10.0));
			box.setScale(glm::vec3(10.0, 4.0, 0.4));
			box.render(6, 12);
			glBindTexture(GL_TEXTURE_2D, textureID6);
			box.render(0, 6);
			box.render(12, 36);

													//ILUMINACION LUZ INTERNA
			box.setShader(&shaderPointLight);				//pared delantera
			glBindTexture(GL_TEXTURE_2D, textureID2);
			box.setPosition(glm::vec3(0.0, 1.0, -10.0));
			box.setScale(glm::vec3(10.0, 4.0, 0.4));
			box.render(6, 12);
			glBindTexture(GL_TEXTURE_2D, textureID6);
			box.render(0, 6);
			box.render(12, 36);

			glBindTexture(GL_TEXTURE_2D, textureID1);		//pared izquierda
			box.setPosition(glm::vec3(-5.0, 1.0, -20.0));
			box.setScale(glm::vec3(0.4, 4.0, 20.0));
			box.render();

			glBindTexture(GL_TEXTURE_2D, textureID1);		//pared derecha
			box.setPosition(glm::vec3(5.0, 1.0, -20.0));
			box.setScale(glm::vec3(0.4, 4.0, 20.0));
			box.render();

			glBindTexture(GL_TEXTURE_2D, textureID1);		//pared trasera
			box.setPosition(glm::vec3(0.0, 1.0, -30.0));
			box.setScale(glm::vec3(10.0, 4.0, 0.4));
			box.render();

			glBindTexture(GL_TEXTURE_2D, textureID5);		//pared separacion
			box.setPosition(glm::vec3(-1.0, 1.0, -23.0));
			box.setScale(glm::vec3(8.0, 4.0, 0.4));
			box.render();

			glBindTexture(GL_TEXTURE_2D, textureID7);		//techo
			box.setPosition(glm::vec3(0.0, 3.0, -20.0));
			box.setScale(glm::vec3(12.0, 0.4, 22.0));
			box.render();

			glBindTexture(GL_TEXTURE_2D, textureID16);			//mesa de centro
			cylinder2.setShader(&shaderPointLight);
			cylinder2.setProjectionMatrix(projection);
			cylinder2.setViewMatrix(view);
			cylinder2.setPosition(glm::vec3(-1.0, -0.3, -18.0));
			cylinder2.setScale(glm::vec3(0.6, 0.6, 0.6));
			cylinder2.render(0, cylinder.getSlices() * cylinder.getStacks() * 2 * 3);
			glBindTexture(GL_TEXTURE_2D, textureID16);
			cylinder2.render(cylinder2.getSlices() * cylinder2.getStacks() * 2 * 3, cylinder2.getSlices() * 3);
			glBindTexture(GL_TEXTURE_2D, textureID16);
			cylinder2.render(cylinder2.getSlices() * cylinder2.getStacks() * 2 * 3 + cylinder2.getSlices() * 3, cylinder2.getSlices() * 3);


			glBindTexture(GL_TEXTURE_2D, textureID8);		//sillon grande
			box.setPosition(glm::vec3(-2.5, -0.35, -18.0));
			box.setScale(glm::vec3(1.0, 0.5, 3.0));
			box.render();
			box.setPosition(glm::vec3(-2.85, 0.15, -18.0));
			box.setScale(glm::vec3(0.3, 0.5, 3.0));
			box.render();

			box.setPosition(glm::vec3(0.5, -0.35, -18.0));
			box.setScale(glm::vec3(1.0, 0.5, 3.0));
			box.render();
			box.setPosition(glm::vec3(0.85, 0.15, -18.0));
			box.setScale(glm::vec3(0.3, 0.5, 3.0));
			box.render();

			glBindTexture(GL_TEXTURE_2D, textureID8);		//sillon pequeño
			box.setPosition(glm::vec3(-1.0, -0.35, -16.5));
			box.setScale(glm::vec3(1.0, 0.5, 1.0));
			box.render();

			glBindTexture(GL_TEXTURE_2D, textureID16);		//mueble pantalla
			box.setPosition(glm::vec3(-1.0, 0.15, -22.3));
			box.setScale(glm::vec3(3.0, 1.5, 1.0));
			box.render();

			glBindTexture(GL_TEXTURE_2D, textureID11);		//pantalla
			box.setPosition(glm::vec3(-1.0, 1.7, -22.3));
			box.setScale(glm::vec3(2.5, 1.5, 0.05));
			box.render();

			//librero
			glBindTexture(GL_TEXTURE_2D, textureID14);
			box.setPosition(glm::vec3(-4.2, 1.0, -13.5));
			box.setScale(glm::vec3(1.0, 3.0, 4.0));
			box.render(6, 12);
			glBindTexture(GL_TEXTURE_2D, textureID16);
			box.render(0, 6);
			box.render(12, 36);

			glBindTexture(GL_TEXTURE_2D, textureID10);		//cuadro
			box.setPosition(glm::vec3(-4.8, 1.5, -19.0));
			box.setScale(glm::vec3(0.1, 2.0, 1.5));
			box.render();

			box.setShader(&shaderPointLight);

			glBindTexture(GL_TEXTURE_2D, textureID15);			//cocina integral
			box.setPosition(glm::vec3(-3.7, 1.0, -26.5));
			box.setScale(glm::vec3(2.0, 3.0, 5.0));
			box.render(6, 12);
			glBindTexture(GL_TEXTURE_2D, textureID9);
			box.render(0, 6);
			box.render(12, 36);

			glBindTexture(GL_TEXTURE_2D, textureID12);		//refri
			box.setPosition(glm::vec3(0.0, 0.6, -23.8));
			box.setScale(glm::vec3(2.0, 2.5, 1.5));
			box.render(12, 18);
			glBindTexture(GL_TEXTURE_2D, textureID9);
			box.render(0, 12);
			box.render(18, 36);

			glBindTexture(GL_TEXTURE_2D, textureID13);		//estufa
			box.setPosition(glm::vec3(0.0, 0.15, -29.1));
			box.setScale(glm::vec3(1.2, 1.5, 1.5));
			box.render(0, 6);
			glBindTexture(GL_TEXTURE_2D, textureID9);
			box.render(6, 36);

//			shaderPointLight.turnOff();
			glBindTexture(GL_TEXTURE_2D, 0);

						//LUZ EXTERNA
			shaderPointLight2.turnOn();
			glUniform3fv(shaderPointLight2.getUniformLocation("light.position"), 1, glm::value_ptr(glm::vec3(0.0, 5.0, -5.0)));
			glUniform3fv(shaderPointLight2.getUniformLocation("light.direction"), 1, glm::value_ptr(glm::vec3(-0.3f, -1.0f, -0.2f)));
			glUniform3fv(shaderPointLight2.getUniformLocation("viewPos"), 1, glm::value_ptr(camera->getPosition()));
			glUniform3f(shaderPointLight2.getUniformLocation("light.ambient"), 0.2, 0.2, 0.2);
			glUniform3f(shaderPointLight2.getUniformLocation("light.diffuse"), 0.2, 0.3, 0.6);
			glUniform3f(shaderPointLight2.getUniformLocation("light.specular"), 0.5, 0.3, 0.2);
			glUniform3f(shaderPointLight2.getUniformLocation("material.ambient"), 1.0, 0.2, 0.6);
			glUniform3f(shaderPointLight2.getUniformLocation("material.diffuse"), 0.4, 0.5, 0.8);
			glUniform3f(shaderPointLight2.getUniformLocation("material.specular"), 0.5, 0.3, 0.2);
			glUniform1f(shaderPointLight2.getUniformLocation("light.constant"), 1.0f);
			glUniform1f(shaderPointLight2.getUniformLocation("light.linear"), 0.7f);
			glUniform1f(shaderPointLight2.getUniformLocation("light.quadratics"), 1.8f);
			glUniform1f(shaderPointLight2.getUniformLocation("material.shininess"), 32.0);

		
		
			shaderPointLight.turnOff();
			shaderPointLight2.turnOff();
			glBindTexture(GL_TEXTURE_2D, 0);

		}




		if (angle > 2 * M_PI)
			angle = 0.0;
		else
			angle += 0.004;

		/*sphere.setShader(&shaderColor);
		sphere.setColor(glm::vec3(0.4f, 0.3f, 0.6f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
		sphere.enableWireMode();
		sphere.render(lightModelmatrix);
		*/
		//rendrizado de Skybox
		// Se Dibuja el Skybox
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderCubeTexture.turnOn();
		GLuint cubeTextureId = shaderCubeTexture.getUniformLocation("skybox");
		glUniform1f(cubeTextureId, 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		sphere.setShader(&shaderCubeTexture);
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(glm::mat4(glm::mat3(view)));
		sphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));
		sphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);
		shaderCubeTexture.turnOff();

		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}