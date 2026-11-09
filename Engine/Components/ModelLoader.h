#pragma once
struct aiMesh;
struct aiMaterial;
class ModelLoader
{
public:
	static std::vector<MeshData> Load(const char* filePath);

private:
	static MeshData ProcessMesh(const aiMesh* mesh);
	static MaterialData ProcessMaterial(const aiMaterial* material);
};
