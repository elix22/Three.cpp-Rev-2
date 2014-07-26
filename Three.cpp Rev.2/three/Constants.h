//
//  Constants.h
//  Three.cpp Rev.2
//
//  Created by Saburo Okita on 08/07/14.
//  Copyright (c) 2014 Saburo Okita. All rights reserved.
//

#ifndef __Three_cpp_Rev_2__Constants__
#define __Three_cpp_Rev_2__Constants__

#include <iostream>
#include <vector>
#include <numeric>
#include <glm/glm.hpp>

#define instance_of(var, type) ( std::dynamic_pointer_cast<type>(var) != nullptr )
#define downcast(var, type) std::dynamic_pointer_cast<type>(var)
#define toggle( var ) ( var = !var )
#define enum_to_int( val ) (static_cast<GLint>(val))

/* Use alias declaration instead of macro definitions */
template<typename T>
using ptr = std::shared_ptr<T>;

template<typename T>
using vector2D = std::vector<std::vector<T>>;

static const float MAX_FLOAT = std::numeric_limits<float>::max();
static const float MIN_FLOAT = std::numeric_limits<float>::min();
static const glm::vec3 NULL_VEC3 = glm::vec3( MIN_FLOAT, MIN_FLOAT, MIN_FLOAT );


// FIXME: should use enums
namespace three {
    
    enum class CULL {
        NONE            = 0,
        BACK            = 1,
        FRONT           = 2,
        FRONT_AND_BACK  = 3
    };
    
    enum class FRONT_FACE {
        CW  = 0,
        CCW = 1,
    };
    
    enum class SHADOW_MAP {
        BASIC       = 0,
        PCF         = 1,
        PCF_SOFT    = 2,
    };
    
    enum class SIDE {
        FRONT_SIDE  = 0,
        BACK_SIDE   = 1,
        DOUBLE_SIDE = 2,
    };
    
    enum class SHADING {
        NO_SHADING      = 0,
        FLAT_SHADING    = 1,
        SMOOTH_SHADING  = 2,
    };
    
    enum class COLOR_MODE {
        NONE    = 0,
        FACE    = 1,
        VERTEX  = 2,
    };
    
    enum class BLENDING_MODE {
        NONE        = 0,
        NORMAL      = 1,
        ADDITIVE    = 2,
        SUBTRACTIVE = 3,
        MULTIPLY    = 4,
        CUSTOM      = 5
    };
    
    
    /* CUSTOM BLENDING EQUATIONS */
    static const int AddEquation             = 100;
    static const int SubtractEquation        = 101;
    static const int ReverseSubtractEquation = 102;
    static const int ZeroFactor              = 200;
    static const int OneFactor               = 201;
    static const int SrcColorFactor          = 202;
    static const int OneMinusSrcColorFactor  = 203;
    static const int SrcAlphaFactor          = 204;
    static const int OneMinusSrcAlphaFactor  = 205;
    static const int DstAlphaFactor          = 206;
    static const int OneMinusDstAlphaFactor  = 207;
    
    // custom blending source factors
    //static const int ZeroFactor             = 200;
    //static const int OneFactor              = 201;
    //static const int SrcAlphaFactor         = 204;
    //static const int OneMinusSrcAlphaFactor = 205;
    //static const int DstAlphaFactor         = 206;
    //static const int OneMinusDstAlphaFactor = 207;
    static const int DstColorFactor           = 208;
    static const int OneMinusDstColorFactor   = 209;
    static const int SrcAlphaSaturateFactor   = 210;
    
    // TEXTURE CONSTANTS
    enum class TEXTURE_OPERATION {
        MULTIPLY = 0,
        MIX      = 1,
        ADD      = 2,
    };
    
    
    // Wrapping modes
    static const int RepeatWrapping         = 1000;
    static const int ClampToEdgeWrapping    = 1001;
    static const int MirroredRepeatWrapping = 1002;
    
    // Filters
    enum class FILTER {
        NEAREST_FILTER          = 1003,
        NEAREST_MIPMAP_NEAREST  = 1004,
        NEAREST_MIPMAP_LINEAR   = 1005,
        LINEAR                  = 1006,
        LINEAR_MIPMAP_NEAREST   = 1007,
        LINEAR_MIPMAP_LINEAR    = 1008,
    };
    
    // Data types
    static const int UnsignedByteType  = 1009;
    static const int ByteType          = 1010;
    static const int ShortType         = 1011;
    static const int UnsignedShortType = 1012;
    static const int IntType           = 1013;
    static const int UnsignedIntType   = 1014;
    static const int FloatType         = 1015;
    
    // Pixel types
    //static const int UnsignedByteType    = 1009;
    static const int UnsignedShort4444Type = 1016;
    static const int UnsignedShort5551Type = 1017;
    static const int UnsignedShort565Type  = 1018;
    
    enum class PIXEL_FORMAT {
        ALPHA           = 1019,
        RGB             = 1020,
        RGBA            = 1021,
        LUMINANCE       = 1022,
        LUMINANCE_ALPHA = 1023,
    };
    
    
    // Compressed texture formats
    static const int RGB_S3TC_DXT1_Format  = 2001;
    static const int RGBA_S3TC_DXT1_Format = 2002;
    static const int RGBA_S3TC_DXT3_Format = 2003;
    static const int RGBA_S3TC_DXT5_Format = 2004;
    
    
    static std::function<void()> UVMapping = [](){};
    
    static std::function<void()> CubeReflectionMapping = [](){};
    static std::function<void()> CubeRefractionMapping = [](){};
    
    static std::function<void()> SphericalReflectionMapping = [](){};
    static std::function<void()> SphericalRefractionMapping = [](){};
};


#endif /* defined(__Three_cpp_Rev_2__Constants__) */
