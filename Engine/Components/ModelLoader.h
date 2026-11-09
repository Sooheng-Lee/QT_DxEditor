#pragma once
struct aiMesh;
class ModelLoader
{
public:
	static std::vector<MeshData> Load(const char* filePath);

private:
	static MeshData ProcessMesh(const aiMesh* mesh);
};
