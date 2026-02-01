#include "GraphicsManager.hpp"
#include "CollisionManager.hpp"
#include "AssetsManager.hpp"

namespace GraphicsManager {

	enum CoordinateType { WORLD, SCREEN };

	AEGfxVertexList* pMeshMiddle = nullptr, *pMeshCorner = nullptr;

	// Use either WORLD or SCREEN based on which rectMode to use center or corner rectMode, texture to render (image/sprite),
	// 3x3 matrix created using CreateTransformMatrix
	// r,g,b,a - colour of the text values of 0.0f - 1.0f 
	static void DrawTexture(CoordinateType renderMode, AEGfxTexture* pTex, AEMtx33 transform, Color color);

	// Generate the mesh to draw on e.g.paper to draw on. There are TWO mesh, 
	// pMeshMiddle (RectMode Center) for game objects to draw on,
	// pMeshCorner (RectMode Corner) for UI to draw on 
	static void GenerateMesh();

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

	void Load() {
		GenerateMesh();
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

	static void DrawTexture(CoordinateType mode, AEGfxTexture* pTex, AEMtx33 transform, Color color) {
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
		AEGfxSetColorToMultiply(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
		AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);

		AEGfxTextureSet(pTex, 0, 0);

		// Set the position of the texture
		AEGfxSetTransform(transform.m);

		// Draw the mesh
		AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
	}

	void DrawGameObject(AEGfxTexture* pTex, AEMtx33 transform, Color color) {
		DrawTexture(WORLD, pTex, transform, color);
	}

	void DrawUI(AEGfxTexture* pTex, AEMtx33 transform, Color color) {
		DrawTexture(SCREEN, pTex, transform, color);
	}

	void DrawUIFollowCam(AEGfxTexture* pTex, AEMtx33 transform, Color color) {
		// Adjust the x and y position of the UI object to adjust based on the camera movement
		f32 camX = 0, camY = 0;
		AEGfxGetCamPosition(&camX, &camY);
		transform.m[0][2] += floorf(camX);
		transform.m[1][2] += floorf(camY);

		DrawUI(pTex, transform, color);
	}

	void DrawTextUI(TextObject& textObj) {
		Utils::Limit(textObj.scale, 0.0f, 1.0f);

		std::string text = textObj.textContent;

		// Get line height and print size with a default text
		f32 charWidth, lineHeight, currentY = textObj.pos.y;
		AEGfxGetPrintSize(AssetsManager::textFont, "A", textObj.scale, &charWidth, &lineHeight);

		size_t start = 0;
		size_t end = text.find('\n');

		while (end != std::string::npos || start < text.length()) {
			// Extract line
			std::string line;
			if (end != std::string::npos) {
				line = text.substr(start, end - start);
				start = end + 1;
				end = text.find('\n', start);
			}
			else {
				line = text.substr(start);
				start = text.length();
			}

			// Draw the line if not empty
			if (!line.empty()) {
				char const* pText = line.c_str();
				f32 w, h;
				AEGfxGetPrintSize(AssetsManager::textFont, pText, textObj.scale, &w, &h);
				AEGfxPrint(AssetsManager::textFont, pText, textObj.pos.x, currentY - h, textObj.scale, 
					textObj.color.GetRed(), textObj.color.GetGreen(), textObj.color.GetBlue(), textObj.color.GetAlpha());
			}

			currentY -= lineHeight * 1.5f; // Move down for next line
		}
	}

	void DrawButton(Button &btn) {
		Color btnColor = btn.color;
		if (btn.isHovered) {
			btnColor = Color{ btn.color.GetRed(), btn.color.GetGreen(), btn.color.GetBlue(), btn.color.GetAlpha() * 0.5f}; // Assign hovered color
		}
		
		// Draw button object
		DrawUIFollowCam(AssetsManager::squareTexture, CreateTransformMatrix(btn.pos.x, btn.pos.y, 0, btn.size.x, btn.size.y), btnColor);
		// Draw text if the string is not a empty string
		if (!btn.textObj.textContent.empty()) {
			DrawTextUI(btn.textObj);
		}
	}
}