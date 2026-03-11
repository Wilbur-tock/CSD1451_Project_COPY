#include "GraphicsManager.hpp"
#include "CollisionManager.hpp"
#include "AssetsManager.hpp"
#include "CameraManager.hpp"
#include "Loading.hpp"

#include "Color.hpp"
#include "GameObject.hpp"

namespace GraphicsManager {
	// WORLD_CENTER = world coordinates and origin point is center (E.g: Tile, enemy, blocks)
	// WORLD_TOP_LEFT_CORNER = world coordinates, origin point top left corner (E.g: Cannon charging bar, Enemy health bar)
	// SCREEN_TOP_LEFT_CORNER = world coordinates, origin point top left corner (E.g: UI panels)
	enum class CoordinateType { 
		WORLD_CENTER, 
		WORLD_TOP_LEFT_CORNER, 
		SCREEN_TOP_LEFT_CORNER
	};
	
	// Mesh for WORLD environment instance such as tiles, blocks objects
	AEGfxVertexList* pMeshCenter = nullptr;
	// Mesh for UI screen overlay and UI in the WORLD environment
	AEGfxVertexList* pMeshTopLeftCorner = nullptr;
	// Mesh for animated sprites
	AEGfxVertexList* pSpriteMesh = nullptr;

	size_t drawCount = 0;

	// Use either WORLD or SCREEN based on which rectMode to use center or corner rectMode, texture to render (image/sprite),
	// 3x3 matrix created using CreateTransformMatrix
	// r,g,b,a - colour of the text values of 0.0f - 1.0f 
	static void DrawTexture(CoordinateType renderMode, AEGfxTexture* pTex, AEMtx33 transform, Color const& color, AEGfxVertexList* pMesh = nullptr);

	void GenerateMesh(AEGfxVertexList*& pMesh, bool isOriginCenter) {
		float meshSize = 1.0f;

		AEGfxMeshStart();
		if (isOriginCenter) { //Generate mesh with center origin
			AEGfxTriAdd(-meshSize, -meshSize, 0xFFFFFFFF, 0.0f, 1.0f,  // Bottom-left
				meshSize, -meshSize, 0xFFFFFFFF, 1.0f, 1.0f,  // Bottom-right
				-meshSize, meshSize, 0xFFFFFFFF, 0.0f, 0.0f); // Top-left

			AEGfxTriAdd(meshSize, -meshSize, 0xFFFFFFFF, 1.0f, 1.0f,  // Bottom-right
				meshSize, meshSize, 0xFFFFFFFF, 1.0f, 0.0f,  // Top-right
				-meshSize, meshSize, 0xFFFFFFFF, 0.0f, 0.0f); // Top-left

			pMesh = AEGfxMeshEnd();
		}
		else { //Generate mesh with top left corner origin
			AEGfxTriAdd(0.0f, -meshSize, 0xFFFFFFFF, 0.0f, 1.0f,
				meshSize, -meshSize, 0xFFFFFFFF, 1.0f, 1.0f,
				0.0f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f);

			AEGfxTriAdd(meshSize, -meshSize, 0xFFFFFFFF, 1.0f, 1.0f,
				meshSize, 0.0f, 0xFFFFFFFF, 1.0f, 0.0f,
				0.0f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f);

			pMesh = AEGfxMeshEnd();
		}
	}

	static void DrawTexture(CoordinateType mode, AEGfxTexture* pTex, AEMtx33 transform, Color const& color, AEGfxVertexList* pMesh) {
		// If mesh is empty used the default mesh
		if (!pMesh) {
			pMesh = pMeshCenter;

			if (mode == CoordinateType::WORLD_TOP_LEFT_CORNER || mode == CoordinateType::SCREEN_TOP_LEFT_CORNER) {
				pMesh = pMeshTopLeftCorner;
			}
		}

		// Calculate new transform matrix if camera zoom
		if (mode != CoordinateType::SCREEN_TOP_LEFT_CORNER) {
			// AFTER (zooms toward screen/camera center):
			float zoom = CameraManager::camera.GetZoom();

			// Get camera position (the world point at the center of the screen)
			AEVec2 camPos = CameraManager::camera.GetPosition();

			// Step 1: Translate so camera center is at origin
			AEMtx33 matTransToOrigin;
			AEMtx33Trans(&matTransToOrigin, -camPos.x, -camPos.y);

			// Step 2: Apply zoom scale
			AEMtx33 matScale;
			AEMtx33Scale(&matScale, zoom, zoom);

			// Step 3: Translate back
			AEMtx33 matTransBack;
			AEMtx33Trans(&matTransBack, camPos.x, camPos.y);

			// Combine: T(back) * Scale * T(-center)
			AEMtx33 zoomMat;
			AEMtx33Concat(&zoomMat, &matScale, &matTransToOrigin);
			AEMtx33Concat(&zoomMat, &matTransBack, &zoomMat);

			// Apply zoom transform to object transform
			AEMtx33Concat(&transform, &zoomMat, &transform);
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
		AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);

		++drawCount;
	}

