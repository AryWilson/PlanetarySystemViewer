//--------------------------------------------------
// Author: Ary Wilson
// Date: 2/18/23
// Description: launches space viewer
//--------------------------------------------------
#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <vector>
#include "agl/window.h"
#include "plymesh.h"
#include "osutils.h"
#include <cstring>
#include <bits/stdc++.h>
#include <time.h>

using namespace std;
using namespace glm;
using namespace agl;

struct Material
{
   float ka;
   float kd;
   float ks;
   float alpha;
};

struct Light
{
   glm::vec3 pos;
   glm::vec3 col;
};

struct Particle
{
   glm::vec3 pos;
   glm::vec4 color;
   float rot;
   float size;
};

struct Planet
{
   // position, velocity, redius, texture
   float size;
   float radius;
   float vel;
   glm::vec3 position;
   string texture;
   string shader;
   vector<Particle> trail;
};

const int PLANET_COUNT = 6;
const int PARTICLE_COUNT = 100;
const float ORBIT = 10;
const float STAR_SIZE = 1.5;
const bool RANDOM_GENERATION = false;

class Viewer : public Window
{
public:
   Viewer() : Window() {

   }

   void setup() {
      eyePos = vec3(7, 0, 0);
      lookPos = vec3(0, 0, 0);
      upDir = vec3(0, 1, 0);
      mesh = PLYMesh("../models/sphere.ply");
      shaders = {"unlit", "phong-texture", "bumpmap", "gas"};

      radius = 10;
      azimuth = 0;
      elevation = 0;
      update_time = 0;
      material = {0.3f, 0.6f, 0.8f, 15.0f};
      light = {lookPos, vec3(1.0f, 1.0f, 1.0f)};
      single_planet = 0;
      single = false;
      bbCentx = 0;
      bbCenty = 0;
      bbCentz = 0;
      d = 1;

      srand(time(NULL));
      for (string s : shaders) {
         renderer.loadShader(s, "../shaders/" + s + ".vs", "../shaders/" + s + ".fs");
      }
      textures = GetFilenamesInDir("../textures", "png");

      renderer.loadCubemap("space", "../textures/space", 0);

      for (int i = 0; i < textures.size()*2; i+=2){
         renderer.loadTexture(textures[i], "../textures/" + textures[i], i + 1);
         renderer.loadTexture(textures[i+1], "../textures/normal/" + textures[i+1], i + 2);
      } 

      renderer.loadTexture("particle", "../textures/particle/particle.png", textures.size()*2 + 1);

      initPlanets();
      setMeshDim(mesh);
      // renderer.blendMode(agl::ADD);
   }

   void mouseMotion(int x, int y, int dx, int dy) {
      if (mouseIsDown(GLFW_MOUSE_BUTTON_LEFT)) {
         azimuth -= dx * (0.02f);
         azimuth = fmod(azimuth, 2 * M_PI);

         elevation += dy * (0.02f);
         // elevation += M_PI_2;
         // elevation = fmod(elevation,M_PI);
         // elevation -= M_PI_2;

         if (elevation >= M_PI_2) {
            elevation = M_PI_2 - 0.01f;
         }
         if (elevation <= -1 * M_PI_2) {
            elevation = -1 * M_PI_2 + 0.01f;
         }
      }
   }

