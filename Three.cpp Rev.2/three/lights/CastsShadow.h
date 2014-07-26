//
//  CastsShadow.h
//  Three.cpp Rev.2
//
//  Created by Saburo Okita on 24/07/14.
//  Copyright (c) 2014 Saburo Okita. All rights reserved.
//

#ifndef __Three_cpp_Rev_2__CastsShadow__
#define __Three_cpp_Rev_2__CastsShadow__

#include <iostream>
#include "internal_headers.h"

#include "Camera.h"
#include "RenderTarget.h"

namespace three {
    class CastsShadow {
    public:
        CastsShadow();
        virtual ~CastsShadow();
        
    public:
        ptr<Camera>         shadowCamera;
        ptr<RenderTarget>   shadowMap;
        glm::mat4           shadowMatrix;
        glm::vec2           shadowMapSize;
        bool                shadowCascade;
        bool                shadowCameraVisible;
        
        std::vector<ptr<VirtualLight>> shadowCascadeArray;
        std::vector<float> shadowCascadeBias;
        std::vector<uint> shadowCascadeWidth;
        std::vector<uint> shadowCascadeHeight;
        std::vector<float> shadowCascadeNearZ;
        std::vector<float> shadowCascadeFarZ;
        
        float shadowBias;
        float shadowDarkness;
        
        float shadowCameraFOV;
        float shadowCameraLeft;
        float shadowCameraRight;
        float shadowCameraTop;
        float shadowCameraBottom;
        float shadowCameraNear;
        float shadowCameraFar;
    };
}

#endif /* defined(__Three_cpp_Rev_2__CastsShadow__) */
