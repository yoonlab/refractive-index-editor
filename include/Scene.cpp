#include "Scene.h"
#include <SDL_image.h>

Scene::Scene()
{
	startPosition = 0;
	isPrepared = false;
}

Scene::~Scene()
{
}

void Scene::prepare()
{
	if (!isPrepared)
	{
		for (int i = 0; i < sceneNodes.size(); i++)
		{
			if (materials.find(sceneNodes[i].material) == materials.end())
			{
				std::cerr << "Material " << sceneNodes[i].material << " was not loaded" << std::endl;
			}
			else
			{
				if (strlen(materials[sceneNodes[i].material.c_str()].diffuseTexName) > 0)
					addTexture(materials[sceneNodes[i].material.c_str()].diffuseTexName, sceneNodes[i].diffuseTextureId);
			}
		}

		//Calculate Bounding Sphere radius
		for (int i = 0; i < sceneNodes.size(); i++)
		{
			float lx = 0.f, ly = 0.f, lz = 0.f;
			float r = 0.f;

			int vertexDataSize = sceneNodes[i].vertexDataSize;
			//Calculate local origin
			for (int j = 0; j < vertexDataSize; j++)
			{
				lx += sceneNodes[i].vertexData[j].vertex[0];
				ly += sceneNodes[i].vertexData[j].vertex[1];
				lz += sceneNodes[i].vertexData[j].vertex[2];
			}
			lx /= (float)vertexDataSize;
			ly /= (float)vertexDataSize;
			lz /= (float)vertexDataSize;
			sceneNodes[i].lx = lx;
			sceneNodes[i].ly = ly;
			sceneNodes[i].lz = lz;

			for (int j = 0; j < sceneNodes[i].vertexDataSize; j++)
			{
				float x = sceneNodes[i].vertexData[j].vertex[0];
				float y = sceneNodes[i].vertexData[j].vertex[1];
				float z = sceneNodes[i].vertexData[j].vertex[2];

				double nx = x - lx;
				double ny = y - ly;
				double nz = z - lz;

				float r2 = sqrt(nx*nx + ny*ny + nz*nz);

				if (r2 > r)
				{
					r = r2;
				}
				//std::cerr << "Boundingsphere for " << sceneNodes[i].name << " = " <<  r << std::endl;

			}
			if (r == 0)
			{
				//std::cerr << "Warning, bounding sphere radius = 0 for " << sceneNodes[i].name << std::endl;
				r = 0.1f;
			}
			sceneNodes[i].boundingSphere = r;
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
		sceneNodes.push_back(*sceneNode);
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

void Scene::addTexture(const char* textureFileName, GLuint& textureId)
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

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

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
					SceneNode sceneNode;
					sceneNode.name = shapes[i].name.c_str();
					sceneNode.material = materials[lastMaterialId].name.c_str();
					sceneNode.vertexDataSize = mVertexData.size();
					sceneNode.vertexData = new Vertex[sceneNode.vertexDataSize];
					memcpy((void*)sceneNode.vertexData, (void*)mVertexData.data(), sizeof(Vertex) * sceneNode.vertexDataSize);
					btTriangleMesh *triMesh = new btTriangleMesh();
					for (int k = 0; k < mVertexData.size() / 3; k += 3)
					{
						btVector3 vertex[3] =
						{
							btVector3(mVertexData[k].vertex[0], mVertexData[k].vertex[1], mVertexData[k].vertex[2]),
							btVector3(mVertexData[k + 1].vertex[0], mVertexData[k + 1].vertex[1], mVertexData[k + 1].vertex[2]),
							btVector3(mVertexData[k + 2].vertex[0], mVertexData[k + 2].vertex[1], mVertexData[k + 2].vertex[2])
						};
						triMesh->addTriangle(vertex[0], vertex[1], vertex[2]);
					}
					btBvhTriangleMeshShape *triColShape = new btBvhTriangleMeshShape(triMesh, true);
					btCollisionObject *colObj = new btCollisionObject();
					colObj->setCollisionShape(triColShape);
					sceneNode.collisionObject = colObj;
					sceneNode.startPosition = startPosition;
					startPosition += sceneNode.vertexDataSize;
					sceneNode.endPosition = sceneNode.startPosition + sceneNode.vertexDataSize;
					sceneNode.primitiveMode = GL_TRIANGLES;
					sceneNode.diffuseTextureId = 0;
					sceneNode.modelViewMatrix = modelMat;
					addSceneNode(&sceneNode);
					mVertexData.clear();
				}
			}

			Vertex v;
			memcpy((void*)& v.vertex, (void*)& shapes[i].mesh.positions[shapes[i].mesh.indices[j] * 3], sizeof(float) * 3);

			if ((shapes[i].mesh.indices[j] * 3) >= shapes[i].mesh.normals.size())
			{
				std::cerr << "Unable to put normal " << std::endl;
				return;
			}

			memcpy((void*)& v.normal, (void*)& shapes[i].mesh.normals[(shapes[i].mesh.indices[j] * 3)], sizeof(float) * 3);

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
				SceneNode sceneNode;
				sceneNode.name = shapes[i].name.c_str();
				sceneNode.material = materials[materialId].name.c_str();
				sceneNode.vertexDataSize = mVertexData.size();
				sceneNode.vertexData = new Vertex[sceneNode.vertexDataSize];
				memcpy((void*)sceneNode.vertexData, (void*)mVertexData.data(), sizeof(Vertex) * sceneNode.vertexDataSize);
				btTriangleMesh *triMesh = new btTriangleMesh();
				for (int k = 0; k < mVertexData.size() / 3; k += 3)
				{
					btVector3 vertex[3] =
					{
						btVector3(mVertexData[k].vertex[0], mVertexData[k].vertex[1], mVertexData[k].vertex[2]),
						btVector3(mVertexData[k + 1].vertex[0], mVertexData[k + 1].vertex[1], mVertexData[k + 1].vertex[2]),
						btVector3(mVertexData[k + 2].vertex[0], mVertexData[k + 2].vertex[1], mVertexData[k + 2].vertex[2])
					};
					triMesh->addTriangle(vertex[0], vertex[1], vertex[2]);
				}
				btBvhTriangleMeshShape *triColShape = new btBvhTriangleMeshShape(triMesh, true);
				btCollisionObject *colObj = new btCollisionObject();
				colObj->setCollisionShape(triColShape);
				sceneNode.collisionObject = colObj;
				sceneNode.startPosition = startPosition;
				sceneNode.endPosition = sceneNode.startPosition + sceneNode.vertexDataSize;
				startPosition += sceneNode.vertexDataSize;
				sceneNode.primitiveMode = GL_TRIANGLES;
				sceneNode.diffuseTextureId = 0;
				sceneNode.modelViewMatrix = modelMat;
				addSceneNode(&sceneNode);
			}
		}
	}
}