   void initPlanets() {

      if(RANDOM_GENERATION) {
         float rnd = fmod( (rand()/((float)rand())) , 1.0);
         vector<float> radii;
         for(int i = 0; i< PLANET_COUNT; i++){
            radii.push_back((rnd * (ORBIT - (STAR_SIZE+1))) + STAR_SIZE+1);
            rnd = fmod( (rand()/((float)rand())) , 1.0);
         }
         std::sort(radii.begin(), radii.end());
         for(int i = 0; i< PLANET_COUNT; i++){
            Planet toAdd;
            rnd = fmod( (rand()/((float)rand())) , 1.0);
            toAdd.size = rnd * (0.7 - 0.1) + 0.1;
            if (toAdd.size > 0.4){
               toAdd.shader = "gas";
            } else {
               toAdd.shader = "bumpmap";
            }

            toAdd.radius = radii[i];

            rnd = fmod( (rand()/((float)rand())) , 1.0);
            toAdd.vel = (rnd * (0.2 - 0.05) + 0.05) * (ORBIT + 0.5 - radii[i]);

            int t = rand()%textures.size();
            toAdd.texture = textures[t];

            // cout << toAdd.radius << endl;
            planets.push_back(toAdd);
         }
      } else {
         Planet a, b, c, d, e, f;
         a.size = 1.0f / 5;
         b.size = 1.0f / 8;
         c.size = 1.0f / 6;
         d.size = 1.0f / 3;
         e.size = 1.0f / 5;
         f.size = 1.0f / 8;
         a.radius = 3.0f;
         b.radius = 4.0f;
         c.radius = 5.0f;
         d.radius = 8.0f;
         e.radius = 10.0f;
         f.radius = 11.0f;
         a.position = vec3(0);
         b.position = vec3(0);
         c.position = vec3(0);
         d.position = vec3(0);
         e.position = vec3(0);
         f.position = vec3(0);

         a.vel = 0.6f;
         b.vel = 0.5f;
         c.vel = 0.3f;
         d.vel = 0.2f;
         e.vel = 0.1f;
         f.vel = 0.2f;
         a.texture = "craters.png";
         b.texture = "smoke.png";
         c.texture = "jupiter.png";
         d.texture = "gas.png";
         e.texture = "swirl1.png";
         f.texture = "swirl2.png";
         a.shader = "rock";
         b.shader = "rock";
         c.shader = "gas";
         d.shader = "gas";
         e.shader = "gas";
         f.shader = "gas";
         planets.push_back(a);
         planets.push_back(b);
         planets.push_back(c);
         planets.push_back(d);
         planets.push_back(e);
         planets.push_back(f);
      }

      Particle particle;
      particle.color = vec4(0.6f, 0.6f, 0.0f, 0.5f);
      particle.size = 0.05;
      particle.pos = vec3(3, 0, 0);
      for (int i = 0; i < PLANET_COUNT * PARTICLE_COUNT; i++) {
         planets[i / PARTICLE_COUNT].trail.push_back(particle);
      }
   }
   
   void setMeshDim(PLYMesh mesh) {
      // find bounding box
      vec3 bbMin = mesh.minBounds();
      vec3 bbMax = mesh.maxBounds();
      bbCentx = (bbMin.x + bbMax.x) / 2.0f;
      bbCenty = (bbMin.y + bbMax.y) / 2.0f;
      bbCentz = (bbMin.z + bbMax.z) / 2.0f;
      // translate bounding box to 0,0,0
      float bbXlen = abs(bbMax.x - bbMin.x);
      float bbYlen = abs(bbMax.y - bbMin.y);
      float bbZlen = abs(bbMax.z - bbMin.z);
      d = std::max(bbXlen, std::max(bbYlen, bbZlen));
   }

   vec3 screenToWorld(const vec2& screen){
      vec4 screenPos = vec4(screen,1,1);

      //flip y coord 
      screenPos.y = height() - screenPos.y;

      // convert to canonical view coords
      screenPos.x = 2.0f*((screenPos.x / width()) - 0.5);
      screenPos.y = 2.0f*((screenPos.y / height()) - 0.5);

      //convert the particle position to screen coords
      mat4 projection = renderer.projectionMatrix();
      mat4 view = renderer.viewMatrix();
      
      vec4 worldPos = inverse(projection * view) * screenPos; 

      // convert from homogeneous to ordinary coord
      worldPos.x /= worldPos.w;
      worldPos.y /= worldPos.w;
      worldPos.z /= worldPos.w;
      return vec3(worldPos.x, worldPos.y, worldPos.z);
   }

   float sphereIntercetion(vec3 p0, vec3 v, vec3 c, float r){
      // cout << "center" << c << endl;

      vec3 l = c - p0;
      float s = dot(l,normalize(v));
      float l2 = dot(l,l);
      if (s < 0 && l2 > pow(r,2)){ // sphere is behind us
         return -1;
      }
      float m2 = pow(length(l),2) - pow(s,2);
      if(m2 > pow(r,2)){
         return -1;
      }
      float q = sqrt(pow(r,2) - m2);
      float t = -1;
      if(l2 > pow(r,2)){
         t = s - q;
      } else {
         t = s + q;
      }
      return t;
      // return p0 + t*v; //intersection point
   }

   bool sphereIntercetionBool(vec3 p0, vec3 v, vec3 c, float r){
      // cout << "center" << c << endl;
      r = r+0.5;
      vec3 l = c - p0;
      float s = dot(l,normalize(v));
      float l2 = dot(l,l);
      if (s < 0 && l2 > pow(r,2)){ // sphere is behind us
         return false;
      }
      float m2 = pow(length(l),2) - pow(s,2);
      if(m2 > pow(r,2)){
         return false;
      }
      float q = sqrt(pow(r,2) - m2);
      float t = -1;
      if(l2 > pow(r,2)){
         t = s - q;
      } else {
         t = s + q;
      }
      return true;
   }
   
