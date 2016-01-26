#pragma once

#include "Common.h"
#include "SceneNode.h"

class Scene
{
public:
	Scene();
	~Scene();
	void addWavefront(const char *fileName, glm::mat4 modelMat);
	void prepare();
	std::vector<SceneNode> sceneNodes;
	std::map<std::string, Material> materials;
	std::map<std::string, SDL_Surface*> textures;
private:
	GLuint startPosition;
	bool isPrepared;
	void addMaterial(Material*);
	void addSceneNode(SceneNode*);
	void addTexture(const char*, GLuint&);
};