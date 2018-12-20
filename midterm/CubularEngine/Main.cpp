#include "stdafx.h"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "GameEntity.h"
#include "Material.h"
#include "Input.h"
#include "BezierCurve.h"
#include "Interpolate.h"


//methods
void CreateManyCubes(Mesh*, Material*);
void CreateBezierExample(Mesh*, Material*, BezierCurve*);
void UpdateBezierExample(BezierCurve *, GameEntity *);
void UpdateScaleExample(GameEntity *);
void UpdateLERPExample(GameEntity *);
void UpdateSLERPExample(GameEntity *);
void SetupLERPExample(Mesh *, Material *);
void SetupSLERPExample(Mesh *, Material *);
Camera* CreateCamera(glm::vec3 pos, glm::vec3 forward, glm::vec3 up, int width, int height, GLFWwindow *window, bool controllable);
void CheckUpdateCameras();

std::vector<GameEntity*> gameEntities;


//bezier cube example vars
float bezierCubeTime = 0;
float bezierCubeStep = 1.f / 500.f;
bool bezierDirForward = true;

//scaling example
int scalingDir = 0;
float scaleAmount = 1.f;

//interpolation declaration
Interpolate interpolate;

//LERP example
glm::vec3 lerpStart = glm::vec3(50.f, 10.f, 5.f);
glm::vec3 lerpEnd = glm::vec3(60.f, 5.f, 10.f);
float lerpTime = 0;
float lerpStep = 1.f / 100.f;
bool lerpDirForward = true;

//SLERP example
glm::vec3 slerpStart = glm::vec3(70.f, 10.f, 5.f);
glm::vec3 slerpEnd = glm::vec3(80.f, 5.f, 10.f);
float slerpTime = 0;
float slerpStep = 1.f / 100.f;
bool slerpDirForward = true;

std::vector<Camera*> cameras;
int curCamera = 0;
bool cameraSwap = false;

