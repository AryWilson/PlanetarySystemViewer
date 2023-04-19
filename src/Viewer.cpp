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
   float alpha;
};

struct Light{
   vec3 pos;
   vec3 col;
};

struct Planet{
   // position, velocity, redius, texture
   float size;
   float radius;
   float vel;
   std::string texture;
};

class Viewer : public Window {
public:

   Viewer() : Window() {
      eyePos = vec3(7, 0, 0);
      lookPos = vec3(0, 0, 0);
      upDir = vec3(0, 1, 0);
      mesh = PLYMesh("../models/planet.ply");

      radius = 10;
      azimuth = 0;
      elevation = 0;
      material = {0.2f,0.8f,0.5f,10.0f};
      light = {vec3(0.0f,0.0f,0.0f), vec3(1.0f,1.0f,1.0f)};
      initPlanets();
   }

   void setup() {
      
      renderer.loadShader("phong-texture","../shaders/phong-texture.vs","../shaders/phong-texture.vs");
      renderer.loadShader("phong-pixel","../shaders/phong-pixel.vs","../shaders/phong-pixel.vs");
      renderer.loadShader("flat","../shaders/flat.vs","../shaders/flat.vs");
      // renderer.loadCubeMap();
      textures = GetFilenamesInDir("../textures", "png");

      for(int i =0; i<textures.size(); i++){
         renderer.loadTexture(textures[i],"textures/"+textures[i],i);
      }

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

   void initPlanets(){
      Planet a,b,c;
      a.size = 1.0f/12;
      b.size = 1.0f/5;
      c.size = 1.0f/8;
      a.radius = 3.0f;
      b.radius = 6.0f;
      c.radius = 10.0f;
      a.vel = 1.0f;
      b.vel = 0.5;
      c.vel = 0.1f;
      a.texture = "planet";
      b.texture = "planet";
      c.texture = "planet";

      planets.push_back(a);
      planets.push_back(b);
      planets.push_back(c);
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

      renderer.push(); // push identity
      renderer.scale(vec3(1.0f/d, 1.0f/d, 1.0f/d));
      renderer.translate(vec3(-1*bbCentx,-1*bbCenty,-1*bbCentz));
      renderer.push(); // push star matrix

      renderer.mesh(mesh);
      renderer.endShader();

      // ---PLANETS---
      renderer.pop(); // get matrix for star
      renderer.beginShader("phong-pixel");
      if(mesh.hasUV()){
         renderer.beginShader("phong-texture");
      } else {
         renderer.beginShader("phong-pixel");
      }
      float theta = elapsedTime()/20.0f;

      for(int i = 0; i < planets.size(); i++){
         renderer.push(); //save matrix for star
         if(mesh.hasUV()){
            renderer.texture("diffuseTexture", planets[i].texture); //?
         }
         float r = planets[i].radius;
         float v = planets[i].vel;
         float s = planets[i].size;
         renderer.translate(vec3(r*cos(v*theta),0,r*sin(v*theta)));
         renderer.scale(vec3(s, s, s));
         renderer.mesh(mesh);
         renderer.pop(); //reset to str matrix 
      }

      renderer.pop(); // reset to identity

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
      // renderer.setUniform("ModelViewMatrix", renderer.);
      // renderer.setUniform("NormalMatrix", renderer.);
      renderer.setUniform("eyePos", eyePos);
      renderer.setUniform("material.kd", material.kd);
      renderer.setUniform("material.ks", material.ks);
      renderer.setUniform("material.ka", material.ka);
      renderer.setUniform("material.alpha", material.alpha);
      renderer.setUniform("light.pos", light.pos);
      renderer.setUniform("light.col", light.col);

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
   vector<string> textures;
   vector<Planet> planets;
};

// void fnExit(){ }


int main(int argc, char** argv)
{
   // atexit (fnExit);

   Viewer viewer;
   viewer.run();
   return 0;
}

