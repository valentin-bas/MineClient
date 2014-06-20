// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclure les en-têtes Windows rarement utilisés
// Fichiers d'en-tête Windows :
#include <windows.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <ctime>
#include <map>
#include <iostream>
#include <vector>
#include <algorithm>


#include "Vector2Int.h"
#include "Vector3Int.h"
#include "shader_utils.h"

#ifndef M_PI
   #define M_PI 3.14159265358979323846
#endif

#include "particle\common\sparticules.h"
#include "particle\common\sparticules_types.h"
#include "particle\common\sparticules_simple_vectormath.h"
#include "particle\common\sparticules_timer.h"

#ifndef V_RETURN
#define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return hr; } }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif
// TODO: faites référence ici aux en-têtes supplémentaires nécessaires au programme
