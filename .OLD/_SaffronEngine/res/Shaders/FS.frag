#version 410

out vec4 FragColor;

in vec3 ourColor;
in vec2 ourTexCoord;
in vec3 ourNormal;
in vec3 ourFragPos;

uniform sampler2D ourTexture;
uniform vec3 extraValues;

uniform vec3  viewPos;
uniform vec3  lightPos;
uniform vec3  lightColor;
uniform float ambientIntensity;
uniform float diffuseIntensity;
uniform float specularIntensity;
uniform float specularPower;
uniform float attConst;
uniform float attLin;
uniform float attQuad;

void main()
{
    //    vec3 norm = normalize(ourNormal);
    //
    //    // Fragment to light vector data
    //    vec3 fragToLight = lightPos - ourFragPos;
    //    float distToLight = length(fragToLight);
    //    vec3 dirToLight = fragToLight / distToLight;
    //
    //    // Attenuation
    //    float attenuation = 1.0f / (attConst + attLin * distToLight + attQuad * pow(distToLight, 2));
    //    // Diffuse
    //    vec3 diffuse = diffuseColor * diffuseIntensity * attenuation * max(0.0f, dot(dirToLight, norm));
    //    // Reflected light vector
    //    vec3 reflected = reflect(-fragToLight, norm);
    //    //    vec3 w = norm * dot(-fragToLight, norm );
    //    //    vec3 reflected = w * 2.0f - fragToLight;
    //
    //    // Calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    //    vec3 specular = attenuation * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-reflected), normalize(viewPos))), specularPower);
    //    // final color
    //    FragColor = clamp(vec4(diffuse + ambient, 1.0f) * texture(ourTexture, ourTexCoord) + vec4(specular, 1.0f), 0.0f, 1.0f);

    // Fragment to light vector data
    vec3 norm = normalize(ourNormal);
    vec3 fragToLight = lightPos - ourFragPos;
    float distToLight = length(fragToLight);
    vec3 dirToLight = normalize(fragToLight);

    // Attenuation
    float attenuation = 1.0f / (attConst + attLin * distToLight + attQuad * pow(distToLight, 2));

    // Ambient
    vec3 ambient = lightColor * ambientIntensity;

    // Diffuse lightning
    float diffuseValue = max(dot(norm, dirToLight), 0.0);
    vec3 diffuse = lightColor * diffuseValue * diffuseIntensity * attenuation;

    // Specular lightning
    vec3 viewDir = normalize(viewPos - ourFragPos);
    vec3 reflectDir = reflect(-dirToLight, norm);

    float specularValue = pow(max(dot(viewDir, reflectDir), 0.0), specularPower);
    vec3 specular = lightColor * specularValue * specularIntensity * attenuation;

    FragColor = texture(ourTexture, ourTexCoord) * vec4(ambient + diffuse + specular, 1.0f);
}