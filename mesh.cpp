#include "Mesh.h"

Mesh::MeshEntry::MeshEntry()
{
	VB = INVALID_OGL_VALUE;
	IB = INVALID_OGL_VALUE; 
	NumIndices = 0; 
	MaterialIndex = INVALID_MATERIAL;
}

Mesh::MeshEntry::~MeshEntry()
{
    if (VB != INVALID_OGL_VALUE)
    {
        glDeleteBuffers(1, &VB);
    }

    if (IB != INVALID_OGL_VALUE)
    {
        glDeleteBuffers(1, &IB);
    }
}

void Mesh::MeshEntry::Init(const std::vector<Vertex>& Vertices, const std::vector<unsigned int>& Indices)
{
	NumIndices = Indices.size();

	glGenBuffers(1, &VB);
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &IB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * NumIndices, &Indices[0], GL_STATIC_DRAW);
}

Mesh::Mesh(){
}

Mesh::~Mesh()
{
	Clear();
}

void Mesh::Clear()
{
	for(unsigned int i = 0; i < m_Textures.size(); i++)
	{
		SAFE_DELETE(m_Textures[i]);
	}
}

bool Mesh::LoadMesh(const std::string& Filename)
{
	//Release previously loaded mesh
	Clear();

	bool Ret = false; 
	Assimp::Importer Importer;

	const aiScene* pScene = Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if(pScene)
	{
		Ret = InitFromScene(pScene, Filename);
	}
	else
	{
		 printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
	}
	return Ret;
}

bool Mesh::InitFromScene(const aiScene* pScene, const std::string& Filename)
{
	m_Entries.resize(pScene->mNumMeshes);
	m_Textures.resize(pScene->mNumMaterials);

	//init meshes in scene
	for(unsigned int i = 0; i < m_Entries.size() ; i++)
	{
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh);
	}

	return InitMaterials(pScene, Filename);
}

void Mesh::InitMesh(unsigned int Index, const aiMesh* paiMesh)
{
	m_Entries[Index].MaterialIndex = paiMesh->mMaterialIndex;

	std::vector<Vertex> Vertices; 
	std::vector<unsigned int> Indices; 

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	for(unsigned int i = 0; i < paiMesh->mNumVertices;i++)
	{
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		Vertex v(glm::vec3(pPos->x, pPos->y, pPos->z),
				 glm::vec2(pTexCoord->x, pTexCoord->y),
				 glm::vec3(pNormal->x, pNormal->y, pNormal->z));
		Vertices.push_back(v);
	}

	for(unsigned int i = 0; i < paiMesh->mNumFaces ; i++)
	{
		const aiFace& Face = paiMesh->mFaces[i]; 
		assert(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}
	
	m_Entries[Index].Init(Vertices, Indices);
	
}

bool Mesh::InitMaterials(const aiScene* pScene, const std::string& filename)
{
	std::string::size_type SlashIndex = filename.find_last_of("/");
	std::string Dir;

	if(SlashIndex == std::string::npos){
		Dir = "."; 
	}
	else if(SlashIndex == 0){
		Dir = "/";
	}
	else{
		Dir = filename.substr(0, SlashIndex);
	}
	bool ret = true; 

	//initialize materials
	for(unsigned int i = 0; i < pScene->mNumMaterials; i++)
	{
		const aiMaterial* pMaterial = pScene->mMaterials[i]; 
		
		m_Textures[i] = NULL;

		if(pMaterial->GetTextureCount(aiTextureType_DIFFUSE)>0)
		{
			aiString path; 

			if(pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS){
				std::string fullpath = Dir + "/" +path.data; 
				m_Textures[i] = new Texture(GL_TEXTURE_2D, fullpath.c_str()); 
				
				if(!m_Textures[i]->load()){
					printf("error loading texture '%s' \n", fullpath.c_str());
					delete m_Textures[i];
					m_Textures[i] = NULL; 
					ret = false; 
				}
				else{
					printf("loaded texture '%s' \n", fullpath.c_str());
				}
			}
		}

		// Load a white texture in case the model does not include its own texture
        if (!m_Textures[i]) {
            m_Textures[i] = new Texture(GL_TEXTURE_2D, "textures/blue.png");

            ret = m_Textures[i]->load();
        }
    }

	return ret;
}

void Mesh::Render(int position, int tex, int normal, int sample)
{
	
    glEnableVertexAttribArray(position);
    glEnableVertexAttribArray(tex);
    glEnableVertexAttribArray(normal);

    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].VB);
		glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
		glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].IB);

        const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

        if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex]) {
			m_Textures[MaterialIndex]->activate(sample, GL_TEXTURE0, 0);
        }

        glDrawElements(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, 0);
    }

    glDisableVertexAttribArray(position);
    glDisableVertexAttribArray(tex);
    glDisableVertexAttribArray(normal);
}