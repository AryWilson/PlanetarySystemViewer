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

struct Particle {
  glm::vec3 pos;
  glm::vec4 color;
  float size;
  float time;
};

struct Planet{
   // position, velocity, redius, texture
   float size;
   float radius;
   float vel;
   string texture;
   vector<Particle> trail;
};



class Viewer : public Window {
public:

   Viewer() : Window() {
      eyePos = vec3(7, 0, 0);
      lookPos = vec3(0, 0, 0);
      upDir = vec3(0, 1, 0);
      mesh = PLYMesh("../models/sphere.ply");
      shaders = {"unlit", "phong-texture"};


      radius = 10;
      azimuth = 0;
      elevation = 0;
      material = {0.1f,0.6f,0.8f,15.0f};
      light = {lookPos, vec3(1.0f,1.0f,1.0f)};

      

   }

   void setup() {
      for(string s : shaders){
         renderer.loadShader(s, "../shaders/"+s+".vs", "../shaders/"+s+".fs");
      }
      // renderer.loadCubeMap();
      renderer.loadTexture("planet","textures/bricks.png",0);
      renderer.loadTexture("particle","textures/particle.png",1);

      // textures = GetFilenamesInDir("../textures", "png");
      

      // for(int i =0; i<textures.size(); i++){
      //    renderer.loadTexture(textures[i],"textures/"+textures[i],i);
      // }
      
      initPlanets();

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
      a.size = 1.0f/10;
      b.size = 1.0f/3;
      c.size = 1.0f/5;
      a.radius = 3.0f;
      b.radius = 5.0f;
      c.radius = 7.0f;
      a.vel = 0.5f;
      b.vel = 0.3f;
      c.vel = 0.2f;
      a.texture = "planet";
      b.texture = "planet";
      c.texture = "planet";
      for (int i = 0; i < 10; i++){
         Particle particle;
         particle.color = vec4(1.0f,1.0f,.8f,0.5f);
         particle.size = 0.1;
         particle.pos = vec3(0, 0, 0);
         particle.time = 100;
         a.trail.push_back(particle);
      }
      for (int i = 0; i < 10; i++){
         Particle particle;
         particle.color = vec4(1.0f,1.0f,.8f,0.5f);
         particle.size = 0.1;
         particle.pos = vec3(0, 0, 0);
         particle.time = 100;
         b.trail.push_back(particle);
      }
      for (int i = 0; i < 10; i++){
         Particle particle;
         particle.color = vec4(1.0f,1.0f,.8f,0.5f);
         particle.size = 0.1;
         particle.pos = vec3(1, 0, 0);
         particle.time = 100;
         c.trail.push_back(particle);
      }

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

  void updateConfetti(float dt, vector<Particle> mParticles, vec3 position)
  {
    bool one = agl::random() > 0.25;

    for (int i = 0; i < mParticles.size(); i++){
      
      if(one && mParticles[i].time <=0){
        //one new particle
        mParticles[i].pos = position;
        one = false;

      } else{
        // updates the time
        mParticles[i].time -= dt;
      }
    }
  }

  // render all sprites in pool
  void drawConfetti(vector<Particle> mParticles)
  {
    renderer.texture("image", "particle");
    for (int i = 0; i < mParticles.size(); i++)
    {
      Particle particle = mParticles[i];
      renderer.sprite(particle.pos, particle.color, particle.size);
    }
  }

//load in mesh
   void fnExit(){ mesh.clear();}

   void draw() {
      // dt();
      // elapsedTime();
      update();

      // ---STAR---
      renderer.beginShader("unlit"); // activates shader with given name
      float aspect = ((float)width()) / height();

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

      // renderer.sphere();
      renderer.mesh(mesh);


      renderer.endShader();

      // ---PLANETS---
      renderer.pop(); // get matrix for star
      renderer.beginShader("phong-texture");

      float theta = elapsedTime();
      float delta = dt();

      for(int i = 0; i < planets.size(); i++){
         renderer.push(); //save matrix for star
         renderer.texture("diffuseTexture", planets[i].texture); //?
         float r = planets[i].radius;
         float v = planets[i].vel;
         float s = planets[i].size;
         vec3 pos = vec3(r*cos(v*theta),0,r*sin(v*theta));
         renderer.translate(pos);
         renderer.scale(vec3(s, s, s));
         // renderer.mesh(mesh);
         updateConfetti(delta, planets[i].trail, pos);
         drawConfetti(planets[i].trail);
         // renderer.sphere();
         renderer.mesh(mesh);
         renderer.pop(); //reset to str matrix 
      }

      renderer.pop(); // reset to identity

      

      
      // renderer.setUniform("ViewMatrix", renderer.viewMatrix());
      // renderer.setUniform("ProjMatrix", renderer.projectionMatrix());
      // renderer.setUniform("HasUV", mesh.hasUV());
      // renderer.setUniform("ModelViewMatrix", renderer.);
      // renderer.setUniform("NormalMatrix", renderer.);
      // renderer.setUniform("eyePos", eyePos);
      renderer.setUniform("material.kd", material.kd);
      renderer.setUniform("material.ks", material.ks);
      renderer.setUniform("material.ka", material.ka);
      renderer.setUniform("material.alpha", material.alpha);
      renderer.setUniform("light.pos", light.pos);
      renderer.setUniform("light.col", light.col);
      renderer.lookAt(eyePos,lookPos,upDir);
      renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);
      
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
   vector<string> shaders;

};

// void fnExit(){ }


int main(int argc, char** argv)
{
   // atexit (fnExit);

   Viewer viewer;
   viewer.run();
   return 0;
}

