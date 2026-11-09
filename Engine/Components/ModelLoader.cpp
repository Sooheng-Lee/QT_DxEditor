#include "../pch.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ModelLoader.h"

std::vector<MeshData> ModelLoader::Load(const char* filePath)
{
	if (!filePath || !*filePath)
		throw std::runtime_error("The asset path is empty.");

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		filePath,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals |
		aiProcess_ConvertToLeftHanded |
		aiProcess_PreTransformVertices
	);

	if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
	{
		throw std::runtime_error(importer.GetErrorString());
	}

	std::vector<MeshData> meshes;
	meshes.reserve(scene->mNumMeshes);

	for (UINT idx = 0; idx < scene->mNumMeshes; ++idx)
	{
		MeshData data = ProcessMesh(scene->mMeshes[idx]);
		meshes.push_back(data);
	}

	return meshes;
}

MeshData ModelLoader::ProcessMesh(const aiMesh* mesh)
{
	MeshData data;
	data.materialIdx = mesh->mMaterialIndex;
	data.vertices.reserve(mesh->mNumVertices);
	data.indices.reserve(mesh->mNumFaces * 3);

	// Vertex
	for (UINT idx = 0; idx < mesh->mNumVertices; ++idx)
	{
		VertexDefault vertex = {};
		const aiVector3D& pos = mesh->mVertices[idx];
		vertex.Position = XMFLOAT3(pos.x, pos.y, pos.z);

		if (mesh->HasNormals())
		{
			const aiVector3D& normal = mesh->mNormals[idx];
			vertex.Normal = XMFLOAT3(normal.x, normal.y, normal.z);
		}

		if (mesh->HasTextureCoords(0))
		{
			const aiVector3D& uv = mesh->mTextureCoords[0][idx];
			vertex.UV = XMFLOAT2(uv.x, uv.y);
		}

		if (mesh->HasVertexColors(0))
		{
			const aiColor4D& color = mesh->mColors[0][idx];
			vertex.Color = XMFLOAT4(color.r, color.g, color.b, color.a);
		}
		else
		{
			vertex.Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		data.vertices.push_back(vertex);
	}

	for (UINT idx = 0; idx < mesh->mNumFaces; ++idx)
	{
		const aiFace& face = mesh->mFaces[idx];
		if (face.mNumIndices != 3) continue;

		for (unsigned subIdx = 0; subIdx < face.mNumIndices; ++subIdx)
		{
			data.indices.push_back(face.mIndices[subIdx]);
		}
	}

	return data;
}
