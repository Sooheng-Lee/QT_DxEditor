#include "../pch.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ModelLoader.h"
#include <filesystem>

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
		const aiMesh* mesh = scene->mMeshes[idx];
		MeshData meshData = ProcessMesh(mesh);
		if (mesh->mMaterialIndex < scene->mNumMaterials)
		{
			const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			meshData.material = ProcessMaterial(material);
			if (!meshData.material.diffuseTexPath.empty())
			{
				if (scene->GetEmbeddedTexture(meshData.material.diffuseTexPath.c_str()))
					throw std::runtime_error("Embedded diffuse textures are not supported. Export textures as separate image files.");
				const auto path = std::filesystem::u8path(filePath).parent_path() /
					std::filesystem::u8path(meshData.material.diffuseTexPath);
				meshData.material.diffuseTexPath = path.lexically_normal().u8string();
			}
		}
		meshes.push_back(std::move(meshData));
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

MaterialData ModelLoader::ProcessMaterial(const aiMaterial* material)
{
	MaterialData data;
	aiColor4D color;

	// Diffuse Color
	if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
	{
		data.diffuseColor = { color.r, color.g, color.b, color.a };
	}

	// Specular Color
	if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
	{
		data.specularColor = { color.r, color.g, color.b, color.a };
	}

	float shininess = 0.0f;
	if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
	{
		data.shininess = shininess;
	}

	if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		aiString path;
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
		{
			data.diffuseTexPath = path.C_Str();
		}
	}

	return data;
}
