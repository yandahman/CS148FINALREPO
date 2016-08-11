// CS148 Summer 2016 Homework 3 - Shaders
//Code was developed in reference to Phong Model
//described in http://www.cs.brandeis.edu/~cs155/Lecture_16.pdf
//worked with Sebastian Astiz Le Bras

#version 330 core
out vec4 color;

in vec3 FragPos;  
in vec3 Position;
in vec3 Normal;  
in vec2 TextCoor;
  
uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float maxHeight;
uniform float minHeight;
uniform float MaxPosHeight;

uniform sampler2D grasstexture;
uniform sampler2D sandtexture;
uniform sampler2D snowtexture;
uniform sampler2D rocktexture;

void main()
{
    //from highest to lowest 

    float specFactor = 0.0f; //how specular an object is
    float specPwr = 16; //specular Power to put reflect dot product to
    vec3 currColor = vec3(0.0f,0.0f,0.0f);

    if (Position[1] > (1.0f/5.0f) * MaxPosHeight){
        currColor =  vec3(texture(snowtexture, TextCoor));
        specFactor = 0.7f;
        specPwr = 64;

    }else if (Position[1] > (0.80f/5.0f) * MaxPosHeight){
        specPwr = 0;
        float Scaling1 = Position[1] - ((0.80f/5.0f) * MaxPosHeight);
        float Scaling2 = Position[1] - ((0.80f/5.0f) * MaxPosHeight);
        Scaling1 = Scaling1 / ((0.2f/5.0f) * MaxPosHeight);
        Scaling2 = Scaling2 / ((0.2f/5.0f) * MaxPosHeight);
        Scaling2 = 1.0f - Scaling2;

        currColor += vec3(texture(snowtexture, TextCoor)) * Scaling1;
        currColor += vec3(texture(rocktexture, TextCoor)) * Scaling2;
        specFactor += 0.7f * Scaling1;
        specFactor += 0.02f * Scaling2;
        specPwr += 64 * Scaling1;
        specPwr += 16 * Scaling2;

    }else if (Position[1] > (0.0f/5.0f) * MaxPosHeight){
        currColor =  vec3(texture(rocktexture, TextCoor));
        specFactor = 0.02f;
    }else if (Position[1] > (-0.6f/5.0f) * MaxPosHeight){

        float Scaling1 = Position[1] - ((-0.60f/5.0f) * MaxPosHeight);
        float Scaling2 = Position[1] - ((-0.60f/5.0f) * MaxPosHeight);
        Scaling1 = Scaling1 / ((0.6f/5.0f) * MaxPosHeight);
        Scaling2 = Scaling2 / ((0.6f/5.0f) * MaxPosHeight);
        Scaling2 = 1.0f - Scaling2;

        currColor += vec3(texture(rocktexture, TextCoor)) * Scaling1;
        currColor += vec3(texture(grasstexture, TextCoor)) * Scaling2;
        specFactor += 0.02f * Scaling1;
        specFactor += 0.15f * Scaling2;


    }else if (Position[1] > (-1.0f) * MaxPosHeight){
        currColor = vec3(texture(grasstexture, TextCoor)); 
        specFactor = 0.15f;

    }else if (Position[1] > (-0.2f/5.0f) * MaxPosHeight){
        currColor = vec3(texture(sandtexture, TextCoor));
        specFactor = 0.05f;

    }else if (Position[1] > (-3.0f/10.0f) * MaxPosHeight){
        currColor = vec3(0.5f,0.5f,1.0f);

    }else if (Position[1] > (-4.0f/5.0f) * MaxPosHeight){
        currColor = vec3(0.3f,0.2f,0.8f);

    }else{
        currColor = vec3(0.05f,0.05f,0.5f);;
    }

    

    // Ambient Light 
    float ambFactor = 0.2f; //factor to dim light (simulate as ambient)
    vec3 ambLight = ambFactor*lightColor; //dims the light
    vec3 ambRes = ambLight*currColor; //determine the object reflection color/intensity

    // Diffuse Light 
    float diffFactor = 0.5f;//factor to dim normal diffuse reflections
    vec3 lightVec = normalize(lightPos - FragPos);//determine vector pointing from light to
                                       		  //Fragment; normalize so that dot product=cos
    vec3 nrm = normalize(Normal); //normal should be normalized
    float diffComp = max(dot(lightVec,nrm),0.0); //compute component pointing 
						 //in direction of Normal
    vec3 diffLight = diffFactor*diffComp*lightColor; //compute the intensity/color of incoming light
    vec3 diffRes = diffLight*currColor; //determine the color/intensity of diffuse Reflection off object

    // Specular Light 
    

    
    vec3 viewVec = normalize(viewPos - FragPos); //same as lightVec but for viewing eye
    vec3 reflecVec = reflect(-lightVec,nrm); //create a vector pointing in the eye direction by 
					     //reflecting across the normal
    float reflecPwr = pow(max(dot(reflecVec,viewVec),0.0),specPwr); //Calculate the specular 
								    //reflected power according 
								    //to Phong Model
    vec3 specLight = specFactor*reflecPwr*lightColor; //Calculate resulting light intensity/color vector
    vec3 specRes = specLight*currColor; //calculate the color/intensity of reflecting from the object
    color = vec4(ambRes+diffRes+specRes, 1.0f); //pass the color/intensity of ambient/diffuse/specular contributions
    

    //color = vec4(vec3((Position[1]-maxHeight)/(maxHeight-minHeight)+1,1.f,1-Position[1]/(maxHeight)), 1.0f);
} 