	// Builds a custom quad mesh where the UV coordinates span exactly one frame width.
	// This is called once during GraphicsManager::Load() and the result is stored
	// in pSpriteMesh for use by DrawSpriteFrame every frame.
	//
	// Why a custom mesh instead of reusing pMeshMiddle?
	// pMeshMiddle UV coordinates span 0.0 to 1.0 (the full texture width).
	// For a sprite sheet, we only ever want to sample 1/frameCount of the texture
	// at a time. Baking that fraction into the mesh UVs means the GPU naturally
	// clips to one frame, and AEGfxTextureSet only needs to shift the offset.
	void BuildSpriteMesh() {

		// frameUWidth is the UV width of a single frame in normalised texture space.
		// For 6 frames: 1.0 / 6 = 0.1667
		// This becomes the right-edge U coordinate of the quad.
		// The left edge is always 0.0 because AEGfxTextureSet shifts the whole
		// window by the frame offset at draw time.
		float frameUWidth = 1.0f / Loading::SPRITE_FRAME_COUNT; 

		// Begin recording vertex data for a new mesh
		AEGfxMeshStart();

		// The quad is built from two triangles sharing a diagonal edge.
		// Vertex layout in local space (center-origin, spans -1 to +1):
		//
		//  (-1, 1) -------- (1, 1)    UV: (0,0) ------- (frameUWidth, 0)
		//     |          /    |              |           /      |
		//     |        /      |              |         /        |
		//     |      /        |              |       /          |
		//  (-1,-1) -------- (1,-1)    UV: (0,1) ------- (frameUWidth, 1)
		//
		// Note: in Alpha Engine UV Y is flipped - 0.0 is the TOP of the texture,
		// 1.0 is the BOTTOM.

		// Triangle 1: bottom-left, bottom-right, top-left
		AEGfxTriAdd(
			-1.0f, -1.0f, 0xFFFFFFFF, 0.0f, 1.0f,		// Bottom-left  | UV left,  bottom
			1.0f, -1.0f, 0xFFFFFFFF, frameUWidth, 1.0f, // Bottom-right | UV right, bottom
			-1.0f, 1.0f, 0xFFFFFFFF, 0.0f, 0.0f			// Top-left     | UV left,  top
		);
		// Triangle 2: bottom-right, top-right, top-left
		AEGfxTriAdd(
			1.0f, -1.0f, 0xFFFFFFFF, frameUWidth, 1.0f,  // Bottom-right | UV right, bottom
			1.0f, 1.0f, 0xFFFFFFFF, frameUWidth, 0.0f,   // Top-right    | UV right, top
			-1.0f, 1.0f, 0xFFFFFFFF, 0.0f, 0.0f			 // Top-left     | UV left,  top
		); 

		// Finalise and store the mesh. pSpriteMesh is reused every frame by
		// DrawSpriteFrame - it is only built once here during Load().
		pSpriteMesh = AEGfxMeshEnd();

	}

	// Renders a single frame from a horizontal sprite sheet in screen space.
	// Unlike DrawScreenOverlay, this function supplies a UV offset to AEGfxTextureSet
	// so only the correct frame column of the sheet is sampled by the GPU.
	//
	// Parameters:
	//   tex     - the full sprite sheet texture (all frames in one image)
	//   frame   - which frame to show, 0-based (e.g. 0 to 5 for a 6-frame sheet)
	//   screenX - center X position in screen coordinates
	//   screenY - center Y position in screen coordinates
	//   w, h    - render size in pixels
	void DrawSpriteFrame(AEGfxTexture* tex, int frame, float screenX, float screenY, float w, float h) {
		// Do nothing if the texture or mesh has not been loaded yet, prevent crash due to nullptr
		if (!tex || !pSpriteMesh) return;

		// Calculate the UV X offset for this frame.
		// UV coordinates are normalised 0.0 - 1.0 across the full texture width.
		// Dividing by the total frame count gives the width of one frame in UV space.
		// Multiplying by the frame index shifts to the correct column.
		// Example: frame 2 of 6 -> uvOffsetX = 2 * (1/6) = 0.333
		float uvOffsetX = frame * (1.0f / Loading::SPRITE_FRAME_COUNT);

		// Build the transform matrix for this sprite.
		AEMtx33 mat = CreateTransformMatrix(screenX, screenY, 0.0f, w * 0.5f, h * 0.5f);

		// Apply the same camera offset correction used by DrawScreenOverlay.
		// This keeps the sprite locked to screen space even when the world camera moves.
		// Without this, the sprite would drift with the camera during gameplay.
		f32 camX = 0, camY = 0;
		AEGfxGetCamPosition(&camX, &camY);
		mat.m[0][2] += floorf(camX);
		mat.m[1][2] += floorf(camY);

		// Set the render mode to texture so the GPU samples the sprite sheet
		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
		// Enable alpha blending so transparent pixels in the sprite are see-through
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		AEGfxSetTransparency(1.0f);
		// Multiply color set to white (1,1,1,1) so the sprite draws with its original colors.
		// Changing these values would tint the sprite.
		AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
		// Additive color set to zero so no extra color is added on top of the sprite
		AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);

