varying vec3 N;
varying vec3 v;
varying vec4 IPhong;
uniform int shadingMode;
//struct my_light_source 
//{   
   //vec4 ambient;              // Aclarri   
   //vec4 diffuse;              // Dcli  
   //vec4 intensity; 
   //vec4 specular;             // Scli   
   //vec4 position;             // Ppli   
//};    
struct my_materials
{   
   vec4 emission;    // Ecm   
   vec4 ambient;     // Acm   
   vec4 diffuse;     // Dcm   
   vec4 specular;    // Scm   
   float shininess;  // Srm  
};  
//uniform my_light_source mls;
void main(void)
{

   vec3 v1 = vec3(gl_ModelViewMatrix * gl_Vertex);       
   vec3 N1 = normalize(gl_NormalMatrix * gl_Normal);
   N = N1;
   v = v1;
   //gouraud shading
   if(shadingMode < 1) {
      my_materials mm;
      mm.ambient = vec4(0.19125, 0.0735, 0.0225, 1.0);
      mm.diffuse = vec4(0.7038, 0.27048, 0.0828, 1.0);
      mm.specular = vec4(0.256777, 0.137622, 0.086014, 1.0);
      mm.emission = vec4(0.0,0.0,0.0,0.0);
      mm.shininess = float(12.8);
      vec3 mls2pos = vec3(7.0, 7.0, 7.0);
      vec4 mls2intensity = vec4(2.55, 2.55, 2.55, 0.0);
      vec4 mls2ambient = vec4(.2, .2, .2, 0.0);
      vec3 L = normalize(mls2pos - v1);
      vec3 V1 = normalize(v1 * -1);
      vec3 H = normalize(L + V1);
      vec4 Idiff = mm.diffuse * mls2intensity * max(dot(N1,L), 0.0);  
      Idiff = clamp(Idiff, 0.0, 1.0); 
      IPhong = mm.ambient * mls2ambient +  Idiff + mm.specular * mls2intensity * pow(dot(N1, H), mm.shininess);
  }
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}