#include "Scene.h"
#include <SDL_image.h>

Scene::Scene()
{
	isPrepared = false;
}

Scene::~Scene()
{
	for (auto sceneNode : sceneNodes)
	{
		delete sceneNode;
	}
	sceneNodes.clear();

	textures.clear();
	materials.clear();
}

void Scene::prepare()
{
	if (!isPrepared)
	{
		for (int i = 0; i < sceneNodes.size(); i++)
		{
			if (materials.find(sceneNodes[i]->getMaterialName()) == materials.end())
			{
				std::cerr << "Material " << sceneNodes[i]->getMaterialName() << " was not loaded" << std::endl;
			}
			else
			{
				if (strlen(materials[sceneNodes[i]->getMaterialName().c_str()].diffuseTexName) > 0)
					addTexture(materials[sceneNodes[i]->getMaterialName().c_str()].diffuseTexName, sceneNodes[i]->getDiffuseTextureIdPtr());
			}
		}

		//Calculate Bounding Sphere radius
		for (auto sceneNode : sceneNodes)
		{
			sceneNode->calcBoundingSphere();
			sceneNode->glInit();
		}
		isPrepared = true;
	}
}

void Scene::addMaterial(Material* material)
{
	materials[material->name] = *material;
}

void Scene::addSceneNode(SceneNode* sceneNode)
{
	if (!sceneNode)
	{
		std::cerr << "Unable to add null sceneNode" << std::endl;
	}
	else
	{
		sceneNodes.push_back(sceneNode);
	}
}

// Used to check file extension
bool hasEnding(std::string const &fullString, std::string const &ending)
{
	if (fullString.length() >= ending.length())
	{
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else
	{
		return false;
	}
}

void Scene::addTexture(const char* textureFileName, GLuint *textureId)
{
	std::string fileNameStr(TEXTURE_DIRECTORY);
	fileNameStr += DIRECTORY_SEPARATOR;
	fileNameStr += textureFileName;

	std::map<std::string, SDL_Surface*>::const_iterator it = textures.find(textureFileName);

	SDL_Surface* image = 0;
	if (it != textures.end())
	{
		//std::cout << textureFileName << " is already a loaded texture: " << textures[textureFileName]->w << std::endl;
		image = textures[textureFileName];
	}
	else
	{
		image = IMG_Load(fileNameStr.c_str());
		if (!image)
		{
			std::cerr << "Unable to load texture: " << textureFileName << std::endl;
			return;
		}
		textures[textureFileName] = image;

	}

	glGenTextures(1, textureId);
	glBindTexture(GL_TEXTURE_2D, *textureId);

	int mode = GL_RGB;

	// This is still not working correctly (tga images still have wrong colors)
	std::string tga(".tga");
	if (hasEnding(fileNameStr, tga))
	{
		mode = GL_BGR;
		if (image->format->BytesPerPixel == 4)
		{
			mode = GL_BGRA;
		}
	}
	//todo: change to GL_BRG on .tga images

	if (image->format->BytesPerPixel == 4)
	{
		mode = GL_RGBA;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, mode, image->w, image->h, 0, mode, GL_UNSIGNED_BYTE, image->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Scene::addWavefront(const char * fileName, glm::mat4 modelMat)
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string modelDirectory(MODEL_DIRECTORY);
	modelDirectory += DIRECTORY_SEPARATOR;
	std::string fileNameStr(modelDirectory);
	fileNameStr += fileName;
	std::string err;
	bool noError = (tinyobj::LoadObj(shapes, materials, err, fileNameStr.c_str(), modelDirectory.c_str()));
	if (!noError)
	{
		std::cerr << err << std::endl;
		return;
	}

	for (size_t i = 0; i<materials.size(); i++)
	{
		Material m;
		memcpy((void*)& m.ambient, (void*)& materials[i].ambient[0], sizeof(float) * 3);
		memcpy((void*)& m.diffuse, (void*)& materials[i].diffuse[0], sizeof(float) * 3);
		memcpy((void*)& m.emission, (void*)& materials[i].emission[0], sizeof(float) * 3);
		memcpy((void*)& m.specular, (void*)& materials[i].specular[0], sizeof(float) * 3);
		memcpy((void*)& m.transmittance, (void*)&materials[i].transmittance[0], sizeof(float) * 3);
		memcpy((void*)& m.illum, (void*)& materials[i].illum, sizeof(int));
		memcpy((void*)& m.ior, (void*)& materials[i].ior, sizeof(float));
		memcpy((void*)& m.shininess, (void*)& materials[i].shininess, sizeof(float));
		memcpy((void*)& m.dissolve, (void*)& materials[i].dissolve, sizeof(float));
		strcpy_s(m.name, materials[i].name.c_str());
		strcpy_s(m.ambientTexName, materials[i].ambient_texname.c_str());
		strcpy_s(m.diffuseTexName, materials[i].diffuse_texname.c_str());
		strcpy_s(m.normalTexName, materials[i].specular_highlight_texname.c_str());
		strcpy_s(m.specularTexName, materials[i].specular_texname.c_str());
		addMaterial(&m);
	}

	for (size_t i = 0; i < shapes.size(); i++)
	{
		std::vector<Vertex> mVertexData;

		unsigned int materialId, lastMaterialId = 0;
		if (shapes[i].mesh.material_ids.size() > 0)
		{
			materialId = lastMaterialId = shapes[i].mesh.material_ids[0];
		}

		for (int j = 0; j <shapes[i].mesh.indices.size(); j++)
		{
			if ((j % 3) == 0)
			{
				lastMaterialId = materialId;
				materialId = shapes[i].mesh.material_ids[j / 3];

				if (materialId != lastMaterialId)
				{
					//new node
					SceneNode *sceneNode = new SceneNode
						(
							&shapes[i].name,
							&materials[lastMaterialId].name,
							&mVertexData,
							GL_TRIANGLES,
							&modelMat
						);
					addSceneNode(sceneNode);
					mVertexData.clear();
				}
			}

			Vertex v;
			memcpy((void*)& (v.position), (void*)& shapes[i].mesh.positions[shapes[i].mesh.indices[j] * 3], sizeof(float) * 3);

			if ((shapes[i].mesh.indices[j] * 3) >= shapes[i].mesh.normals.size())
			{
				std::cerr << "Unable to put normal " << std::endl;
				return;
			}

			memcpy((void*)& (v.normal), (void*)& shapes[i].mesh.normals[(shapes[i].mesh.indices[j] * 3)], sizeof(float) * 3);

			if ((shapes[i].mesh.indices[j] * 2) >= shapes[i].mesh.texcoords.size())
			{
				std::cerr << "Unable to put texcoord in " << shapes[i].name << std::endl;
				return;
			}
			tinyobj::mesh_t* m = &shapes[i].mesh;
			v.textureCoordinate[0] = m->texcoords[(int)m->indices[j] * 2];
			v.textureCoordinate[1] = 1 - m->texcoords[(int)m->indices[j] * 2 + 1]; // Account for wavefront to opengl coordinate system conversion

			mVertexData.push_back(v);
			if (j == shapes[i].mesh.indices.size() - 1)
			{
				SceneNode *sceneNode = new SceneNode
					(
						&shapes[i].name,
						&materials[lastMaterialId].name,
						&mVertexData,
						GL_TRIANGLES,
						&modelMat
						);
				addSceneNode(sceneNode);
			}
		}
	}
}
