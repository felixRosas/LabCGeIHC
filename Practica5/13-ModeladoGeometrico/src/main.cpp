//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"
#include "Headers/Shader.h"
// Geometry primitives
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"

Sphere sphere(20, 20);
Cylinder cylinder(20, 20, 0.5, 0.5);
Cylinder cylinder2(20, 20, 0.6, 0.45);
Box box;

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Shader shader;

int screenWidth;
int screenHeight;

GLFWwindow * window;

bool exitApp = false;
int lastMousePosX;
int lastMousePosY;

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
double planoCercano = 0.01;
float camaraX = 0.0f;
float camaraY = 0.0f;
float camaraZ = -8.0f;


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

	shader.initialize("../../Shaders/transformaciones.vs", "../../Shaders/transformaciones.fs");

	sphere.init();
	sphere.setShader(&shader);
	sphere.setColor(glm::vec3(0.3, 0.3, 1.0));

	cylinder.init();
	cylinder.setShader(&shader);
	cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
	
	cylinder2.init();
	cylinder2.setShader(&shader);
	cylinder2.setColor(glm::vec3(0.8, 0.3, 1.0));

	box.init();
	box.setShader(&shader);
	box.setColor(glm::vec3(0.2, 0.8, 0.4));

}

void destroyWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void destroy() {
	destroyWindow();

	shader.destroy();
	sphere.destroy();
	cylinder.destroy();
	cylinder2.destroy();
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
		case GLFW_KEY_UP:
			camaraY = camaraY - 1;
			break;
		case GLFW_KEY_DOWN:
			camaraY = camaraY + 1;
			break;
		case GLFW_KEY_LEFT:
			camaraX = camaraX + 1;
			break;
		case GLFW_KEY_RIGHT:
			camaraX = camaraX - 1;
			break;
		case GLFW_KEY_Q:
			camaraZ = camaraZ + 1;
			break;
		case GLFW_KEY_W:
			camaraZ = camaraZ - 1;
			break;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	lastMousePosX = xpos;
	lastMousePosY = ypos;
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
	deltaTime = 1 / TimeManager::Instance().CalculateFrameRate(false);
	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	double lastTime = TimeManager::Instance().GetTime();

	while (psi) {
		psi = processInput(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//creamos matriz de proyeccion en perspectiva
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / screenWidth, 0.01f, 100.0f);
		//matriz de vista
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(camaraX, camaraY, camaraZ));	//cambiar ese valor de la camara mediante teclado


		//matriz con diagonal untaria
		//matriz del cilindro del torso
		glm::mat4 matrix0 = glm::mat4(1.0f);
		//se coloca el torso en la coordenada (0,0,-1)
		matrix0 = glm::translate(matrix0, glm::vec3(0.0f, 0.0f, -1.0f));


		//*****************************************	PARTE INFERIOR**************************************************
			//matriz de la esfera 1, se coloca -0.5 en y debajo del torso
			//la colocamos antes de escalar el torso
			glm::mat4 matrixs1 = glm::translate(matrix0, glm::vec3(0.0f, -0.5f, 0.0f));
				//primero debemos trasladar y despues escalar
				//matrixs2 es punto que une al cuerpo con la pierna izquierda.
				glm::mat4 matrixs2 = glm::translate(matrixs1, glm::vec3(-0.225f, 0.0f, 0.0f));
						//pierna izquierda
						glm::mat4 matrix1 = glm::rotate(matrixs2, -0.2f, glm::vec3(0.0f, 0.0f, 1.0f));	//rotacion en z de -0.2
						matrix1 = glm::translate(matrix1, glm::vec3(0.0, -0.4, 0.0));
								//matrixs4 es rodilla izquierda.
								glm::mat4 matrixs4 = glm::translate(matrix1, glm::vec3(0.0f, -0.4f, 0.0f));
										//pegamos antes de escalar matrixs4
										//Generamos nuevo cilindro para pie izquierdo(Matrix2)
										glm::mat4 matrix2 = glm::rotate(matrixs4, 0.3f, glm::vec3(0.0f, 0.0f, 1.0f));
										matrix2 = glm::translate(matrix2, glm::vec3(0.0f, -0.3f, 0.0f));
												//pie
												glm::mat4 matrixs12 = glm::translate(matrix2, glm::vec3(0.0f, -0.3f, 0.0f));
														glm::mat4 matrix7 = glm::rotate(matrixs12, 1.5f, glm::vec3(0.0f, 0.0f, -1.0f));
														matrix7 = glm::translate(matrix7, glm::vec3(0.0f, -0.1f, 0.0f));
														matrix7 = glm::scale(matrix7, glm::vec3(0.1, 0.2, 0.2));
														cylinder.setProjectionMatrix(projection);
														cylinder.setViewMatrix(view);
														cylinder.enableWireMode();
														cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
														cylinder.render(matrix7);
												matrixs12 = glm::scale(matrixs12, glm::vec3(0.1f, 0.1f, 0.1f));
												sphere.setProjectionMatrix(projection);
												sphere.setViewMatrix(view);
												sphere.enableWireMode();
												sphere.render(matrixs12);
										matrix2 = glm::scale(matrix2, glm::vec3(0.1, 0.6, 0.2));
										cylinder.setProjectionMatrix(projection);
										cylinder.setViewMatrix(view);
										cylinder.enableWireMode();
										cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
										cylinder.render(matrix2);
								matrixs4 = glm::scale(matrixs4, glm::vec3(0.1f, 0.1f, 0.1f));
								sphere.setProjectionMatrix(projection);
								sphere.setViewMatrix(view);
								sphere.enableWireMode();
								sphere.render(matrixs4);
						matrix1 = glm::scale(matrix1, glm::vec3(0.15f, 0.8f, 0.15f));
						cylinder.setProjectionMatrix(projection);
						cylinder.setViewMatrix(view);
						cylinder.enableWireMode();
						cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
						cylinder.render(matrix1);
				matrixs2 = glm::scale(matrixs2, glm::vec3(0.1f, 0.1f, 0.1f));
				sphere.setProjectionMatrix(projection);
				sphere.setViewMatrix(view);
				sphere.enableWireMode();
				sphere.render(matrixs2);

				//matrixs3 es la esfera que une cuerpo con pierna derecha
				glm::mat4 matrixs3 = glm::translate(matrixs1, glm::vec3(0.225f, 0.0f, 0.0f));
						//antes de escalar matrixs3
						//pierna izquierda
						glm::mat4 matrix8 = glm::rotate(matrixs3, 0.2f, glm::vec3(0.0f, 0.0f, 1.0f));	//rotacion en z de -0.2
						matrix8 = glm::translate(matrix8, glm::vec3(0.0, -0.4, 0.0));
								//matrixs4 es rodilla izquierda.
								glm::mat4 matrixs13 = glm::translate(matrix8, glm::vec3(0.0f, -0.4f, 0.0f));
								//pegamos antes de escalar matrixs4
										//Generamos nuevo cilindro para pie izquierdo(Matrix2)
										glm::mat4 matrix9 = glm::rotate(matrixs13, 0.3f, glm::vec3(0.0f, 0.0f, -1.0f));
										matrix9 = glm::translate(matrix9, glm::vec3(0.0f, -0.3f, 0.0f));
												//pie
												glm::mat4 matrixs14 = glm::translate(matrix9, glm::vec3(0.0f, -0.3f, 0.0f));
														glm::mat4 matrix10 = glm::rotate(matrixs14, 1.5f, glm::vec3(0.0f, 0.0f, 1.0f));
														matrix10 = glm::translate(matrix10, glm::vec3(0.0f, -0.1f, 0.0f));
														matrix10 = glm::scale(matrix10, glm::vec3(0.1, 0.2, 0.2));
														cylinder.setProjectionMatrix(projection);
														cylinder.setViewMatrix(view);
														cylinder.enableWireMode();
														cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
														cylinder.render(matrix10);
												matrixs14 = glm::scale(matrixs14, glm::vec3(0.1f, 0.1f, 0.1f));
												sphere.setProjectionMatrix(projection);
												sphere.setViewMatrix(view);
												sphere.enableWireMode();
												cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
												sphere.render(matrixs14);
										matrix9 = glm::scale(matrix9, glm::vec3(0.1, 0.6, 0.2));
										cylinder.setProjectionMatrix(projection);
										cylinder.setViewMatrix(view);
										cylinder.enableWireMode();
										cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
										cylinder.render(matrix9);
								matrixs13 = glm::scale(matrixs13, glm::vec3(0.1f, 0.1f, 0.1f));
								sphere.setProjectionMatrix(projection);
								sphere.setViewMatrix(view);
								sphere.enableWireMode();
								sphere.render(matrixs13);
						matrix8 = glm::scale(matrix8, glm::vec3(0.15f, 0.8f, 0.15f));
						cylinder.setProjectionMatrix(projection);
						cylinder.setViewMatrix(view);
						cylinder.enableWireMode();
						cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
						cylinder.render(matrix8);
				matrixs3 = glm::scale(matrixs3, glm::vec3(0.1f, 0.1f, 0.1f));
				sphere.setProjectionMatrix(projection);
				sphere.setViewMatrix(view);
				sphere.enableWireMode();
				sphere.render(matrixs3);
			matrixs1 = glm::scale(matrixs1, glm::vec3(0.1f, 0.1f, 0.1f));
			sphere.setProjectionMatrix(projection);
			sphere.setViewMatrix(view);
			sphere.enableWireMode();
			sphere.render(matrixs1);


	//*****************************************	PARTE SUPERIOR**************************************************
			//esfera enmedio arriba cabeza
			glm::mat4 matrixs5 = glm::translate(matrix0, glm::vec3(0.0f, 0.5f, 0.0f));
					//CABEZA
					glm::mat4 matrix11 = glm::mat4(1.0f);
					matrix11 = glm::translate(matrixs5, glm::vec3(0.0f, 0.5f, 0.0f));
					matrix11 = glm::scale(matrix11, glm::vec3(0.4f, 0.6f, 0.4f));
					cylinder2.setProjectionMatrix(projection);
					cylinder2.setViewMatrix(view);
					cylinder2.enableWireMode();
					cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
					cylinder2.render(matrix11);
					//		BRAZO DERECHO
					//esfera antes del brazo derecho
					glm::mat4 matrixs6 = glm::translate(matrixs5, glm::vec3(0.3f, 0.0f, 0.0f));
							//cilindro brazo izq
							glm::mat4 matrix3 = glm::rotate(matrixs6, -0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
							matrix3 = glm::translate(matrix3, glm::vec3(0.25f, 0.0f, 0.0f));
									//MATRIXS8 es codo derecho
									glm::mat4 matrixs8 = glm::translate(matrix3, glm::vec3(0.3f, 0.0f, 0.0f));
											//Cilindro mano derecha
											glm::mat4 matrix5 = glm::rotate(matrixs8, 0.3f, glm::vec3(0.0f, 0.0f, 1.0f));
											matrix5 = glm::translate(matrix5, glm::vec3(0.0f, -0.3f, 0.0f));
													glm::mat4 matrixs9 = glm::translate(matrix5, glm::vec3(0.0f, -0.3f, 0.0f));
													matrixs9 = glm::scale(matrixs9, glm::vec3(0.1f, 0.1f, 0.1f));
													sphere.setProjectionMatrix(projection);
													sphere.setViewMatrix(view);
													sphere.enableWireMode();
													sphere.render(matrixs9);
											matrix5 = glm::scale(matrix5, glm::vec3(0.1, 0.6, 0.2));
											cylinder.setProjectionMatrix(projection);
											cylinder.setViewMatrix(view);
											cylinder.enableWireMode();
											cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
											cylinder.render(matrix5);
									matrixs8 = glm::scale(matrixs8, glm::vec3(0.1f, 0.1f, 0.1f));
									sphere.setProjectionMatrix(projection);
									sphere.setViewMatrix(view);
									sphere.enableWireMode();
									sphere.render(matrixs8);
							matrix3 = glm::scale(matrix3, glm::vec3(0.5f, 0.15f, 0.15f));
							cylinder.setProjectionMatrix(projection);
							cylinder.setViewMatrix(view);
							cylinder.enableWireMode();
							cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
							cylinder.render(matrix3);
					matrixs6 = glm::scale(matrixs6, glm::vec3(0.1f, 0.1f, 0.1f));
					sphere.setProjectionMatrix(projection);
					sphere.setViewMatrix(view);
					sphere.enableWireMode();
					sphere.render(matrixs6);
					
					//		BRAZO IZQUIERDO
					//esfera antes del brazo izquierdo
					glm::mat4 matrixs7 = glm::translate(matrixs5, glm::vec3(-0.3f, 0.0f, 0.0f));
							//antes de escalamiento
							glm::mat4 matrix4 = glm::rotate(matrixs7, -0.5f, glm::vec3(0.0f, 0.0f, -1.0f));
							matrix4 = glm::translate(matrix4, glm::vec3(-0.25f, 0.0f, 0.0f));
							//MATRIXS9 es codo derecho
							glm::mat4 matrixs10 = glm::translate(matrix4, glm::vec3(-0.3f, 0.0f, 0.0f));
											//Cilindro mano derecha
											glm::mat4 matrix6 = glm::rotate(matrixs10, 0.3f, glm::vec3(0.0f, 0.0f, -1.0f));
											matrix6 = glm::translate(matrix6, glm::vec3(0.0f, -0.3f, 0.0f));
													glm::mat4 matrixs11 = glm::translate(matrix6, glm::vec3(0.0f, -0.3f, 0.0f));
													matrixs11 = glm::scale(matrixs11, glm::vec3(0.1f, 0.1f, 0.1f));
													sphere.setProjectionMatrix(projection);
													sphere.setViewMatrix(view);
													sphere.enableWireMode();
													sphere.render(matrixs11);
											matrix6 = glm::scale(matrix6, glm::vec3(0.1, 0.6, 0.2));
											cylinder.setProjectionMatrix(projection);
											cylinder.setViewMatrix(view);
											cylinder.enableWireMode();
											cylinder.setColor(glm::vec3(0.0, 0.3, 1.0));
											cylinder.render(matrix6);
									matrixs10 = glm::scale(matrixs10, glm::vec3(0.1f, 0.1f, 0.1f));
									sphere.setProjectionMatrix(projection);
									sphere.setViewMatrix(view);
									sphere.enableWireMode();
									sphere.render(matrixs10);
							matrix4 = glm::scale(matrix4, glm::vec3(0.5f, 0.15f, 0.15f));
							cylinder.setProjectionMatrix(projection);
							cylinder.setViewMatrix(view);
							cylinder.enableWireMode();
							cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
							cylinder.render(matrix4);
					matrixs7 = glm::scale(matrixs7, glm::vec3(0.1f, 0.1f, 0.1f));
					sphere.setProjectionMatrix(projection);
					sphere.setViewMatrix(view);
					sphere.enableWireMode();
					sphere.render(matrixs7);

			matrixs5 = glm::scale(matrixs5, glm::vec3(0.1f, 0.1f, 0.1f));
			sphere.setProjectionMatrix(projection);
			sphere.setViewMatrix(view);
			sphere.enableWireMode();
			sphere.render(matrixs5);
	//**************************************************************************************************************
		//se escala del cilindro del torso
		matrix0 = glm::scale(matrix0, glm::vec3(0.6f, 1.0f, 0.6f));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
		cylinder.render(matrix0);		



		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}