/*---------------------------------------------------------*/
/* --------------- MuseodeTanques_cGeIHC_20241 ------------*/
/*-----------------         2024-1   ----------------------*/
/* Alumnos:Contreras Torres Edgar Alan 314027618 Grupo 08
   Garces Angulo Carlos Alberto 316113421 Grupo 03
   Herrera Savador Kenia 316086574 Grupo 09                */
/*---------------- Grupo de Teoria 01      ----------------*/

#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>	//main
#include <stdlib.h>		
#include <glm/glm.hpp>	//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>	//Texture

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>

//#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void my_input(GLFWwindow *window);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor *monitors;

void getResolution(void);

// camera
Camera camera(glm::vec3(0.0f, 20.0f, 220.0f));
float MovementSpeed = 0.1f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
		lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, 0.0f);

//// Light
glm::vec3 lightColor = glm::vec3(0.7f);
glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);

glm::vec3 luzPosicional(0.0f, 0.0f, 0.0f);
bool luzOn = false;
float cntColor = 0.0f;
int colorIndx = 0;
const int numColores = 6;
glm::vec3 cicloColores[numColores] = {
	glm::vec3(0.5f, 0.5f, 0.5f),
	glm::vec3(0.5f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.5f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.5f),
	glm::vec3(0.5f, 0.0f, 0.5f),
	glm::vec3(0.5f, 0.5f, 0.0f)
};

glm::vec3 luzPosicional2(0.0f, 0.0f, 0.0f);
bool luz2On = false;

// posiciones tanque2
float	rotTorreta2 = 0.0f,
		movCanonT2 = -20.0f,
		proyectilCompH = 0.0f,
		proyectilCompV = 0.0f;

bool animacionT2 = false;

int recorrido_T2 = 0;

//Variables para tiro parabolico
float	myTime = 0.0f,
vini = 60.0f;

//Keyframes (Manipulación y dibujo)
float	posX = 0.0f,
posZ = 0.0f,
giroTanque3 = 0.0f,
rotTorreta3 = 0.0f,
OCComp = 0.0f;

float	incX = 0.0f,
incZ = 0.0f,
giroTanque3Inc = 0.0f,
rotTorreta3Inc = 0.0f,
OCCompInc = 0.0f;

bool compO = false;

#define MAX_FRAMES 25 // Cuantos cuadros clave va a poder guardar el sistema
int i_max_steps = 60; //cantidad de cuadros intermedios que se generan
int i_curr_steps = 0;
typedef struct _frame // Guardadr los datos
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posZ;		//Variable para PosicionZ
	float giroTanque3;
	float rotTorreta3;
	float OCComp;
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir número en caso de tener Key guardados
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posZ = posZ;
	KeyFrame[FrameIndex].giroTanque3 = giroTanque3;
	KeyFrame[FrameIndex].rotTorreta3 = rotTorreta3;
	KeyFrame[FrameIndex].OCComp = OCComp;

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posZ = KeyFrame[0].posZ;
	giroTanque3 = KeyFrame[0].giroTanque3;
	rotTorreta3 = KeyFrame[0].rotTorreta3;
	OCComp = KeyFrame[0].OCComp;
}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps; //playindex cuadro clave inicaial
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;
	giroTanque3Inc = (KeyFrame[playIndex + 1].giroTanque3 - KeyFrame[playIndex].giroTanque3) / i_max_steps;
	rotTorreta3Inc = (KeyFrame[playIndex + 1].rotTorreta3 - KeyFrame[playIndex].rotTorreta3) / i_max_steps;
	OCCompInc = (KeyFrame[playIndex + 1].OCComp - KeyFrame[playIndex].OCComp) / i_max_steps;;
}