int main()
{

	irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();

	engine->setSoundVolume(0.25f);

	//engine->play2D("../libraries/irrKlang-1.5.0/media/explosion.wav", false);

    {
        //init GLFW
        {
            if (glfwInit() == GLFW_FALSE)
            {
#ifdef _DEBUG
                std::cout << "GLFW failed to initialize" << std::endl;
                _CrtDumpMemoryLeaks();
                std::cin.get();
#endif
                return 1;
            }
        }
#ifdef _DEBUG
        std::cout << "GLFW successfully initialized!" << std::endl;
#endif // _DEBUG

        //create & init window, set viewport
        int width = 1200;
        int height = 800;
        GLFWwindow* window = glfwCreateWindow(width, height, "Babby's First Cube?", nullptr, nullptr);
        {
            if (window == nullptr)
            {
#ifdef _DEBUG
                std::cout << "GLFW failed to create window" << std::endl;
                _CrtDumpMemoryLeaks();
                std::cin.get();
#endif
                glfwTerminate();
                return 1;
            }

            //tells OpenGL to use this window for this thread
            //(this would be more important for multi-threaded apps)
            glfwMakeContextCurrent(window);

            //gets the width & height of the window and specify it to the viewport
            int windowWidth, windowHeight;
            glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
            glViewport(0, 0, windowWidth, windowHeight);
        }
#ifdef _DEBUG
        std::cout << "Window successfully initialized!" << std::endl;
#endif // _DEBUG

        //init GLEW
        {
            if (glewInit() != GLEW_OK)
            {
#ifdef _DEBUG
                std::cout << "GLEW failed to initialize" << std::endl;
                _CrtDumpMemoryLeaks();
                std::cin.get();
#endif
                glfwTerminate();
                return 1;
            }
        }
#ifdef _DEBUG
        std::cout << "GLEW successfully initialized!" << std::endl;
#endif // _DEBUG

        //init the shader program
        //TODO - this seems like a better job for a shader manager
        //       perhaps the Shader class can be refactored to fit a shader program
        //       rather than be a thing for vs and fs
        GLuint shaderProgram = glCreateProgram();
        {

            //create vS and attach to shader program
            Shader *vs = new Shader();
            vs->InitFromFile("../assets/shaders/vertexShader.glsl", GL_VERTEX_SHADER);
            glAttachShader(shaderProgram, vs->GetShaderLoc());

            //create FS and attach to shader program
            Shader *fs = new Shader();
            fs->InitFromFile("../assets/shaders/fragmentShader.glsl", GL_FRAGMENT_SHADER);
            glAttachShader(shaderProgram, fs->GetShaderLoc());

            //link everything that's attached together
            glLinkProgram(shaderProgram);

            GLint isLinked;
            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);
            if (!isLinked)
            {
                char infolog[1024];
                glGetProgramInfoLog(shaderProgram, 1024, NULL, infolog);
#ifdef _DEBUG
                std::cout << "Shader Program linking failed with error: " << infolog << std::endl;
                std::cin.get();
#endif

                // Delete the shader, and set the index to zero so that this object knows it doesn't have a shader.
                glDeleteProgram(shaderProgram);
                glfwTerminate();
                _CrtDumpMemoryLeaks();
                return 1;
            }

            //everything's in the program, we don't need this
            delete fs;
            delete vs;
        }

#ifdef _DEBUG
        std::cout << "Shaders compiled attached, and linked!" << std::endl;
#endif // _DEBUG

		 
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        //init the mesh (a cube)
        //TODO - replace this with model loading
        GLfloat vertices[] = {
            -1.0f,-1.0f,-1.0f, // triangle 1 : begin
            -1.0f,-1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f, // triangle 1 : end
            1.0f, 1.0f,-1.0f, // triangle 2 : begin
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f,-1.0f, // triangle 2 : end
            1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f,-1.0f,
            1.0f,-1.0f,-1.0f,
            1.0f, 1.0f,-1.0f,
            1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f,-1.0f,
            1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f,-1.0f, 1.0f,
            1.0f,-1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f,-1.0f,-1.0f,
            1.0f, 1.0f,-1.0f,
            1.0f,-1.0f,-1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f,-1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f,-1.0f,
            -1.0f, 1.0f,-1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f,-1.0f, 1.0f
        };

		//=================== create a bunch of cubes=============================
		Mesh* myMesh = new Mesh();
		myMesh->InitWithVertexArray(vertices, _countof(vertices), shaderProgram);
		Material* myMaterial = new Material(shaderProgram);
		CreateManyCubes(myMesh, myMaterial);

		//==================== create bezier cubes==================================
		Mesh* bMesh = new Mesh();
		bMesh->InitWithVertexArray(vertices, _countof(vertices), shaderProgram);
		Material* bMat = new Material(shaderProgram);


		glm::vec2 curveStart = glm::vec2(20.f, 10.f);
		BezierCurve* bezierCurve = new BezierCurve(curveStart, glm::vec2(10, 10), glm::vec2(5, 20), glm::vec2(25, 20));
		CreateBezierExample(bMesh, bMat, bezierCurve);

		GameEntity* bezierCube = new GameEntity(
			bMesh,
			bMat,
			glm::vec3(curveStart.x, curveStart.y, 5),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(0.4f, 0.4f, 0.4f),
			glm::vec3(0.13f, 0.73f, 0.27f),
			false,
			glm::vec3(0.f, 0.f, 0.f),
			0,
			"Object"
		);

		gameEntities.push_back(bezierCube);

		//============ create scaling example=============================
		GameEntity* scaleExample = new GameEntity(
			bMesh,
			bMat,
			glm::vec3(40, 5, 5),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(1.f, 1.f, 1.f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			false,
			glm::vec3(0.f, 0.f, 0.f),
			0,
			"Object"
		);

		gameEntities.push_back(scaleExample);

		//================== create lerp example ========================

		SetupLERPExample(bMesh, bMat);

		//moving var
		GameEntity* lerpExample = new GameEntity(
			bMesh,
			bMat,
			lerpStart,
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(0.5f, 0.5f, 0.5f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			false,
			glm::vec3(0.f, 0.f, 0.f),
			0,
			"Object"
		);

		gameEntities.push_back(lerpExample);

		//================== create slerp example ========================

		SetupSLERPExample(bMesh, bMat);

		//moving var
		GameEntity* slerpExample = new GameEntity(
			bMesh,
			bMat,
			slerpStart,
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(0.5f, 0.5f, 0.5f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			false,
			glm::vec3(0.f, 0.f, 0.f),
			0,
			"Object"
		);

		gameEntities.push_back(slerpExample);

		//create floor 
		Mesh* floorMesh = new Mesh();
		floorMesh->InitWithVertexArray(vertices, _countof(vertices), shaderProgram);
		Material* floorMat = new Material(shaderProgram);

		GameEntity* floor = new GameEntity(
			floorMesh,
			floorMat,
			glm::vec3(0.f, -10.f, 0.f),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(100.f, 1.f, 100.f),
			glm::vec3(0.2f, 0.2f, 0.2f),
			false,
			glm::vec3(100.f, 1.f, 100.f),
			1,
			"Floor"
		);

		gameEntities.push_back(floor);


		Input::GetInstance()->Init(window);

		//=====================================setup cameras==========================================
		Camera* freeCam = CreateCamera(
			glm::vec3(0.0f, 0.0f, -20.f),
			glm::vec3(0.0f, 0.0f, 1.f),
			glm::vec3(0.0f, 1.f, 0.0f),
			width,
			height,
			window,
			true);

		cameras.push_back(freeCam);

		Camera* camBezier = CreateCamera(
			glm::vec3(15.f, 15.f, -20.f),
			glm::vec3(0.0f, 0.0f, 1.f),
			glm::vec3(0.0f, 1.f, 0.0f),
			width,
			height,
			window,
			false);

		Camera* camScale = CreateCamera(
			glm::vec3(40.f, 5.f, -20.f),
			glm::vec3(0.0f, 0.0f, 1.f),
			glm::vec3(0.0f, 1.f, 0.0f),
			width,
			height,
			window,
			false);

		Camera* camLERP = CreateCamera(
			glm::vec3(55.f, 7.5f, -20.f),
			glm::vec3(0.0f, 0.0f, 1.f),
			glm::vec3(0.0f, 1.f, 0.0f),
			width,
			height,
			window,
			false);

		cameras.push_back(camBezier);
		cameras.push_back(camScale);
		cameras.push_back(camLERP);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);


        //--------------------================================start main loop========================----------------------------
        while (!glfwWindowShouldClose(window))
        {
            /* INPUT */
            {
                //checks events to see if there are pending input
                glfwPollEvents();

				
                //breaks out of the loop if user presses ESC
                if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                {
                    break;
                }
            }

            /* GAMEPLAY UPDATE */

			for (int i = 0; i < gameEntities.size(); i++)
			{
				gameEntities[i]->Update(gameEntities, i, engine);
			}
			gameEntities[0]->ApplyForce(glm::vec3(0.001f, 0.0f, 0.0f));
			cameras[curCamera]->Update();

			//update bezier example
			UpdateBezierExample(bezierCurve, bezierCube);

			//update scaling example
			UpdateScaleExample(scaleExample);

			//update lerp example
			UpdateLERPExample(lerpExample);

			//update slerp example
			UpdateSLERPExample(slerpExample);

			//update cameras
			CheckUpdateCameras();

            /* PRE-RENDER */
            {
                //start off with clearing the 'color buffer'
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glClearColor(0.f, 0.f, 0.f, 1.0f);
            }

            /* RENDER */
			for (int i = 0; i < gameEntities.size(); i++)
			{
				gameEntities[i]->Render(cameras[curCamera]);
			}


            /* POST-RENDER */
            {
                //'clear' for next draw call
                glBindVertexArray(0);
                glUseProgram(0);
                //swaps the front buffer with the back buffer
                glfwSwapBuffers(window);
            }
        }

		//Delete Sound engine
		engine->drop();

        //de-allocate our mesh!
        delete myMesh;
        delete myMaterial;

		delete floorMesh;
		delete floorMat;

		delete bMesh;
		delete bMat;

		for (int i = 0; i < gameEntities.size(); i++)
		{
			delete gameEntities[i];
		}
		//cubeGraph.clear();
		for (int i = 0; i < cameras.size(); i++)
		{
			delete cameras[i];
		}
        Input::Release();
    }

    //clean up
    glfwTerminate();
#ifdef _DEBUG
    _CrtDumpMemoryLeaks();
#endif // _DEBUG
    return 0;
}

void CreateManyCubes(Mesh* myMesh, Material* myMaterial)
{
	srand(time(NULL));

	int cubeCount = 5;

	int maxLoop = 50;
	for (int i = 0; i < cubeCount; i++)
	{
		bool next = true;
		float randomX, randomY, randomZ;
		maxLoop = 50;
		do
		{
			next = true;
			//get a random x and y
			randomX = -13.8f + (static_cast <float> (rand() / (static_cast <float> (RAND_MAX / (13.8f - (-13.8f))))));
			//randomY = (static_cast <float> (rand() / (static_cast <float> (RAND_MAX / (9.7f - (-9.7f))))));
			randomY = 0;
			//randomZ = 5.f + (static_cast <float> (rand() / (static_cast <float> (RAND_MAX / (20.f - (5.f))))));
			randomZ = 0;

			//see if this position will intersect with another cube | if so, remake the position
			for (int j = 0; j < i; j++)
			{
				if (abs(gameEntities[j]->position.x - randomX) < 2.f && abs(gameEntities[j]->position.y - randomY) < 2.f)
				{
					next = false;
					maxLoop--;
					if (maxLoop <= 0)
					{
						next = true;
					}
				}
			}
		} while (!next);

		//create the entity, also giving it a random color
		GameEntity* myGameEntity = new GameEntity(
			myMesh,
			myMaterial,
			glm::vec3(randomX, randomY, randomZ),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(1.f, 1.f, 1.f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			true,
			glm::vec3(1.f, 1.f, 1.f),
			1,
			"Cube"
		);
		gameEntities.push_back(myGameEntity);
	}
}

void CreateBezierExample(Mesh* bMesh, Material* bMat, BezierCurve* bezierCurve)
{
	int pointCount = 100;

	float interval = (float)(1.f / pointCount);
	for (int i = 0; i < pointCount; i++)
	{
		float t = interval * i;
		glm::vec2 pos = bezierCurve->GetPoint(t);
		GameEntity* myGameEntity = new GameEntity(
			bMesh,
			bMat,
			glm::vec3(pos.x, pos.y, 5),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(0.02f, 0.02f, 0.02f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			false,
			glm::vec3(0.f, 0.f, 0.f),
			0,
			"Object"
		);
		gameEntities.push_back(myGameEntity);
	}

	glm::vec2 pos = bezierCurve->GetPoint(0);
	GameEntity* start = new GameEntity(
		bMesh,
		bMat,
		glm::vec3(pos.x, pos.y, 5),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(1.0f, 0.f, 0.0f),
		false,
		glm::vec3(0.f, 0.f, 0.f),
		0,
		"Object"
	);

	pos = bezierCurve->GetPoint(1);
	GameEntity* end = new GameEntity(
		bMesh,
		bMat,
		glm::vec3(pos.x, pos.y, 5),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(0.0f, 1.f, 0.0f),
		false,
		glm::vec3(0.f, 0.f, 0.f),
		0,
		"Object"
	);

	gameEntities.push_back(start);
	gameEntities.push_back(end);
}

void UpdateBezierExample(BezierCurve *bezierCurve, GameEntity *gameObj)
{
	bezierCubeTime += (bezierDirForward) ? bezierCubeStep : -bezierCubeStep;

	if (bezierCubeTime >= 1.f) { bezierCubeTime = 1.f; bezierDirForward = false; }
	else if (bezierCubeTime <= 0) { bezierCubeTime = 0.f; bezierDirForward = true; }


	glm::vec2 newPos = bezierCurve->GetPoint(bezierCubeTime);

	gameObj->position.x = newPos.x;
	gameObj->position.y = newPos.y;
	gameObj->position.z = 5;
}

void UpdateScaleExample(GameEntity *gameObj)
{
	glm::vec3 scaleSet = glm::vec3(1.f, 1.f, 1.f);
	scaleAmount += (scalingDir % 2 == 0) ? 0.01f : -0.01f;

	if (scaleAmount >= 2.f || scaleAmount <= 1.f)
	{
		scaleAmount = (scaleAmount >= 2.f) ? 2.f : 1.f;
		scalingDir++;
		if (scalingDir == 6) { scalingDir = 0; }
	}

	if (scalingDir == 0 || scalingDir == 1)
	{
		scaleSet.x = scaleAmount;
	}
	else if (scalingDir == 2 || scalingDir == 3)
	{
		scaleSet.y = scaleAmount;
	}
	else
	{
		scaleSet.z = scaleAmount;
	}

	gameObj->scale = scaleSet;
	gameObj->eulerAngles.y += 0.009f;
	gameObj->eulerAngles.x += 0.006f;
	gameObj->eulerAngles.z += 0.003f;
}

void SetupLERPExample(Mesh *bMesh, Material *bMat)
{
	int pointCount = 100;
	float step = 1.f / pointCount;

	for (int i = 0; i < pointCount; i++)
	{
		float t = step * i;

		GameEntity* obj = new GameEntity(
			bMesh,
			bMat,
			interpolate.LERP(lerpStart, lerpEnd, t),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(.02f, .02f, .02f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			false,
			glm::vec3(0.f, 0.f, 0.f),
			0,
			"Object"
		);

		gameEntities.push_back(obj);
	}

	//start / end pos
	GameEntity* lerpStartObj = new GameEntity(
		bMesh,
		bMat,
		lerpStart,
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(.1f, .1f, .1f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		false,
		glm::vec3(0.f, 0.f, 0.f),
		0,
		"Object"
	);
	GameEntity* lerpEndObj = new GameEntity(
		bMesh,
		bMat,
		lerpEnd,
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(.1f, .1f, .1f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		false,
		glm::vec3(0.f, 0.f, 0.f),
		0,
		"Object"
	);

	gameEntities.push_back(lerpStartObj);
	gameEntities.push_back(lerpEndObj);
}

void SetupSLERPExample(Mesh *bMesh, Material *bMat)
{
	int pointCount = 100;
	float step = 1.f / pointCount;

	for (int i = 0; i < pointCount; i++)
	{
		float t = step * i;
		glm::vec3 pos = interpolate.SLERP(slerpStart, slerpEnd, t);
		std::cout << "Creating new obj @: [" << pos.x << ", " << pos.y << ", " << pos.z << "]@t: " << t << std::endl;
		GameEntity* obj = new GameEntity(
			bMesh,
			bMat,
			interpolate.SLERP(slerpStart, slerpEnd, t),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(.02f, .02f, .02f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			false,
			glm::vec3(0.f, 0.f, 0.f),
			0,
			"Object"
		);

		gameEntities.push_back(obj);
	}

	//start / end pos
	GameEntity* slerpStartObj = new GameEntity(
		bMesh,
		bMat,
		slerpStart,
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(.1f, .1f, .1f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		false,
		glm::vec3(0.f, 0.f, 0.f),
		0,
		"Object"
	);
	GameEntity* slerpEndObj = new GameEntity(
		bMesh,
		bMat,
		slerpEnd,
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(.1f, .1f, .1f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		false,
		glm::vec3(0.f, 0.f, 0.f),
		0,
		"Object"
	);

	gameEntities.push_back(slerpStartObj);
	gameEntities.push_back(slerpEndObj);
}

void UpdateLERPExample(GameEntity *gameObj)
{
	lerpTime += (lerpDirForward) ? lerpStep : -lerpStep;
	if (lerpTime >= 1.f || lerpTime <= 0.f) { lerpDirForward = !lerpDirForward; }

	glm::vec3 pos = interpolate.LERP(lerpStart, lerpEnd, lerpTime);

	gameObj->position = pos;
}

//TODO: update this SLERP to be rotations instead of movement (oops)
void UpdateSLERPExample(GameEntity *gameObj)
{
	slerpTime += (slerpDirForward) ? slerpStep : -slerpStep;
	if (slerpTime >= 1.f || slerpTime <= 0.f) { slerpDirForward = !slerpDirForward; }

	//glm::vec3 pos = interpolate.SLERP(slerpStart, slerpEnd, slerpTime);

	//gameObj->position = pos;
}

Camera* CreateCamera(glm::vec3 pos, glm::vec3 forward, glm::vec3 up, int width, int height, GLFWwindow *window, bool control)
{
	Camera* camera = new Camera(
		pos,    //position of camera
		forward,     //the 'forward' of the camera
		up,     //what 'up' is for the camera
		60.0f,                          //the field of view in radians
		(float)width,                   //the width of the window in float
		(float)height,                  //the height of the window in float
		0.01f,                          //the near Z-plane
		500.f,							//far z-plane
		window,
		control
	);

	return camera;
}

void CheckUpdateCameras()
{
	if (Input::GetInstance()->IsKeyDown(GLFW_KEY_RIGHT) && !cameraSwap)
	{
		curCamera++;
		if (curCamera >= cameras.size())
		{
			curCamera = 0;
		}
		cameraSwap = true;
	}
	else if (Input::GetInstance()->IsKeyDown(GLFW_KEY_LEFT) && !cameraSwap)
	{
		curCamera--;
		if (curCamera < 0)
		{
			curCamera = cameras.size() - 1;
		}
		cameraSwap = true;
	}

	if (Input::GetInstance()->IsKeyDown(GLFW_KEY_LEFT) == false && Input::GetInstance()->IsKeyDown(GLFW_KEY_RIGHT) == false && cameraSwap)
	{
		cameraSwap = false;
	}

	//do all the changing of tests in here
	
	if (curCamera == 1)//bezier curve
	{

	}
	else if (curCamera == 2)//scaling cube
	{

	}
	else if (curCamera == 3)//lerp
	{

	}
}