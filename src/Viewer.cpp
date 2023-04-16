//--------------------------------------------------
// Author: Ary Wilson
// Date: 2/18/23
// Description: Loads PLY files in ASCII format
//--------------------------------------------------
#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <vector>
#include "agl/window.h"
#include "plymesh.h"
#include "osutils.h"


using namespace std;
using namespace glm;
using namespace agl;

struct Material{
   float ka;
   float kd;
   float ks;
   vec3 col;
   float alpha;
};

struct Light{
   vec3 pos;
   vec3 col;
};

class Viewer : public Window {
public:

   Viewer() : Window() {
      eyePos = vec3(7, 0, 0);
      lookPos = vec3(0, 0, 0);
      upDir = vec3(0, 1, 0);
      mesh = PLYMesh("../models/sphere.ply");

      radius = 10;
      azimuth = 0;
      elevation = 0;
      material = {0.2f,0.8f,0.5f,vec3(0.4f,0.4f,0.8f),10.0f};
      light = {vec3(0.0f,0.0f,0.0f), vec3(1.0f,1.0f,1.0f)};

   }

   void setup() {
      renderer.loadShader("phong-texture","../shaders/phong-texture.vs","../shaders/phong-texture.vs");
      renderer.loadShader("phong-pixel","../shaders/phong-pixel.vs","../shaders/phong-pixel.vs");
      renderer.loadShader("flat","../shaders/flat.vs","../shaders/flat.vs");
      renderer.loadCubeMap();
      renderer.loadTexture("bricks","textures/bricks.png",0);

   }

   void mouseMotion(int x, int y, int dx, int dy) {
      if (mouseIsDown(GLFW_MOUSE_BUTTON_LEFT)) {
         azimuth -= dx*(0.02f);
         azimuth = fmod(azimuth,2*M_PI);

         elevation += dy*(0.02f);
         // elevation += M_PI_2;
         // elevation = fmod(elevation,M_PI);
         // elevation -= M_PI_2;

         if(elevation>=M_PI_2){elevation = M_PI_2 - 0.01f;}
         if(elevation<=-1*M_PI_2){elevation = -1*M_PI_2 + 0.01f;}
      } 
   }

   void mouseDown(int button, int mods) {
   }

   void mouseUp(int button, int mods) {
   }

   void scroll(float dx, float dy) {
      radius+=dy;
      if(radius<=0){
         radius = 1;
      }
   }

   void keyUp(int key, int mods) {
   }

   void update(){

      float x = radius * sin(azimuth) * cos(elevation);
      float y = radius * sin(elevation);
      float z = radius * cos(azimuth) * cos(elevation);
      eyePos = vec3(x,y,z);
   }

// lookAt() changes the camera position and orientation
// translate() applies a translation transformation
// rotate() applies a rotation transformation
// scale() applies a scale transformation
// loockAt(vec3(0,0,0),lookPos,upDir);

//load in mesh
   void fnExit(){ mesh.clear();}

   void draw() {
      // dt();
      // elapsedTime();
      update();

      // ---STAR---
      renderer.beginShader("flat"); // activates shader with given name
      float aspect = ((float)width()) / height();
      // renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);

      //find bounding box
      vec3 bbMin = mesh.minBounds();
      vec3 bbMax = mesh.maxBounds();
      float bbCentx = (bbMin.x + bbMax.x)/2.0f;
      float bbCenty = (bbMin.y + bbMax.y)/2.0f;
      float bbCentz = (bbMin.z + bbMax.z)/2.0f;
      //translate bounding box to 0,0,0
      float bbXlen = abs(bbMax.x - bbMin.x);
      float bbYlen = abs(bbMax.y - bbMin.y);
      float bbZlen = abs(bbMax.z - bbMin.z);
      float d = std::max(bbXlen,std::max(bbYlen,bbZlen));

      renderer.push();
      renderer.scale(vec3(1.0f/d, 1.0f/d, 1.0f/d));
      renderer.translate(vec3(-1*bbCentx,-1*bbCenty,-1*bbCentz));
      renderer.push();

      renderer.mesh(mesh);
      renderer.endShader();

      // ---PLANETS---
      renderer.pop(); //temp
      renderer.beginShader("phong-pixel");
      // renderer.texture("diffuseTexture", "bricks"); //?

      renderer.translate(vec3(5*cos(elapsedTime()),5*sin(elapsedTime()),0));
      renderer.scale(vec3(1.0f/5, 1.0f/5, 1.0f/5));
      renderer.scale(vec3(1.0f/d, 1.0f/d, 1.0f/d));
      renderer.translate(vec3(-1*bbCentx,-1*bbCenty,-1*bbCentz));


      renderer.mesh(mesh);
      renderer.endShader();

      //scale, rotate, translate
      renderer.lookAt(eyePos,lookPos,upDir);
      renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);



      // begin shader sun
      // set up moodel matrix move sun to center
      // begin shader planets
      // set texture
      // set up moodel matrix (get spheres where they should be)
      //    set up times orbits
      // lookat
      // perspective
      // renderer.mesh(sphere)
      // end shader planets
    

      

      
      renderer.setUniform("ViewMatrix", renderer.viewMatrix());
      renderer.setUniform("ProjMatrix", renderer.projectionMatrix());
      renderer.setUniform("eyePos", eyePos);
      renderer.setUniform("material.kd", material.kd);
      renderer.setUniform("material.ks", material.ks);
      renderer.setUniform("material.ka", material.ka);
      renderer.setUniform("material.col", material.col);
      renderer.setUniform("material.alpha", material.alpha);
      renderer.setUniform("light.pos", light.pos);
      renderer.setUniform("light.col", light.col);

      // all primitives draw here will use the current shader
      renderer.endShader();

   }

protected:
   PLYMesh mesh;
   vec3 eyePos;
   vec3 lookPos;
   vec3 upDir;
   float radius;
   float azimuth; // in [0, 360]
   float elevation; // in [-90, 90]
   Material material;
   Light light;
};

// void fnExit(){ }


int main(int argc, char** argv)
{
   // atexit (fnExit);

   Viewer viewer;
   viewer.run();
   return 0;
}

