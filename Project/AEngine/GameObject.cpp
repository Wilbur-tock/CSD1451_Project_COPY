#include "GameObject.hpp"

#include "GraphicsManager.hpp"

GameObject::GameObject(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, TileIndex _tileId, Color _textureColor) 
	: id{ 0 }, tileId{ _tileId }, transform{ _pos, _size }, velocity{0,0}, texture{ _texture }, 
	textureColor{ _textureColor }, collider{ CollisionType::COLLIDER_BOX, _size } {
    GraphicsManager::GenerateMesh(pMesh);
}

GameObject::GameObject(const GameObject& other)
	: id{ other.id }, tileId{ other.tileId }, transform{ other.transform },
	velocity{ other.velocity }, texture{ other.texture },
	textureColor{ other.textureColor }, collider{ other.collider } {
    // Deep Copy the Mesh
    if (other.pMesh) {
        GraphicsManager::GenerateMesh(pMesh, true);
    }
    else {
        pMesh = nullptr;
    }
}

GameObject& GameObject::operator=(const GameObject& other) {
    if (this == &other) {
        return *this;
    }

    // Unload the mesh
    if (pMesh != nullptr) {
        AEGfxMeshFree(pMesh);
        pMesh = nullptr;
    }

    // Copy
    id = other.id;
    tileId = other.tileId;
    transform = other.transform;
    velocity = other.velocity;
    texture = other.texture;
    textureColor = other.textureColor;
    collider = other.collider;

    // Deep copy the mesh
    if (other.pMesh) {
        GraphicsManager::GenerateMesh(pMesh, true);
    }

    return *this;
}

GameObject::~GameObject() {
	if (pMesh != nullptr) {
		AEGfxMeshFree(pMesh);
		pMesh = nullptr;
	}
}

bool GameObject::IsInMapBounds() const {
	return transform.pos.x >= Config::WORLD_MIN_X && transform.pos.x <= Config::WORLD_MAX_X 
		&& transform.pos.y >= Config::WORLD_MIN_Y && transform.pos.y <= Config::WORLD_MAX_Y;
}

bool GameObject::IsInMapBoundsX() const {
	return transform.pos.x >= Config::WORLD_MIN_X && transform.pos.x <= Config::WORLD_MAX_X;
}

bool GameObject::IsInMapBoundsY() const {
	return transform.pos.y >= Config::WORLD_MIN_Y && transform.pos.y <= Config::WORLD_MAX_Y;
}