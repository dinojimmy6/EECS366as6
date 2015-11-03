
varying vec3 N;
varying vec3 v;
varying vec4 IPhong;
varying vec4 Icook;
varying vec4 Lposition;
uniform int shadingMode;
uniform int illumMode;
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
	mm.ambient = vec4(0.19125, 0.0735, 0.0225, 0.0);
	mm.diffuse = vec4(0.7038, 0.27048, 0.0828, 0.0);
	mm.specular = vec4(0.256777, 0.137622, 0.086014, 0.0);
	mm.emission = vec4(0.0,0.0,0.0,0.0);
	mm.shininess = float(12.8);
	if(shadingMode > 0) {
		vec3 L = normalize(Lposition.xyz - v);
		vec3 N1 = normalize(N);
		vec3 V1 = normalize(v * -1);
		vec3 H = normalize(L + V1);
		if(illumMode == 0) {
			vec4 Idiff = mm.diffuse * mls.diffuse * max(dot(N1,L), 0.0);  
			Idiff = clamp(Idiff, 0.0, 1.0); 
			vec4 Ispec = mm.ambient * mls.ambient +  Idiff + mm.specular * mls.intensity * pow(dot(N1, H), mm.shininess);
			Ispec = clamp(Ispec, 0.0, 1.0); 
			gl_FragColor = Ispec; 
		}
		else {
			//cook model
			float e = float(2.71828);
			float pi = 3.141593;
			float m1 = float(.4);
			float w1 = float(.4);
			float m2 = float(.2);
			float w2 = float(.6);
			vec3 F0 = vec3(0.755, 0.49, 0.095);
			float G = min(float(1.0), min(2 * dot(N1, H) * dot(N1, V1) / dot(V1, H), 2 * dot(N1, H) * dot(N1, L) / dot(V1, H)));
			float alpha = acos(dot(N1, H));
			float D1 = pow(e, -1 * pow(tan(alpha)/m1, 2)) / (pow(m1, 2) * pow(cos(alpha), 4));
			float D2 = pow(e, -1 * pow(tan(alpha)/m2, 2)) / (pow(m2, 2) * pow(cos(alpha), 4));
			float D = D1 * w1 + D2 * w2;
			vec3 n = (vec3(1.0, 1.0, 1.0) + sqrt(F0)) / (vec3(1.0, 1.0, 1.0) - sqrt(F0)); 
			vec3 F = (1.0 - pow(1.0 - dot(N1, V1), 5.0)) + n * pow(1.0 - (dot(N1, V1)), 5.0);
			vec3 temp = D * G * F / (pi * dot(L, N1) * dot(V1, N1));
			vec4 Ir = mm.ambient * mls.ambient + mls.intensity * vec4(F0, 0.0) * dot(N1, L) * mm.diffuse + mm.specular * vec4(temp, 0.0) * mls.intensity;
			gl_FragColor = Ir; 
		}
	}
	else {
		if(illumMode == 0) {
			gl_FragColor = IPhong;
		}
		else {
			gl_FragColor = Icook;
		}
	}
}