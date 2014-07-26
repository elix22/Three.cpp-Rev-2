//
//  main.cpp
//  Three.cpp Rev.2
//
//  Created by Saburo Okita on 07/07/14.
//  Copyright (c) 2014 Saburo Okita. All rights reserved.
//

#include <iostream>
#include <vector>
#include <string>
#include "three/three.h"
#include "examples/examples_header.h"

using namespace std;
using namespace three;

int main(int argc, const char * argv[])
{
//    Ex_001_SimplePrimitives::create()->run();
//    Ex_002_TextureSpecularNormalMappings::create()->run();
//    Ex_003_EnvironmentMapping::create()->run();
//    exit(1);
    
    const string path = "/Users/saburookita/Personal Projects/Three.cpp Rev.2/examples/assets/";
    
    Renderer renderer;
    renderer.init( "Ex 004: Shadow Mapping?", 1600 * 3 / 4, 900 * 3 / 4 );
    
    /* Create scene */
    auto scene = Scene::create();
    scene->fog = Fog::create( 0x72645b / 2, 2.0, 15.0 );
    
    /* Create camera */
    auto camera = PerspectiveCamera::create( 50.0, renderer.aspectRatio, 0.001, 100.0 );
    camera->position = glm::vec3(0.0, 0.0, 5.5);
    camera->lookAt( 0.0, 0.0, 0.0 );
    
    
    auto sphere_1 = Mesh::create( SphereGeometry::create(30, 20, 0.66f ),
                                  PhongMaterial::create(0x777777, 0x777777, 0x0, 0x999999, 30) );
    sphere_1->translate(-1.0, -.0, 0.0);
    
    auto cube_2 = Mesh::create( CubeGeometry::create(1.0),
                                PhongMaterial::create(0x777777, 0x777777, 0x0, 0x999999, 30) );
    cube_2->translate(+1.0, 0.0, 0.0);
    
    scene->add( sphere_1 );
    scene->add( cube_2 );
    
    
    auto env = Mesh::create( CubeGeometry::create(20.0f), MeshCubeMapMaterial::create() );
    env->texture = TextureUtils::loadAsEnvMap( path + "cube/pisa",
                                              "nx.png", "ny.png", "nz.png",
                                              "px.png", "py.png", "pz.png");
    
    sphere_1->envMap = downcast(env->texture, EnvMap);
    cube_2->envMap   = downcast(env->texture, EnvMap);
    scene->add( env );
    
    
    
    /* Create directional light */
    auto dir_light = DirectionalLight::create(0x99CCFF, 1.35, glm::vec3(3.0, 1.0, 3.0) );
    scene->add( dir_light );
    
    /* Create an ambient light */
    scene->add( AmbientLight::create(0x777777));
    
    /* Create a post render callback for objects rotation */
    bool rotate_objects = false;
    float light_rotation_1 = 0.0;
    renderer.setPostRenderCallbackHandler( [&](){
        dir_light->position.x = 2.0 * cosf( light_rotation_1 );
        dir_light->position.z = 2.0 * sinf( light_rotation_1 );
        
        light_rotation_1 += 0.01;
        
        if( rotate_objects ) {
            sphere_1->rotateY(-1.0f);
            cube_2->rotateY(-1.0f);
        }
    });
    
    /* Override key callback handler */
    renderer.setKeyCallbackHandler([&](GLFWwindow *window, int key, int scancode, int action, int mod) {
        if( action == GLFW_PRESS ) {
            switch ( key) {
                case GLFW_KEY_ESCAPE: case GLFW_KEY_Q:
                    glfwSetWindowShouldClose( window, GL_TRUE );
                    return;
                    
                case GLFW_KEY_R: /* Toggle rotation */
                    rotate_objects = !rotate_objects;
                    break;
                    
                default:
                    break;
            }
        }
    });
    
    renderer.gammaInput  = true;
    renderer.gammaOutput = true;
    renderer.clearColor = scene->fog->color;
    renderer.render(scene, camera );

    
    return 0;
}

