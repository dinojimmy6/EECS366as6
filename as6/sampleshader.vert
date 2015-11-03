varying vec3 N;
varying vec3 v;
varying vec4 IPhong;
varying vec4 Icook;
varying vec4 Lposition;
uniform int shadingMode;
uniform int illumMode;
uniform int lightSource;
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
struct my_materials
{   
   vec4 emission;    // Ecm   
   vec4 ambient;     // Acm   
   vec4 diffuse;     // Dcm   
   vec4 specular;    // Scm   
   float shininess;  // Srm
   vec3 F0;
   vec3 Rd;
   vec2 m;
   vec2 w;  
};
uniform my_materials mm_p;
uniform my_materials mm_c;  
uniform my_light_source mls;
void main(void)
{

	vec3 V1 = vec3(gl_ModelViewMatrix * gl_Vertex);       
	vec3 N1 = normalize(gl_NormalMatrix * gl_Normal);
	vec4 Lpos;
	if(lightSource == 1) {
		Lpos = gl_ModelViewMatrix * mls.position;
	}
	else {
		Lpos = vec4(0.0, 0.0, 0.0, 0.0);
	}
	Lposition = Lpos;
	N = N1;
	v = V1;
	//gouraud shading
	if(shadingMode < 1) {
		vec3 L = normalize(Lpos.xyz - V1);
		vec3 V1 = normalize(V1 * -1);
		vec3 H = normalize(L + V1);
		if(illumMode == 0) {
			vec4 Idiff = mm_p.diffuse * mls.intensity * max(dot(N1,L), 0.0);  
			Idiff = clamp(Idiff, 0.0, 1.0); 
			IPhong = mm_p.ambient * mls.ambient +  Idiff + mm_p.specular * mls.intensity * pow(dot(N1, H), mm_p.shininess);
		}
		else {
			//cook model
			float e = float(2.71828);
			float pi = 3.141593;
			float G = min(float(1.0), min(2 * dot(N1, H) * dot(N1, V1) / dot(V1, H), 2 * dot(N1, H) * dot(N1, L) / dot(V1, H)));
			float alpha = acos(dot(N1, H));
			float D1 = pow(e, -1 * pow(tan(alpha)/mm_c.m.x, 2)) / (pow(mm_c.m.x, 2) * pow(cos(alpha), 4));
			float D2 = pow(e, -1 * pow(tan(alpha)/mm_c.m.y, 2)) / (pow(mm_c.m.y, 2) * pow(cos(alpha), 4));
			float D = D1 * mm_c.w.x + D2 * mm_c.w.y;
			vec3 n = (vec3(1.0, 1.0, 1.0) + sqrt(mm_c.F0)) / (vec3(1.0, 1.0, 1.0) - sqrt(mm_c.F0)); 
			vec3 F = (1.0 - pow(1.0 - dot(N1, V1), 5.0)) + n * pow(1.0 - (dot(N1, V1)), 5.0);
			vec3 temp = D * G * F / (pi * dot(L, N1) * dot(V1, N1));
			Icook = mm_c.ambient * mls.ambient + mls.intensity * vec4(mm_c.Rd, 0.0) * dot(N1, L) * mm_c.diffuse + mm_c.specular * vec4(temp, 0.0) * mls.intensity;		
		}
  }
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}