//--------------------------------------------------
// Author: Ary Wilson
// Date: 2/18/23
// Description: Loads PLY files in ASCII format
//--------------------------------------------------
#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include "plymesh.h"

using namespace std;
using namespace glm;

namespace agl {
   void PLYMesh::clear(){
      _faces.clear();
      _positions.clear();
      _normals.clear();
      _uv.clear();
   }

   PLYMesh::PLYMesh(const std::string& filename) {
      load(filename);
   }

   PLYMesh::PLYMesh() {
      // cout<<"hi"<<endl;
      
   }

//    PLYMesh::PLYMesh(const PLYMesh& orig){
//    // copy constructor
//    _faces = orig.indices();
//    _positions = orig.positions();
//    _normals = orig.normals();
  
// }

//    PLYMesh& PLYMesh::operator=(const PLYMesh& orig) {
//       // assignment operator
//       if (&orig == this) {
//          return *this;
//       }
//       clear();

//       _faces = orig.indices();
//       _positions = orig.positions();
//       _normals = orig.normals();
         
//       return *this;
//    }

   PLYMesh::~PLYMesh() {
      clear();
   }   
   
   void PLYMesh::init() {
      assert(_positions.size() != 0);
      initBuffers(&_faces, &_positions, &_normals, &_uv);
   }

   bool PLYMesh::load(const std::string& filename) {
      if (_positions.size() != 0) {
         std::cout << "WARNING: Cannot load different files with the same PLY mesh\n";
         return false;
      }
      fstream file;
      file.open(filename, ios::in);
      if(!file.is_open()){
         cerr << "could not open file" << endl;
         file.close();
         return false;
      }
      string line;
      getline(file,line);
      if(line.compare("ply")!=0){
         cerr << "mesh file not ply" << endl;
         file.close();
         return false;
      }
      int nvertices;
      int nfaces;
      regex re_v("element vertex ([0-9]+)");
      regex re_f("element face ([0-9]+)");
      regex end_h("end_header");
      regex re_vn("(-?[0-9]+\\.?[0-9]*) (-?[0-9]+\\.?[0-9]*) (-?[0-9]+\\.?[0-9]*) (-?[0-9]+\\.?[0-9]*) (-?[0-9]+\\.?[0-9]*) (-?[0-9]+\\.?[0-9]*)");
      regex re_txtr("(-?[0-9]+\\.?[0-9]*) (-?[0-9]+\\.?[0-9]*) (-?[0-9]+\\.?[0-9]*) (-?[0-9]+\\.?[0-9]*) (-?[0-9]+\\.?[0-9]*) (-?[0-9]+\\.?[0-9]*) (-?[0-9]+\\.?[0-9]*) (-?[0-9]+\\.?[0-9]*)");
      regex re_fn("3 (-?[0-9]+) (-?[0-9]+) (-?[0-9]+)");
      smatch matches;
      
      while(getline(file,line)){
         if(regex_search(line, matches, re_v)) {
            // cout << matches[1] << endl;
            nvertices = stoi(matches[1]);
         }
         if(regex_search(line, matches, re_f)) {
            // cout << matches[1] << endl;
            nfaces = stoi(matches[1]);

         }
         if(regex_search(line, end_h)) {
            // cout << line << endl;

            break;
         }
      }   
      // init();
      //&_faces, &_positions, &_normals      
      for(int i = 0; i<nvertices;){

         if(!getline(file,line)){
            cerr << "too few lines" << endl;
            file.close();
            return false;
         }
         
         if(regex_search(line, matches, re_vn)) {
            i++;
            // cout << matches[0] << endl;

            _positions.push_back(stof(matches[1]));
            _positions.push_back(stof(matches[2]));
            _positions.push_back(stof(matches[3]));
            _normals.push_back(stof(matches[4]));
            _normals.push_back(stof(matches[5]));
            _normals.push_back(stof(matches[6]));
         }

         if(regex_search(line, matches, re_txtr)) {
            i++;
            // cout << matches[0] << endl;

            _positions.push_back(stof(matches[1]));
            _positions.push_back(stof(matches[2]));
            _positions.push_back(stof(matches[3]));
            _normals.push_back(stof(matches[4]));
            _normals.push_back(stof(matches[5]));
            _normals.push_back(stof(matches[6]));
            _uv.push_back(stof(matches[7]));
            _uv.push_back(stof(matches[8]));
         }
      }
      for(int i = 0; i<nfaces;){
         if(!getline(file,line)){
            cerr << "too few lines" << endl;
            file.close();
            return false;
         }
         if(regex_search(line, matches, re_fn)) {
            // cout << matches[0] << endl;
            i++;
            _faces.push_back(stoi(matches[1]));
            _faces.push_back(stoi(matches[2]));
            _faces.push_back(stoi(matches[3]));
         }

      }
      file.close();
      return true;
   }

   glm::vec3 PLYMesh::minBounds() const {
      GLfloat x = INFINITY;
      GLfloat y = INFINITY;
      GLfloat z = INFINITY;
      for(int i = 0; i+2<_positions.size();){
         x = std::min(x,_positions[i++]);
         y = std::min(y,_positions[i++]);
         z = std::min(z,_positions[i++]);
      }
      return glm::vec3(x,y,z);
   }

   glm::vec3 PLYMesh::maxBounds() const {
      GLfloat x = -1*INFINITY;
      GLfloat y = -1*INFINITY;
      GLfloat z = -1*INFINITY;
      for(int i = 0; i+2<_positions.size();){
         x = std::max(x,_positions[i++]);
         y = std::max(y,_positions[i++]);
         z = std::max(z,_positions[i++]);
      }
      return glm::vec3(x,y,z);
   }

   int PLYMesh::numVertices() const {
      return _positions.size()/3;
   }

   int PLYMesh::numTriangles() const {
      return _faces.size()/3;
   }

   const std::vector<GLfloat>& PLYMesh::positions() const {
      return _positions;
   }

   const std::vector<GLfloat>& PLYMesh::normals() const {
      return _normals;
   }

   const std::vector<GLuint>& PLYMesh::indices() const {
      return _faces;
   }

   const vector<GLfloat>& PLYMesh::uv() const {
      return _uv;
   }

   bool PLYMesh::hasUV() const {
      return !(_uv.size() == 0);
   }
}