   void mouseDown(int button, int mods){
      if(!single){
         vec2 mousePos = mousePosition();
         vec3 worldPos = screenToWorld(mousePos);
         // cout << "mosuePos" << mousePos << endl;
         vec3 rayDir = normalize(worldPos - eyePos);
         // cout << "rayPos" << rayDir << endl;

         for (int i = 0; i< planets.size(); i++){
            if(sphereIntercetionBool(eyePos, rayDir, planets[i].position, planets[i].size)){
               single = true;
               single_planet = i;
            }
         }
         
      }
   }


   // void mouseDown(int button, int mods){
   //    if(!single){
   //       vec2 mousePos = mousePosition();
   //       vec3 worldPos = screenToWorld(mousePos);
   //       // cout << "mosuePos" << mousePos << endl;
   //       vec3 rayDir = normalize(worldPos - eyePos);
   //       // cout << "rayPos" << rayDir << endl;

   //       float dists[PLANET_COUNT];
   //       memset(dists, -1, sizeof(dists));
   //       float min = ORBIT*4;

   //       for (int i = 0; i< planets.size(); i++){
   //          cout << planets[i].position << endl;
   //          float dist = sphereIntercetion(eyePos, rayDir, planets[i].position, planets[i].size);
   //          if(dist >= 0){
   //             single = true;
   //             // single_planet = i;
   //             if (dist < min){
   //                min = dist;

   //             }
   //          }
   //       }
   //       if(single){
   //          for(int i = 0; i< PLANET_COUNT; i++){
   //             if(dists[i] >= 0 && dists[i] <= min + 0.0001){
   //                single_planet = i;
   //                break;
   //             }
   //          }
   //       }
   //    }
   // }

   void scroll(float dx, float dy) {
      radius += dy;
      if (radius <= 0) {
         radius = 1;
      }
   }

   void keyUp(int key, int mods) {
      if (key >= 49 && key <= 49 + 6) {
         single = true;
         single_planet = key - 49;
      }
      else if (key == 32) {
         single = false;
      }
   }

   void update() {
      float x = radius * sin(azimuth) * cos(elevation);
      float y = radius * sin(elevation);
      float z = radius * cos(azimuth) * cos(elevation);
      eyePos = vec3(x, y, z);
   }

   void updateTrail(float dt, int idx, vec3 position) {
      bool one = agl::random() > 0.5;
      // vector<Particle> mParticles = planets[i].trail;

      for (int i = 0; i < planets[idx].trail.size(); i++) {

         if (one && planets[idx].trail[i].color.w <= 0) {
            // one new particle
            planets[idx].trail[i].pos = position;
            planets[idx].trail[i].color = vec4(0.6f, 0.6f, 0.0f, 0.5f);
            one = false;
         }
         else
         {
            // updates the opacity
            planets[idx].trail[i].color.w -= dt;
         }
      }
   }

   // render all sprites in pool
   void drawTrail(vector<Particle> mParticles) {
      renderer.beginShader("sprite");
      renderer.texture("image", "particle");
      // renderer.rotate(eyePos.x,vec3(1,0,0));
      // renderer.rotate(eyePos.y,vec3(0,1,0));
      // renderer.rotate(eyePos.z,vec3(0,0,1));
      for (int i = 0; i < mParticles.size(); i++) {
         Particle particle = mParticles[i];
         renderer.sprite(particle.pos, particle.color, particle.size);
      }
      renderer.endShader();
   }

   void drawSingle(Planet planet) {
      // ---SINGLE PLANET
      renderer.beginShader(planet.shader); // activates shader with given name
      // renderer.beginShader("phong-texture");
      // renderer.beginShader("gas"); 
      // renderer.beginShader("bumpmap");

      float aspect = ((float)width()) / height();
      renderer.texture("diffuseTexture", planet.texture);
      renderer.texture("normalMapTexture", planet.texture);

      float theta = elapsedTime();
      float v = planet.vel;
      // float s = planet.size;

      renderer.rotate(v * theta, vec3(0, 1, 0));
      renderer.rotate(glm::radians(90.0f), vec3(1, 0, 0));
      // renderer.scale(vec3(s / d, s / d, s / d));
      renderer.scale(vec3(1.0f / d, 1.0f / d, 1.0f / d));
      renderer.translate(vec3(-1 * bbCentx, -1 * bbCenty, -1 * bbCentz));
      renderer.mesh(mesh);

      renderer.setUniform("ProjMatrix", renderer.projectionMatrix());
      renderer.setUniform("material.kd", material.kd);
      renderer.setUniform("material.ks", material.ks);
      renderer.setUniform("material.ka", material.ka);
      renderer.setUniform("material.alpha", material.alpha);
      renderer.setUniform("light.pos", vec3(1,1,0.5));
      renderer.setUniform("light.col", light.col);
      renderer.setUniform("time", theta);
      renderer.lookAt(eyePos, lookPos, upDir);
      renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);

