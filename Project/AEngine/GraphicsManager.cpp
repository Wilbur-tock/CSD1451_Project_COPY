#include "GraphicsManager.hpp"
#include "CollisionManager.hpp"
#include "AssetsManager.hpp"

namespace GraphicsManager {
	enum CoordinateType { WORLD, SCREEN };

	AEGfxVertexList* pMeshMiddle = nullptr, *pMeshCorner = nullptr;

	// Generate the mesh to draw on e.g.paper to draw on. There are TWO mesh, 
	// pMeshMiddle (RectMode Center) for game objects to draw on,
	// pMeshCorner (RectMode Corner) for UI to draw on 
	static void GenerateMesh();

	// Use either WORLD or SCREEN based on which rectMode to use center or corner rectMode, texture to render (image/sprite),
	// 3x3 matrix created using CreateTransformMatrix
	// r,g,b,a - colour of the text values of 0.0f - 1.0f 
	static void DrawTexture(CoordinateType renderMode, AEGfxTexture* pTex, AEMtx33 transform, Color const& color);

	static void GenerateMesh() {
		AEGfxMeshStart();

		float meshSize = 1.0f;

		//Generate mesh with center rectmode
		AEGfxTriAdd(-meshSize, -meshSize, 0xFFFFFFFF, 0.0f, 1.0f,  // Bottom-left
			meshSize, -meshSize, 0xFFFFFFFF, 1.0f, 1.0f,  // Bottom-right
			-meshSize, meshSize, 0xFFFFFFFF, 0.0f, 0.0f); // Top-left

		AEGfxTriAdd(meshSize, -meshSize, 0xFFFFFFFF, 1.0f, 1.0f,  // Bottom-right
			meshSize, meshSize, 0xFFFFFFFF, 1.0f, 0.0f,  // Top-right
			-meshSize, meshSize, 0xFFFFFFFF, 0.0f, 0.0f); // Top-left

		pMeshMiddle = AEGfxMeshEnd();

		//Generate mesh with corner rectmode
		AEGfxMeshStart();

		AEGfxTriAdd(0.0f, -meshSize, 0xFFFFFFFF, 0.0f, 1.0f,
			meshSize, -meshSize, 0xFFFFFFFF, 1.0f, 1.0f,
			0.0f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f);

		AEGfxTriAdd(meshSize, -meshSize, 0xFFFFFFFF, 1.0f, 1.0f,
			meshSize, 0.0f, 0xFFFFFFFF, 1.0f, 0.0f,
			0.0f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f);

		pMeshCorner = AEGfxMeshEnd();
	}

	static void DrawTexture(CoordinateType mode, AEGfxTexture* pTex, AEMtx33 transform, Color const& color) {
		// Set the texture and render mode
		AEGfxVertexList* mesh = pMeshMiddle;
		if (mode == WORLD) {
			mesh = pMeshMiddle;
		}
		else if (mode == SCREEN) {
			mesh = pMeshCorner;
		}

		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		AEGfxSetTransparency(1.0f);

		// Change the colour of the sprite
		AEGfxSetColorToMultiply(color.Red(), color.Green(), color.Blue(), color.Alpha());
		AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);

		AEGfxTextureSet(pTex, 0, 0);

		// Set the position of the texture
		AEGfxSetTransform(transform.m);

		// Draw the mesh
		AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
	}

	void Load() {
		GenerateMesh();
		AEGfxSetTextureMode(AE_GFX_TM_PRECISE);
	}

	void Unload() {
		if (pMeshMiddle) {
			AEGfxMeshFree(pMeshMiddle);
			pMeshMiddle = nullptr;
		}
		if (pMeshCorner) {
			AEGfxMeshFree(pMeshCorner);
			pMeshCorner = nullptr;
		}
	}

	AEMtx33 CreateTransformMatrix(float x, float y, float rot, float scaleX, float scaleY) {
		AEMtx33 matScale, matRot, matTrans, matResult;

		// Create individual components
		AEMtx33Scale(&matScale, scaleX, scaleY);
		AEMtx33Rot(&matRot, rot);
		AEMtx33Trans(&matTrans, x, y);

		// Concatenate: Translation * Rotation * Scale
		AEMtx33Concat(&matResult, &matRot, &matScale);
		AEMtx33Concat(&matResult, &matTrans, &matResult);

		return matResult;
	}

	void DrawGameObject(AEGfxTexture* pTex, AEMtx33 const& transform, Color const& color) {
		DrawTexture(WORLD, pTex, transform, color);
	}

	void DrawGameObject(GameObject const& gameObject) {
		AEMtx33 mat = CreateTransformMatrix(gameObject.transform.pos.x, gameObject.transform.pos.y, 0, gameObject.transform.size.x *0.5f, gameObject.transform.size.y * 0.5f);
		DrawTexture(WORLD, gameObject.texture, mat, gameObject.textureColor);
	}

	void DrawUI(AEGfxTexture* pTex, AEMtx33 const& transform, Color const& color) {
		DrawTexture(SCREEN, pTex, transform, color);
	}

	void DrawUIFollowCam(AEGfxTexture* pTex, AEMtx33 const& transform, Color const& color) {
		AEMtx33 newTrans = transform;
		// Adjust the x and y position of the UI object to adjust based on the camera movement
		f32 camX = 0, camY = 0;
		AEGfxGetCamPosition(&camX, &camY);
		newTrans.m[0][2] += floorf(camX);
		newTrans.m[1][2] += floorf(camY);

		DrawUI(pTex, newTrans, color);
	}
}