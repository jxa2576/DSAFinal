#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

//Memory leak detection
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#endif

#include <string>
#include "irrKlang.h"