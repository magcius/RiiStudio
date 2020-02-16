#pragma once

#include <LibCore/common.h>

#include <lib3d/interface/i3dmodel.hpp>
#include <LibCore/api/Collection.hpp>

#include <memory>

#include <glm/mat4x4.hpp>

struct SceneState;

class Renderer
{
public:
	Renderer();
	~Renderer();
	void render(u32 width, u32 height);
	void createShaderProgram() { bShadersLoaded = true; }
	void destroyShaders() { bShadersLoaded = false; }

	void prepare(const px::CollectionHost& model, const px::CollectionHost& texture);

private:
	u32 mShaderProgram = 0;
	bool bShadersLoaded = false;


	glm::vec3 max{ 0, 0, 0 };
	std::vector<glm::vec3> pos;
	std::vector<u32> idx;
	std::vector<glm::vec3> colors;

	std::unique_ptr<SceneState> mState;

	glm::vec3 eye{ 0.0f, 0.0f, 0.0f };
};