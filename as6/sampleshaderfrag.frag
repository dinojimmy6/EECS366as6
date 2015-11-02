
varying vec3 N;
varying vec3 v;
varying vec4 IPhong;
uniform int shadingMode;
struct my_light_source 
{   
   vec4 ambient;              // Aclarri   
   vec4 diffuse;              // Dcli  
   vec4 intensity; 
   vec4 specular;             // Scli   
   vec4 position;             // Ppli   
   vec4 halfVector;           // Derived: Hi   
   vec3 spotDirection;        // Sdli   
   float spotExponent;        // Srli   
   float spotCutoff;          // Crli                              
                              // (range: [0.0,90.0], 180.0)   
   float spotCosCutoff;       // Derived: cos(Crli)                 
                              // (range: [1.0,0.0],-1.0)   
   float constantAttenuation; // K0   
   float linearAttenuation;   // K1   
   float quadraticAttenuation;// K2  
};    

uniform my_light_source mls;
struct my_materials
{   
   vec4 emission;    // Ecm   
   vec4 ambient;     // Acm   
   vec4 diffuse;     // Dcm   
   vec4 specular;    // Scm   
   float shininess;  // Srm  
};  


//my_materials mm;
//mm.ambient = vec4(0.19125, 0.0735, 0.0225, 1.0);
//mm.diffuse = vec4(0.7038, 0.27048, 0.0828, 1.0);
//mm.diffuse = vec4(0.005, 0.005, 0.005,1.0);
//mm.specular = vec4(0.256777, 0.137622, 0.086014, 1.0);
//mm.emission = vec4(0.0,0.0,0.0,0.0);
//mm.shininess = float(12.8);
void main(void)
{
   my_materials mm;
   mm.ambient = vec4(0.19125, 0.0735, 0.0225, 1.0);
   mm.diffuse = vec4(0.7038, 0.27048, 0.0828, 1.0);
   mm.specular = vec4(0.256777, 0.137622, 0.086014, 1.0);
   mm.emission = vec4(0.0,0.0,0.0,0.0);
   mm.shininess = float(12.8);
   if(shadingMode > 0) {
      vec3 L = normalize(mls.position.xyz - v);
      vec3 N1 = normalize(N);
      vec3 V1 = normalize(v * -1);
      vec3 H = normalize(L + V1);
      vec4 Idiff = mm.diffuse * mls.diffuse * max(dot(N1,L), 0.0);  
      Idiff = clamp(Idiff, 0.0, 1.0); 
      vec4 Ispec = mm.ambient * mls.ambient +  Idiff + mm.specular * mls.intensity * pow(dot(N1, H), mm.shininess);
      Ispec = clamp(Ispec, 0.0, 1.0); 
      gl_FragColor = Ispec;
   }
   else {
      gl_FragColor = IPhong;
   }
}