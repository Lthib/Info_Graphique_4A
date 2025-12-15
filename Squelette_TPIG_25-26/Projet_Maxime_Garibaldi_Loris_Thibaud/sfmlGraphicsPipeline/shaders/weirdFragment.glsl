#version 400

//Authors : Maxime Garibaldi/Loris Thibaud
//Structure definition for Material, DirectionalLight, PointLight and SpotLight
//Parameters are exactly the same as the corresponding C++ classes
//Refer to the C++ documentation for more information

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
    vec3 position;
    vec3 spotDirection;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float innerCutOff;
    float outerCutOff;
};

uniform Material material;

#define MAX_NR_DIRECTIONAL_LIGHTS 10
uniform int numberOfDirectionalLight = 0;
uniform DirectionalLight directionalLight[MAX_NR_DIRECTIONAL_LIGHTS];

#define MAX_NR_POINT_LIGHTS 10
uniform int numberOfPointLight = 0;
uniform PointLight pointLight[MAX_NR_POINT_LIGHTS];

#define MAX_NR_SPOT_LIGHTS 10
uniform int numberOfSpotLight = 0;
uniform SpotLight spotLight[MAX_NR_SPOT_LIGHTS];

// Surfel: a SURFace ELement. All coordinates are in world space
in vec3 surfel_position;
in vec4 surfel_color;
in vec3 surfel_normal;

// Camera position in world space
in vec3 cameraPosition;

// Resulting color of the fragment shader
out vec4 outColor;

vec3 contourComplem(vec3 color, vec3 N, vec3 V)
{
    float ndotv = max(dot(N, V), 0.0);

    if(ndotv < 0.5)
    {
        return vec3(color.b, 1.0 - color.g, color.r);//Au lieu de rendre 0, on rend une couleur complémentaire
    }
    else
    {
        return color;
    }
}


//Phong illumination model for a directional light
vec3 computeDirectionalLight(DirectionalLight light, vec3 surfel_to_camera)
{
    vec3 surfel_to_light = -light.direction;

    // Diffuse shading
    float diffuse_factor = max(dot(surfel_normal, surfel_to_light), 0.0);

    // Specular
    vec3 reflect_direction = reflect(-surfel_to_light, surfel_normal);
    float specular_dot = clamp(dot(surfel_to_camera, reflect_direction), 0, 1);
    float specular_factor = pow(specular_dot, material.shininess);

    // Combine results
    vec3 ambient  =                   light.ambient  * material.ambient ;
    vec3 diffuse  = diffuse_factor  * light.diffuse  * material.diffuse ;
    vec3 specular = specular_factor * light.specular * material.specular;

    return (ambient + diffuse + specular);
}

//Phong illumination model for a point light
vec3 computePointLight(PointLight light, vec3 surfel_to_camera)
{
    // Diffuse shading
    vec3 surfel_to_light = light.position - surfel_position;
    float distance = length( surfel_to_light );
    surfel_to_light *= float(1) / distance;
    float diffuse_factor = max(dot(surfel_normal, surfel_to_light), 0.0);
    
    // Specular
    vec3 reflect_direction = reflect(-surfel_to_light, surfel_normal);
    float specular_dot = clamp(dot(surfel_to_camera, reflect_direction), 0, 1);
    float specular_factor = pow(specular_dot, material.shininess);

    // Attenuation: TODO
    float attenuation = 1.0;

    // Combine results    
    vec3 ambient  = attenuation *                   light.ambient  * material.ambient ;
    vec3 diffuse  = attenuation * diffuse_factor  * light.diffuse  * material.diffuse ;
    vec3 specular = attenuation * specular_factor * light.specular * material.specular;

    return (ambient + diffuse + specular);
}

//Phong illumination model for a spot light
vec3 computeSpotLight(SpotLight light, vec3 surfel_to_camera)
{
    // Diffuse
    vec3 surfel_to_light = light.position - surfel_position;
    float distance = length( surfel_to_light );
    surfel_to_light *= float(1) / distance;
    float diffuse_factor = max(dot(surfel_normal, surfel_to_light), 0.0);
    
    // Specular
    vec3 reflect_direction = reflect(-surfel_to_light, surfel_normal);
    float specular_dot = clamp(dot(surfel_to_camera, reflect_direction), 0, 1);
    float specular_factor = pow(specular_dot, material.shininess);

    // Spotlight (soft edges): TODO
    float intensity = 1.0;

    // Attenuation
    float attenuation = 1.0;

    // Combine results    
    vec3 ambient  =             attenuation *                   light.ambient  * material.ambient ;
    vec3 diffuse  = intensity * attenuation * diffuse_factor  * light.diffuse  * material.diffuse ;
    vec3 specular = intensity * attenuation * specular_factor * light.specular * material.specular;
    
    return (ambient + diffuse + specular);
}

vec3 cel(vec3 color) { 
    return floor(color * 4.0) / 4.0; 
}


vec3 papier(vec3 color, vec3 position)
{
    float grain = 10;
    float intensity = 0.05;
    // Noise basé sur la position
    float n = fract(sin(dot(position.xy * grain, vec2(12.9898, 78.233))) * 43758.5453);   

    // Amplitude contrôlée
    float paper = mix(1.0 - intensity, 1.0 + intensity, n);

    return color * paper;
}

void main()
{
    //Surface to camera vector
    vec3 surfel_to_camera = normalize( cameraPosition - surfel_position );

    int clampedNumberOfDirectionalLight = max(0, min(numberOfDirectionalLight, MAX_NR_DIRECTIONAL_LIGHTS));
    int clampedNumberOfPointLight = max(0, min(numberOfPointLight, MAX_NR_POINT_LIGHTS));
    int clampedNumberOfSpotLight = max(0, min(numberOfSpotLight, MAX_NR_SPOT_LIGHTS));

    vec3 tmpColor = vec3(0.0, 0.0, 0.0);

    for(int i=0; i<clampedNumberOfDirectionalLight; ++i)
        tmpColor += computeDirectionalLight(directionalLight[i], surfel_to_camera);

    for(int i=0; i<clampedNumberOfPointLight; ++i)
        tmpColor += computePointLight(pointLight[i], surfel_to_camera);

    for(int i=0; i<clampedNumberOfSpotLight; ++i)
        tmpColor += computeSpotLight(spotLight[i], surfel_to_camera);

    tmpColor = cel(tmpColor);
    tmpColor = papier(tmpColor,surfel_position);
    tmpColor = clamp(tmpColor, 0.05, 0.95);//eviter les noirs/blancs
    tmpColor = contourComplem(tmpColor, normalize(surfel_normal), surfel_to_camera);
    
    outColor = vec4(tmpColor,1.0);
}