void animate(void)
{

	// Simula sol
	/*cntDir += 0.01f;
	if (cntDir <= 1.5f)
		lightDirection = glm::vec3(1.0f, 0.0f, 0.0f);
	if (cntDir > 1.5f and cntDir <= 3.0f)
		lightDirection = glm::vec3(1.0f, -1.0f, 0.0f);
	if (cntDir > 3.0f and cntDir <= 4.5f)
		lightDirection = glm::vec3(0.0f, -1.0f, 0.0f);
	if (cntDir > 4.5f and cntDir <= 6.0f)
		lightDirection = glm::vec3(-1.0f, -1.0f, 0.0f);
	if (cntDir > 6.0f and cntDir <= 7.5f)
		lightDirection = glm::vec3(-1.0f, 0.0f, 0.0f);
	if (cntDir > 9.0f)
		cntDir = 0.0f; 
	*/
	
	// Ciclo de colores para luz posicional[2]
	/*if (luzOn == true) {
		cntColor += 0.01f;
		if (cntColor > 1.5f) {
			cntColor = 0.0f;
			colorIndx = (colorIndx+1)%numColores; // % mod con el numero de colores
		}
		luzPosicional = cicloColores[colorIndx];
	}*/

	// Animacion tanque 3
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += incX;
			posZ += incZ;
			giroTanque3 += giroTanque3Inc;
			rotTorreta3 += rotTorreta3Inc;
			OCComp += OCCompInc;

			i_curr_steps++;
		}
	}

	//Animacion  tanque 2	
	if (animacionT2)
	{
		if (recorrido_T2 == 0) {
			rotTorreta2 += 0.2f;
			if (rotTorreta2 >= 90.0f) {
				recorrido_T2 = 1;
			}
		}
		else if (recorrido_T2 == 1) {
			movCanonT2 += 0.1f;
			if (movCanonT2 >= -10.0f) {
				recorrido_T2 = 2;
				//animacionT2 = false;
			}
		}
		else if (recorrido_T2 == 2) {
			proyectilCompH = vini * cos(0.1745f) * myTime; // vcos(a)t
			proyectilCompV = (vini * sin(0.1745f)) - (0.5 * 9.81f * myTime * myTime) + 7.5f; // vsen(a)-0.5gt**2+H
			myTime += 0.05f;
			if (proyectilCompV <= -62.0f) {  //myTime == 2*20.0f*sin(0.1745f))/9.81 //proyectilCompV <= 0.0f
				recorrido_T2 = 0;
				myTime = 0.0f;
				animacionT2 = false;
			}
		}
	}
}

void getResolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	// --------------------
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC 20241", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");

	vector<std::string> faces
	{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models 
	// -----------
	Model piso("resources/objects/piso/Piso.obj");
	Model museo("resources/objects/MuseoTexturizado/Museo_Texturizado.obj");
	//Arbol
	Model arbol1("resources/objects/Arbol/Tree.obj");
	Model arbol2("resources/objects/Arbol/Tree.obj");

	//pino
	Model pino("resources/objects/arbolpino/arbolpino.obj");
	
	//roca
	Model roca("resources/objects/Roca/roca.obj");


	//letrero
	Model letrero("resources/objects/Letrero/letrero.obj");
	
	// mesa
	Model mesa1("resources/objects/mesa/mesa1.obj");

	//Bancas
	Model banca1("resources/objects/Banca/banca.obj");
	
	//tanques
	Model tanque1("resources/objects/Tanque1/Tanque1.obj");
	//Model tanque2("resources/objects/Tanque2/Tanque2.obj");
	//Model tanque3("resources/objects/Tanque3/Tanque3.obj");

	Model cuerpoT2("resources/objects/Tanque2/CuerpoT2.obj");
	Model torretaT2("resources/objects/Tanque2/torretaT2.obj");
	Model canonT2("resources/objects/Tanque2/canonT2.obj");
	Model proyectilT2("resources/objects/Tanque2/proyectilT2.obj");

	Model cuerpoT3("resources/objects/Tanque3_3/cuerpoT3.obj");
	Model torretaT3("resources/objects/Tanque3_3/torretaT3.obj");
	Model canonT3("resources/objects/Tanque3_3/canonT3.obj");
	Model compuertaT3("resources/objects/Tanque3_3/compuertaT3.obj");

	ModelAnim Soldado("resources/objects/Soldado/RifleTurn.dae");
	Soldado.initShaders(animShader.ID);

	//Inicialización de KeyFrames
	/*for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
	}*/

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		animate();

		// render
		// ------
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection); // funte de luz direccional no sufre atenuacion
		staticShader.setVec3("dirLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f)); //(tiende al negro)
		staticShader.setVec3("dirLight.diffuse", glm::vec3(0.9f)); //color de la fuente de luz (tiende al blanco)
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f)); // brillo del objeto

		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.008f); // configuracion de atenuacion de la intencidad de luz
		staticShader.setFloat("pointLight[0].linear", 0.00009f); 
		staticShader.setFloat("pointLight[0].quadratic", 0.00032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(-89.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", luzPosicional);
		staticShader.setVec3("pointLight[1].diffuse", luzPosicional);
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 0.01f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.00032f);

		// Para agregar mas funtes de luz modificar #define NUMBER 2 en archivo shader_ligth_mod.fs (configurar la fuentes de luz)
		staticShader.setVec3("pointLight[2].position", glm::vec3(0.0, 0.0f, 80.0f));
		staticShader.setVec3("pointLight[2].ambient", luzPosicional2);
		staticShader.setVec3("pointLight[2].diffuse", luzPosicional2);
		staticShader.setVec3("pointLight[2].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[2].constant", 0.01f);
		staticShader.setFloat("pointLight[2].linear", 0.009f);
		staticShader.setFloat("pointLight[2].quadratic", 0.00032f);

		staticShader.setVec3("spotLight[0].position", glm::vec3(0.0f, 20.0f, 10.0f));
		staticShader.setVec3("spotLight[0].direction", glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(60.0f)));
		staticShader.setFloat("spotLight[0].constant", 1.0f);
		staticShader.setFloat("spotLight[0].linear", 0.0009f);
		staticShader.setFloat("spotLight[0].quadratic", 0.0005f);

		staticShader.setFloat("material_shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp00 = glm::mat4(1.0f);
		glm::mat4 tmp01 = glm::mat4(1.0f);
		glm::mat4 tmp02 = glm::mat4(1.0f);
		glm::mat4 tmp03 = glm::mat4(1.0f);
		glm::mat4 tmp04 = glm::mat4(1.0f);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);


		// -------------------------------------------------------------------------------------------------------------------------
		// Personajes Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projection);
		animShader.setMat4("view", view);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);

		//Soldado
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-65.0f, 0.0f, -40.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.15f));
		animShader.setMat4("model", model);
		Soldado.Draw(animShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		//Piso
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f));
		staticShader.setMat4("model", model);
		piso.Draw(staticShader);

		//Estructura de museo
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f));
		staticShader.setMat4("model", model);
		museo.Draw(staticShader);

		//Arbol derecha
		model = glm::translate(glm::mat4(10.0f), glm::vec3(160.0f, 0.0f, 130.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, -10.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.5f));
		staticShader.setMat4("model", model);
		arbol1.Draw(staticShader);

		//Arbol izquierda
		model = glm::translate(glm::mat4(10.0f), glm::vec3(-160.0f, 0.0f, 130.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, -10.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.5f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		//Arboles de atras
		//1 el que esta enmedio de los dos pequeños
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-175.0f, 0.0f, -255.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		staticShader.setMat4("model", model);
		pino.Draw(staticShader);
		//2 el pequeño de la izq
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-140.0f, 0.0f, -255.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		pino.Draw(staticShader);
		//3
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-175.0f, 0.0f, -225.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		pino.Draw(staticShader);
		//4
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-175.0f, 0.0f, -75.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		staticShader.setMat4("model", model);
		pino.Draw(staticShader);
		//5
		model = glm::translate(glm::mat4(1.0f), glm::vec3(185.0f, 0.0f, -75.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		staticShader.setMat4("model", model);
		pino.Draw(staticShader);

		//roca
		model = glm::translate(glm::mat4(1.0f), glm::vec3(180.0f, 0.0f, 150.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		staticShader.setMat4("model", model);
		roca.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-180.0f, 0.0f, 150.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		staticShader.setMat4("model", model);
		roca.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-145.0f, 0.0f, 150.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		staticShader.setMat4("model", model);
		roca.Draw(staticShader);

		//Letreros
		model = glm::translate(glm::mat4(10.0f), glm::vec3(90.0f, 0.0f, 170.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f));
		staticShader.setMat4("model", model);
		letrero.Draw(staticShader);

		//Bancas
		model = glm::translate(glm::mat4(10.0f), glm::vec3(-80.0f, 0.0f, 90.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		banca1.Draw(staticShader);

		model = glm::translate(glm::mat4(10.0f), glm::vec3(-35.0f, 0.0f, 90.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		banca1.Draw(staticShader);

		model = glm::translate(glm::mat4(10.0f), glm::vec3(63.0f, 0.0f, 90.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		banca1.Draw(staticShader);

		model = glm::translate(glm::mat4(10.0f), glm::vec3(103.0f, 0.0f, 90.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		banca1.Draw(staticShader);

		//mesas
		model = glm::translate(glm::mat4(10.0f), glm::vec3(100.0f, 0.0f, 120.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.20f));
		staticShader.setMat4("model", model);
		mesa1.Draw(staticShader);

		model = glm::translate(glm::mat4(10.0f), glm::vec3(60.0f, 0.0f, 120.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.20f));
		staticShader.setMat4("model", model);
		mesa1.Draw(staticShader);

		model = glm::translate(glm::mat4(10.0f), glm::vec3(-80.0f, 0.0f, 120.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.20f));
		staticShader.setMat4("model", model);
		mesa1.Draw(staticShader);

		model = glm::translate(glm::mat4(10.0f), glm::vec3(-35.0f, 0.0f, 120.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.20f));
		staticShader.setMat4("model", model);
		mesa1.Draw(staticShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Tanques
		// -------------------------------------------------------------------------------------------------------------------------
		// Tanque 1 (estatico)
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, 0.0f, -30.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		staticShader.setMat4("model", model);
		tanque1.Draw(staticShader);

		/*model = glm::translate(glm::mat4(1.0f), glm::vec3(110.0f, 0.0f, -20.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		staticShader.setMat4("model", model);
		tanque2.Draw(staticShader);*/

		/*model = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 0.0f, -150.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		tanque3.Draw(staticShader);*/

		// Tanque 2 (animacion)
		//cuerpo
		model = glm::translate(glm::mat4(1.0f), glm::vec3(130.0f, 0.0f, 0.0f));
		tmp00 = model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		staticShader.setMat4("model", model);
		cuerpoT2.Draw(staticShader);

		//torreta
		model = glm::translate(tmp00, glm::vec3(-5.0f, 13.5f, -8.7f));
		tmp03 = model = glm::rotate(model, glm::radians(rotTorreta2), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		staticShader.setMat4("model", model);
		torretaT2.Draw(staticShader);

		//cañon
		model = glm::translate(tmp03, glm::vec3(0.0f, 7.5f, 11.0f));
		tmp04 = model = glm::rotate(model, glm::radians(movCanonT2), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		staticShader.setMat4("model", model);
		canonT2.Draw(staticShader);

		//proyectil
		model = glm::translate(tmp04, glm::vec3(0.0f, 0.0f, 4.0f));
		model = glm::translate(model, glm::vec3(0.0f, proyectilCompV, proyectilCompH));
		model = glm::scale(model, glm::vec3(0.5f));
		staticShader.setMat4("model", model);
		proyectilT2.Draw(staticShader);

		// Tanque 3 (segunda sala, animacion por cuadros clave)
		//cuerpo
		model = glm::translate(glm::mat4(1.0f), glm::vec3(120.0f, 0.0f, -200.0f));
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));
		model = glm::rotate(model, glm::radians(giroTanque3), glm::vec3(0.0f, 1.0f, 0.0));
		tmp01 = model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		cuerpoT3.Draw(staticShader);

		//torreta
		model = glm::translate(tmp01, glm::vec3(0.0f, 0.0f, 0.0f));
		tmp02 = model = glm::rotate(model, glm::radians(rotTorreta3), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		torretaT3.Draw(staticShader);

		//cañon
		model = glm::translate(tmp02, glm::vec3(0.6f, 22.4f, -18.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		canonT3.Draw(staticShader);

		//compuerta
		model = glm::translate(tmp02, glm::vec3(7.0f, 28.0f, 6.5f));
		model = glm::rotate(model, glm::radians(OCComp), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		compuertaT3.Draw(staticShader);

		
		// -------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario
		// -------------------------------------------------------------------------------------------------------------------------

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, view, projection, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		//std::cout <<"frame time = " << frameTime << " milli sec"<< std::endl;
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	skybox.Terminate();
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);

	//To Configure Model Tanque 2
	//Tanque 2 animation
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		animacionT2 ^= true;
	//reinicio
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		rotTorreta2 = 0.0f,
			movCanonT2 = -20.0f;
		proyectilCompH = 0.0f;
		proyectilCompV = 0.0f;
	}

	//To Configure Model Tanque 3
	if ((glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) && (giroTanque3 <= 10 and giroTanque3 >= -10))
		posX--;
	else if ((glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) && (giroTanque3 <= 100 and giroTanque3 >= 80))
		posZ++;
	else if ((glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) && (giroTanque3 <= 190 and giroTanque3 >= 170))
		posX++;
	else if ((glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) && (giroTanque3 <= 280 and giroTanque3 >= 260))
		posZ--;
	else if ((glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) && (giroTanque3 >= -100 and giroTanque3 <= -80))
		posZ--;
	else if ((glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) && (giroTanque3 >= -190 and giroTanque3 <= -170))
		posX++;
	else if ((glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) && (giroTanque3 >= -280 and giroTanque3 <= -260))
		posZ++;
	if ((glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) && (giroTanque3 <= 10 and giroTanque3 >= -10))
		posX++;
	else if ((glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) && (giroTanque3 <= 100 and giroTanque3 >= 80))
		posZ--;
	else if ((glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) && (giroTanque3 <= 190 and giroTanque3 >= 170))
		posX--;
	else if ((glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) && (giroTanque3 <= 280 and giroTanque3 >= 260))
		posZ++;
	else if ((glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) && (giroTanque3 >= -100 and giroTanque3 <= -80))
		posZ++;
	else if ((glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) && (giroTanque3 >= -190 and giroTanque3 <= -170))
		posX--;
	else if ((glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) && (giroTanque3 >= -280 and giroTanque3 <= -270))
		posZ--;
	if (giroTanque3 == 360 or giroTanque3 == -360)
		giroTanque3 = 0.0f;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		rotTorreta3--;
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		rotTorreta3++;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		giroTanque3--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		giroTanque3++;
	if ((glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) && compO == false) {
		OCComp = 90.0f;
		compO = true;
	}
	else {
		if ((glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) && compO == true) {
			OCComp = 0.0f;
			compO = false;
		}
	}
		
	//To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_M && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}

	//Luces (cambiar teclas)
	/*if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		lightPosition.x++;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		lightPosition.x--;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		lightPosition.y++;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		lightPosition.y--;

	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
		luzPosicional = glm::vec3(0.5f, 0.5f, 0.5f);
		luzOn = true;
		cntColor = 0.0f;
	}*/
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}