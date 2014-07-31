//
//  ShadowMapPlugin.cpp
//  Three.cpp Rev.2
//
//  Created by Saburo Okita on 24/07/14.
//  Copyright (c) 2014 Saburo Okita. All rights reserved.
//

#include "ShadowMapPlugin.h"
#include "three.h"

using namespace std;

namespace three {
    ptr<ShadowMapPlugin> ShadowMapPlugin::create() {
        return make_shared<ShadowMapPlugin>();
    }
    
    ShadowMapPlugin::ShadowMapPlugin() :
        frustum         (Frustum::create()),
        projectionScreen(glm::mat4(1.0)),
        min             (glm::vec3(0.0)),
        max             (glm::vec3(0.0)),
        position        (glm::vec3(0.0)),
        depthShader     (SHADERLIB_DEPTH_RGBA)
    {
    }
    
    ShadowMapPlugin::~ShadowMapPlugin() {
    }
    
    void ShadowMapPlugin::init( ptr<Scene> scene, ptr<Camera> camera ) {
        depthShader->compileShader();
        
        descendants = scene->getDescendants();
        
        for( auto entry: scene->directionalLights.getCollection() ) {
            ptr<DirectionalLight> light = entry.second;
            if( !light->castShadow )
                continue;
            
            if( light->shadowCascade ) {
//                for(int i = 0; i < light->shadowCascadeArray.size(); i++ ) {
//                    ptr<VirtualLight> virtual_light;
//                    
//                    if( light->shadowCascadeArray[i] == nullptr  ) {
//                        virtual_light = createVirtualLight( light, i );
//                        virtual_light->originalCamera = camera;
//                        
//                        ptr<Gyroscope> gyro = Gyroscope::create();
//                        gyro->position = light->shadowCascadeOffset;
//                        gyro->add   ( virtual_light );
//                        gyro->add   ( virtual_light->target );
//                        camera->add ( gyro );
//                        
//                        light->shadowCascadeArray[i] = virtual_light;
//                    }
//                    else {
//                        virtual_light = light->shadowCascadeArray[i];
//                    }
//                    
//                    updateVirtualLight(light, i );
//                    lights.push_back( virtual_light );
//                }
            }
            else {
                lights.push_back( light );
            }
        }
        
        for( auto entry: scene->spotLights.getCollection() ) {
            ptr<SpotLight> light = entry.second;
            if( light->castShadow )
                lights.push_back( light );
        }
        
        
//        for( ptr<HemisphereLight> light: scene->hemisphereLights ) {
//            if( light->castShadow )
//                lights.push_back( light );
//        }
//        for( ptr<PointLight> light: scene->pointLights ) {
//            if( light->castShadow )
//                lights.push_back( light );
//        }
        
        for( ptr<Light> light: lights ) {
            if( light->shadowMap == nullptr ) {
                FILTER shadow_filter = FILTER::LINEAR;
                if( shadowMapType == SHADOW_MAP::PCF_SOFT )
                    shadow_filter = FILTER::NEAREST_FILTER;
                
                light->shadowMap     = RenderTarget::create(GL_FRAMEBUFFER, 0);
                light->shadowTexture = ShadowTexture::create(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, static_cast<GLuint>(shadow_filter), static_cast<GLuint>(shadow_filter));
                
                light->shadowMapSize = glm::vec2(1600.0 * 3 / 4, 900.0 * 3 / 4);
                
                light->shadowMap->generateFrameBuffer();
                light->shadowTexture->genTexture();

                light->shadowMap->bind();
                light->shadowTexture->bind();


                glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, light->shadowMapSize.x, light->shadowMapSize.y, 0, GL_RGBA, GL_FLOAT, 0 );

                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLuint>(shadow_filter) );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLuint>(shadow_filter) );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
                
                glGenerateMipmap(GL_TEXTURE_2D);
                
                glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, light->shadowTexture->textureID, 0 );
                GLenum draw_buffers[1] = { GL_COLOR_ATTACHMENT0 };
                glDrawBuffers(1, draw_buffers);
            }
            
            if( light->shadowCamera == nullptr ) {
                if( instance_of(light, SpotLight)) {
                    light->shadowCamera = PerspectiveCamera::create( light->shadowCameraFOV, light->shadowMapSize.x / light->shadowMapSize.y, light->shadowCameraNear, light->shadowCameraFar );
                }
                else if( instance_of(light, DirectionalLight)) {
                    light->shadowCamera = OrthographicCamera::create(-15.0, 15.0, 15.0, -15.0, -30.0, 30.0);
                }
                else {
                    cerr << "Unsupported light type for shadow" << endl;
                    continue;
                }
            }
            
            light->updateMatrixWorld(false);
            auto shadow_map = light->shadowMap;
            auto shadow_cam = light->shadowCamera;
            
            shadow_cam->position = light->position;
            shadow_cam->lookAt( light->target->position );
            
            shadow_cam->updateMatrixWorld(false);
            shadow_cam->matrixWorldInverse = glm::inverse( shadow_cam->matrixWorld );
            light->shadowMatrix = shadow_cam->projection * shadow_cam->matrix;
        }
        
        passthruShader = SHADERLIB_SIMPLE_PASS->clone();
        passthruShader->compileShader();
        
        GLuint quad_vertex_array;
        glGenVertexArrays( 1, &quad_vertex_array );
        glBindVertexArray( quad_vertex_array );
        
        static const GLfloat quad_vertex_buffer_data[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,
        };
        
        
        glGenBuffers( 1, &quad_vertex_buffer );
        glBindBuffer( GL_ARRAY_BUFFER, quad_vertex_buffer );
        glBufferData( GL_ARRAY_BUFFER, sizeof( quad_vertex_buffer_data ), quad_vertex_buffer_data, GL_STATIC_DRAW );
    }
    
    void ShadowMapPlugin::setState( ptr<Scene> scene, ptr<Camera> camera ) {
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glDisable( GL_BLEND );
        glEnable( GL_CULL_FACE );
        glCullFace( GL_BACK );
        
        glEnable( GL_DEPTH_TEST );
    }

    
    void ShadowMapPlugin::render( ptr<Scene> scene, ptr<Arcball> arcball, ptr<Camera> camera ) {
        for( ptr<Light> light: lights ) {
            light->updateMatrixWorld(false);
            
            auto shadow_map = light->shadowMap;
            auto shadow_cam = light->shadowCamera;
            
            shadow_cam->position = light->position * light->quaternion;
            shadow_cam->lookAt( light->target->position );
            
            
            shadow_cam->updateMatrixWorld(false);
            shadow_cam->matrixWorldInverse = glm::inverse( shadow_cam->matrixWorld );
            
            glm::mat4 bias_matrix (
                                   0.5, 0.0, 0.0, 0.0,
                                   0.0, 0.5, 0.0, 0.0,
                                   0.0, 0.0, 0.5, 0.0,
                                   0.5, 0.5, 0.5, 1.0
                                   );
            light->shadowMatrix = bias_matrix * shadow_cam->projection * shadow_cam->matrixWorld;
            light->shadowMap->bind();
            
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//            glViewport(0, 0, 1600.0 * 3 / 4, 900.0 * 3 / 4);
            
            depthShader->bind();
            for( auto object: descendants ){
                object->updateMatrixWorld(false);

                if( instance_of(object, Mesh) == false )
                    continue;

                if( object->castShadow == false )
                    continue;

                depthShader->draw(light->shadowCamera, nullptr, object, false );                
            }
            depthShader->unbind();

//            glBindFramebuffer( GL_FRAMEBUFFER, 0 );
//            
//            glViewport(0, 0, 256, 256);
//            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//            passthruShader->bind();
//            
//            glActiveTexture( GL_TEXTURE0 );
//            glBindTexture( GL_TEXTURE_2D, light->shadowTexture->textureID );
//            passthruShader->shader->setUniform("texture_sampler", 0);
//            
//            glEnableVertexAttribArray( 0 );
//            glBindBuffer( GL_ARRAY_BUFFER, quad_vertex_buffer );
//            glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0 );
//            glDrawArrays( GL_TRIANGLES, 0, 6 );
//            
//            glDisableVertexAttribArray( 0 );
//            passthruShader->unbind();
        }
        
    }
    
    
    ptr<VirtualLight> ShadowMapPlugin::createVirtualLight( ptr<Light> light, int cascade ) {
        ptr<VirtualLight> virtual_light = VirtualLight::create();
        
        virtual_light->isVirtual  = true;
        virtual_light->onlyShadow = true;
        virtual_light->castShadow = true;
        
        virtual_light->shadowCameraNear     = light->shadowCameraNear;
        virtual_light->shadowCameraFar      = light->shadowCameraFar;
        virtual_light->shadowCameraTop      = light->shadowCameraTop;
        virtual_light->shadowCameraBottom   = light->shadowCameraBottom;
        virtual_light->shadowCameraLeft     = light->shadowCameraLeft;
        virtual_light->shadowCameraRight    = light->shadowCameraRight;
        
        virtual_light->shadowCameraVisible  = light->shadowCameraVisible;
        virtual_light->shadowDarkness       = light->shadowDarkness;
        
        virtual_light->shadowBias           = light->shadowCascadeBias[cascade];
        virtual_light->shadowMapSize.x      = light->shadowCascadeWidth[cascade];
        virtual_light->shadowMapSize.y      = light->shadowCascadeHeight[cascade];
        
        float near_z = light->shadowCascadeNearZ[cascade];
        float far_z  = light->shadowCascadeFarZ[cascade];
        
        virtual_light->pointsFrustum[0] = glm::vec3(-1.0, -1.0, near_z);
        virtual_light->pointsFrustum[1] = glm::vec3( 1.0, -1.0, near_z);
        virtual_light->pointsFrustum[2] = glm::vec3(-1.0,  1.0, near_z);
        virtual_light->pointsFrustum[3] = glm::vec3( 1.0,  1.0, near_z);
        
        virtual_light->pointsFrustum[4] = glm::vec3(-1.0, -1.0, far_z);
        virtual_light->pointsFrustum[5] = glm::vec3( 1.0, -1.0, far_z);
        virtual_light->pointsFrustum[6] = glm::vec3(-1.0,  1.0, far_z);
        virtual_light->pointsFrustum[7] = glm::vec3( 1.0,  1.0, far_z);
        
        return virtual_light;
    }
    
    void ShadowMapPlugin::updateVirtualLight( ptr<Light> light, int cascade ) {
        ptr<VirtualLight> virtual_light = light->shadowCascadeArray[cascade];
        
        virtual_light->position         = light->position;
        virtual_light->target->position = light->target->position;
        
        virtual_light->lookAt( virtual_light->target->position );
        
        virtual_light->shadowCameraVisible  = light->shadowCameraVisible;
        virtual_light->shadowDarkness       = light->shadowDarkness;
        virtual_light->shadowBias           = light->shadowBias;
        
        float near_z = light->shadowCascadeNearZ[cascade];
        float far_z  = light->shadowCascadeFarZ[cascade];
        
        virtual_light->pointsFrustum[0].z = near_z;
        virtual_light->pointsFrustum[1].z = near_z;
        virtual_light->pointsFrustum[2].z = near_z;
        virtual_light->pointsFrustum[3].z = near_z;
        
        virtual_light->pointsFrustum[4].z = far_z;
        virtual_light->pointsFrustum[5].z = far_z;
        virtual_light->pointsFrustum[6].z = far_z;
        virtual_light->pointsFrustum[7].z = far_z;
    }
    
    void ShadowMapPlugin::updateShadowCamera( ptr<Camera> camera, ptr<VirtualLight> light ) {
        glm::vec3 min_vec( MAX_FLOAT, MAX_FLOAT, MAX_FLOAT );
        glm::vec3 max_vec( MIN_FLOAT, MIN_FLOAT, MIN_FLOAT );
        
        ptr<OrthographicCamera> shadow_cam = downcast(light->shadowCamera, OrthographicCamera);
        
        for( int i = 0; i < 8; i++ ) {
            glm::vec3 point = light->pointsWorld[i];
            
            point = Projector::unprojectVector(point, camera);
            point = glm::vec3(shadow_cam->matrixWorldInverse * glm::vec4(point, 1.0));
            
            if( point.x < min_vec.x )
                min_vec.x = point.x;
            if( point.x > max_vec.x )
                max_vec.x = point.x;
            
            
            if( point.y < min_vec.y )
                min_vec.y = point.y;
            if( point.y > max_vec.y )
                max_vec.y = point.y;
            
            
            if( point.z < min_vec.z )
                min_vec.z = point.z;
            if( point.z > max_vec.z )
                max_vec.z = point.z;
        }
        
        shadow_cam->left  = min_vec.x;
        shadow_cam->right = max_vec.x;
        shadow_cam->bottom = min_vec.y;
        shadow_cam->top    = max_vec.y;
        shadow_cam->updateProjectionMatrix();
    }
    
}