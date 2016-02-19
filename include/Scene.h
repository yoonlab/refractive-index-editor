#pragma once

#include "Common.h"
#include "TexturedMesh.h"
#include "PointIndicator.h"

class Scene
{
public:
    Scene();
    ~Scene();
    void addWavefront(const char *fileName, glm::mat4 modelMat);
    void setPointsToBeShown(std::vector<PosColorVertex> *points);
    void prepare();
    std::vector<TexturedMesh *> texturedMeshes;
    PointIndicator *pointIndicator;
    std::map<std::string, Material> materials;
    std::map<std::string, SDL_Surface*> textures;
    const bool getIsPrepared() { return isPrepared; }

private:
    bool isPrepared;
    void addMaterial(Material*);
    void addTexturedMesh(TexturedMesh*);
    void addTexture(const char*, GLuint *);
};