		// Bind the sprite sheet texture and supply the UV X offset.
		// This is the key difference from DrawScreenOverlay which always passes (0, 0).
		// The GPU will start sampling from uvOffsetX instead of 0, showing only
		// the current frame column. The mesh UVs already span one frame width (0 to 1/6),
		// so together they isolate exactly one frame of the sheet.
		AEGfxTextureSet(tex, uvOffsetX, 0.0f);
		// Apply the transform matrix so the quad is placed and sized correctly
		AEGfxSetTransform(mat.m);
		// Draw pSpriteMesh (the custom frame mesh) as two triangles forming a quad
		AEGfxMeshDraw(pSpriteMesh, AE_GFX_MDM_TRIANGLES);
	}

	void Load() {
		GenerateMesh(pMeshCenter);
		GenerateMesh(pMeshTopLeftCorner, false);
		BuildSpriteMesh();
		AEGfxSetTextureMode(AE_GFX_TM_PRECISE);
	}

	void Unload() {
		if (pMeshCenter) {
			AEGfxMeshFree(pMeshCenter);
			pMeshCenter = nullptr;
		}
		if (pMeshTopLeftCorner) {
			AEGfxMeshFree(pMeshTopLeftCorner);
			pMeshTopLeftCorner = nullptr;
		}
		if (pSpriteMesh) { 
			AEGfxMeshFree(pSpriteMesh); 
			pSpriteMesh = nullptr; 
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
		DrawTexture(CoordinateType::WORLD_CENTER, pTex, transform, color);
	}

	void DrawGameObject(GameObject const& gameObject) {
		AEMtx33 mat = CreateTransformMatrix(gameObject.transform.pos.x, gameObject.transform.pos.y, 0, gameObject.transform.size.x * 0.5f, gameObject.transform.size.y * 0.5f);
		DrawTexture(CoordinateType::WORLD_CENTER, gameObject.texture, mat, gameObject.textureColor, gameObject.pMesh);
	}

	void DrawWorldUI(AEGfxTexture* pTex, AEMtx33 const& transform, Color const& color) {
		DrawTexture(CoordinateType::WORLD_TOP_LEFT_CORNER, pTex, transform, color);
	}

	void DrawScreenOverlay(AEGfxTexture* pTex, AEMtx33 const& transform, Color const& color) {
		AEMtx33 newTrans = transform;
		// Adjust the x and y position of the UI object to adjust based on the camera movement
		f32 camX = 0, camY = 0;
		AEGfxGetCamPosition(&camX, &camY);
		newTrans.m[0][2] += floorf(camX);
		newTrans.m[1][2] += floorf(camY);

		DrawTexture(CoordinateType::SCREEN_TOP_LEFT_CORNER, pTex, newTrans, color);
	}

	void DrawLine(AEVec2 pos, AEVec2 size) {
		float halfW = size.x * 0.5f;
		float halfH = size.y * 0.5f;
		float lineSize = 1.0f;
		Color lineColor = { 0,1,0 };

		// Points
		AEVec2 top		= { pos.x, pos.y + (size.y * 0.5f) };
		AEVec2 btm		= { pos.x, pos.y - (size.y * 0.5f) };
		AEVec2 right	= { pos.x + halfW, pos.y };
		AEVec2 left		= { pos.x - halfW, pos.y };

		AEMtx33 mat = CreateTransformMatrix(top.x, top.y, 0, halfW, lineSize);
		DrawTexture(CoordinateType::WORLD_CENTER, AssetsManager::squareTexture, mat, lineColor);

		mat = CreateTransformMatrix(btm.x, btm.y, 0, halfW, lineSize);
		DrawTexture(CoordinateType::WORLD_CENTER, AssetsManager::squareTexture, mat, lineColor);

		mat = CreateTransformMatrix(right.x, right.y, 0, lineSize, halfH);
		DrawTexture(CoordinateType::WORLD_CENTER, AssetsManager::squareTexture, mat, lineColor);

		mat = CreateTransformMatrix(left.x, left.y, 0, lineSize, halfH);
		DrawTexture(CoordinateType::WORLD_CENTER, AssetsManager::squareTexture, mat, lineColor);
	}
}