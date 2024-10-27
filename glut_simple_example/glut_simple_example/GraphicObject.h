#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "TreeExt.h"
#include "Transform.h"
#include "Texture.h"
#include "BoundingBox.h"
#include "Mesh.h"


class GraphicObject :  public TreeExt<GraphicObject> {
	
	Transform _transform;
	glm::u8vec3 _color = glm::u8vec3(255,255,255);
	Texture _texture;
	std::shared_ptr<Mesh> _mesh_ptr;

public:
	const auto& transform() const { return _transform; }
	auto& transform() { return _transform; }
	const auto& color() const { return _color; }
	auto& color() { return _color; }
	const auto& texture() const { return _texture; }
	auto& texture() { return _texture; }
	const auto& mesh() const { return *_mesh_ptr; }
	auto& mesh() { return *_mesh_ptr; }

	Transform worldTransform() const { return isRoot() ? _transform : parent().worldTransform() * _transform; }

	BoundingBox localBoundingBox() const;
	BoundingBox boundingBox() const { return _transform.mat() * localBoundingBox(); }

	BoundingBox worldBoundingBox() const;

	void setTextureImage(const std::shared_ptr<Image>& img_ptr) { _texture.setImage(img_ptr); }
	void setMesh(const std::shared_ptr<Mesh>& mesh_ptr) { _mesh_ptr = mesh_ptr; }

	bool hasTexture() const { return _texture.id(); }
	bool hasMesh() const { return _mesh_ptr != nullptr; }

	void draw() const;
};