      renderer.endShader();
      return;
   }

   void drawSpace() {
      // ---STAR---
      renderer.beginShader("unlit"); // activates shader with given name
      float aspect = ((float)width()) / height();
      renderer.push(); // push identity
      renderer.scale(vec3(STAR_SIZE));
      renderer.push(); // push star scale
      
      renderer.scale(vec3(1.0f / d, 1.0f / d, 1.0f / d));
      renderer.translate(vec3(-1 * bbCentx, -1 * bbCenty, -1 * bbCentz));
      renderer.push(); // push mesh matrix

      renderer.mesh(mesh);
      renderer.endShader();

      // ---PLANETS---
      renderer.pop(); // get matrix for star
      renderer.beginShader("phong-texture");

      float theta = elapsedTime();
      float delta = dt();
      update_time += delta;
      bool update = false;
      if (update_time >= 1) {
         update_time = 0;
         update = true;
      }

      for (int i = 0; i < planets.size(); i++) {
         renderer.push();// save matrix for star
         renderer.texture("diffuseTexture", planets[i].texture); //?
         float r = planets[i].radius;
         float v = planets[i].vel;
         float s = planets[i].size;
         glm::vec3 _pos = vec3(r * cos(v * theta), 0, r * sin(v * theta));
         
         renderer.translate(_pos);
         renderer.rotate(glm::radians(90.0f), vec3(1, 0, 0));
         renderer.scale(vec3(s, s, s));

         if (update){
            updateTrail(delta, i, _pos);
         }
         planets[i].position = _pos;
         // planets[i].pos = vec3(renderer.getModelMatrix() * vec4(pos,1));
         // cout << planets[i].position << endl;
         // renderer.sphere();
         renderer.mesh(mesh);
         renderer.pop(); // reset to mesh matrix
      }

      renderer.pop(); // remove star size
      renderer.pop(); // reset to identity
      

      renderer.setUniform("ProjMatrix", renderer.projectionMatrix());
      renderer.setUniform("material.kd", material.kd);
      renderer.setUniform("material.ks", material.ks);
      renderer.setUniform("material.ka", material.ka);
      renderer.setUniform("material.alpha", material.alpha);
      renderer.setUniform("light.pos", light.pos);
      renderer.setUniform("light.col", light.col);
      renderer.lookAt(eyePos, lookPos, upDir);
      renderer.perspective(glm::radians(70.0f), aspect, 0.1f, 50.0f);

      renderer.endShader();
      
      for (int i = 0; i < planets.size(); i++) {
         drawTrail(planets[i].trail);
      }
      return;
   }
   // load in mesh
   void fnExit() { mesh.clear(); }

   void draw() {
      // update campos
      update();
      // ---SPACE---
      renderer.beginShader("cubemap");
      // renderer.texture("cubemap", "space");
      renderer.cubemap("cubemap","space");
      renderer.skybox(ORBIT + 10);
      // renderer.skybox(10);
      renderer.endShader();

      if (single) {
         // ---SINGLE PLANET
         drawSingle(planets[single_planet]);
      }
      else{
         // ---STAR AND PLANETS
         drawSpace();
      }

      // renderer.setUniform("ViewMatrix", renderer.viewMatrix());
      // renderer.setUniform("ProjMatrix", renderer.projectionMatrix());
      // renderer.setUniform("HasUV", mesh.hasUV());
      // renderer.setUniform("ModelViewMatrix", renderer.);
      // renderer.setUniform("NormalMatrix", renderer.);
      // renderer.setUniform("eyePos", eyePos);
      // renderer.setUniform("material.kd", material.kd);
      // renderer.setUniform("material.ks", material.ks);
      // renderer.setUniform("material.ka", material.ka);
      // renderer.setUniform("material.alpha", material.alpha);
      // renderer.setUniform("light.pos", light.pos);
      // renderer.setUniform("light.col", light.col);
      // renderer.lookAt(eyePos,lookPos,upDir);
      // renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);
   }

protected:
   PLYMesh mesh;
   vec3 eyePos;
   vec3 lookPos;
   vec3 upDir;
   float radius;
   float azimuth;   // in [0, 360]
   float elevation; // in [-90, 90]
   Material material;
   Light light;
   std::vector<string> textures;
   std::vector<Planet> planets;
   std::vector<string> shaders;

   float update_time;
   bool single;
   int single_planet;
   float bbCentx;
   float bbCenty;
   float bbCentz;
   float d;

};



// void fnExit(){ }

int main(int argc, char **argv) {
   // atexit (fnExit);

   Viewer viewer;
   viewer.run();
   return 0;
}
