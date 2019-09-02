#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

uniform vec3 vHSV;

uniform vec3 vHSV1;

vec3 vHSV3= vec3(1.0,1.0,1.0);

uniform int flag;

uniform int mean;

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{    

	//if(mean[0] > vHSV.z)
	//{
	
	//}
	if (flag == 1)
	{
   vec4 textureColor = texture(texture_diffuse1, TexCoords);
   vec3 fragRGB = textureColor.rgb;
   vec3 fragHSV = rgb2hsv(fragRGB).xyz;
   fragHSV.x += vHSV.x / 360.0;
   fragHSV.yz *= vHSV.yz;
   fragHSV.xyz = mod(fragHSV.xyz, 1.0);
   fragRGB = hsv2rgb(fragHSV);
   //FragColor = texture(texture_diffuse1, TexCoords) ;
   FragColor = vec4(fragRGB, textureColor.w);
   }
   else if(flag == 2)
   {
   vec4 textureColor = texture(texture_diffuse1, TexCoords);
   vec3 fragRGB = textureColor.rgb;
   vec3 fragHSV = rgb2hsv(fragRGB).xyz;
   fragHSV.x += vHSV1.x / 360.0;
   fragHSV.yz *= vHSV1.yz;
   fragHSV.xyz = mod(fragHSV.xyz, 1.0);
   fragRGB = hsv2rgb(fragHSV);
   //FragColor = texture(texture_diffuse1, TexCoords) ;
   FragColor = vec4(fragRGB, textureColor.w);
   }
   else{
		 vec4 textureColor = texture(texture_diffuse1, TexCoords);
		 vec3 fragRGB = textureColor.rgb;
         vec3 fragHSV = rgb2hsv(fragRGB).xyz;
         fragHSV.x += vHSV3.x / 360.0;
         fragHSV.yz *= vHSV3.yz;
         fragHSV.xyz = mod(fragHSV.xyz, 1.0);
         fragRGB = hsv2rgb(fragHSV);
        //FragColor = texture(texture_diffuse1, TexCoords) ;
         FragColor = vec4(fragRGB, textureColor.w);
   }
}