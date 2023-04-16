#version 400

in vec3 normCol;
out vec4 FragColor;
void main()
{
   FragColor = vec4(normCol, 1.0);

}
