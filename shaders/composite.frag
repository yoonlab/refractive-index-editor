#version 330 core

uniform sampler2D depthTex;
uniform sampler2D colorTex;
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform vec3 camPos;
uniform bool refractionMode;
uniform float near;

uniform float bgN;
uniform float bgF;
uniform float invN;
uniform float invA;
uniform float invH;
uniform float hotN;
uniform float hotF;
uniform float scale;

in vec4 pos;
in vec2 texCoord;

// Ouput data
out vec3 color;

float maxStep = 100;
float stepDelta = 1;

vec3 d = vec3(0, 1, 0);
float ni = 1.0;
float slope = 0.001;

float logistic(in float L, in float invK, in float xNaught, in float x)
{
    return L / (1 + exp((xNaught - x) / invK));
}

float logisticDeriv(in float L, in float invK, in float xNaught, in float x)
{
    return L / (2 * invK * cosh((xNaught - x) / invK) + 2 * invK);
}

float n(in vec3 pos)
{
    float deStd = logistic(bgN, 10*bgF, 0, pos.y) + logistic(invN, -0.1*invA, invH, pos.y) + logistic(hotN, 0.3*hotF, 0, pos.y);
    return deStd / (abs(bgN) + abs(invN) + abs(hotN)) * scale * 0.003 + 1;
}

vec3 gradN(in vec3 pos)
{
    float deStdDy = logisticDeriv(bgN, 10*bgF, 0, pos.y) + logisticDeriv(invN, -0.1*invA, invH, pos.y) + logisticDeriv(hotN, 0.3*hotF, 0, pos.y);
    return vec3(0, deStdDy / (abs(bgN) + abs(invN) + abs(hotN)) * scale * 0.003, 0);
}

void main()
{
    if (refractionMode)
    {
        vec4 originalColor = texture2D(colorTex, texCoord);
        float depth = texture2D(depthTex, texCoord).r;
        vec4 unnormalizedVP = (inverse(ProjectionMatrix) * vec4(pos.xy, depth, 1.0));
        vec3 viewPosition = unnormalizedVP.xyz / unnormalizedVP.w;
        vec4 unnormalizedWP = inverse(ViewMatrix) * vec4(viewPosition, 1.0);
        vec3 worldPosition = unnormalizedWP.xyz / unnormalizedWP.w;
        bool isIntersection = false;

        // RK4
        vec3 v0 = normalize(worldPosition - camPos);
        vec3 x0 = camPos + v0 * near;
        vec3 dx1, dx2, dx3, dx4, dv1, dv2, dv3, dv4, dx, dv, x, v;
        x = x0;
        v = v0;
        for (float s = 0; s < maxStep; s += stepDelta)
        {
            dx1 = stepDelta * v / n(x);
            dv1 = stepDelta * gradN(x);
            dx2 = stepDelta * (v + dv1 / 2.0) / n(x + dx1 / 2.0);
            dv2 = stepDelta * gradN(x + dx1 / 2.0);
            dx3 = stepDelta * (v + dv2 / 2.0) / n(x + dx2 / 2.0);
            dv3 = stepDelta * gradN(x + dx2 / 2.0);
            dx4 = stepDelta * (v + dv3) / n(x + dx3);
            dv4 = stepDelta * gradN(x + dx3);
            dx = (dx1 + 2.0 * dx2 + 2.0 * dx3 + dx4) / 6.0;
            dv = (dv1 + 2.0 * dv2 + 2.0 * dv3 + dv4) / 6.0;
            x += dx;
            v += dv;
            /* do something here */
            vec4 projPos = (ProjectionMatrix * ViewMatrix * vec4(x, 1));
            vec3 projPosN = projPos.xyz/projPos.w;
            vec2 depthTexCoordThere = (projPosN.xy + vec2(1.0)) / 2.0;
            if (projPosN.z > texture2D(depthTex, depthTexCoordThere).r)
            {
                color = texture2D(colorTex, depthTexCoordThere).rgb;
                isIntersection = true;
            }
        }

        if (!isIntersection)
    	   color = vec3(0.8);
    }
    else
    {
        color = texture2D(colorTex, texCoord).rgb;
    }
}