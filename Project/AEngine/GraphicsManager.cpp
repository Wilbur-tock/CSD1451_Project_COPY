#include "pch.hpp"

namespace GraphicsManager {

	AEGfxVertexList* pMeshMiddle = nullptr, *pMeshCorner = nullptr;
	s8 textFont;

	void Load() {
		textFont = AEGfxCreateFont("Assets/liberation-mono.ttf", Config::FONT_SIZE);
		GenerateMesh();
	}

	void GenerateMesh() {
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

	void DrawTexture(AEGfxVertexList* pMesh, AEGfxTexture* pTex, AEMtx33 transform, float r, float g, float b, float a) {
		// Set the texture and render mode
		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		AEGfxSetTransparency(1.0f);

		// Change the colour of the sprite
		AEGfxSetColorToMultiply(r, g, b, a);
		AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);

		AEGfxTextureSet(pTex, 0, 0);

		// Set the position of the texture
		AEGfxSetTransform(transform.m);

		// Draw the mesh
		AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);
	}

	void DrawGameObject(AEGfxTexture* pTex, AEMtx33 transform, float r, float g, float b, float a) {
		DrawTexture(pMeshMiddle, pTex, transform, r, g, b, a);
	}

	void DrawUI(AEGfxTexture* pTex, AEMtx33 transform, float r, float g, float b, float a) {
		DrawTexture(pMeshCorner, pTex, transform, r, g, b, a);
	}

	void DrawUIFollowCam(AEGfxTexture* pTex, AEMtx33 transform, float r, float g, float b, float a) {
		f32 camX = 0, camY = 0;
		AEGfxGetCamPosition(&camX, &camY);
		transform.m[0][2] += floorf(camX);
		transform.m[1][2] += floorf(camY);
		
		DrawTexture(pMeshCorner, pTex, transform, r, g, b, a);
	}

	void DrawTextUI(std::string text, f32 x, f32 y, float r, float g, float b, float a, float textScale) {
		f32 currentY = y;

		// Get line height and print size with a default text
		f32 charWidth, lineHeight;
		AEGfxGetPrintSize(textFont, "A", textScale, &charWidth, &lineHeight);

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
				AEGfxGetPrintSize(textFont, pText, textScale, &w, &h);
				AEGfxPrint(textFont, pText, x, currentY - h, textScale, r, g, b, a);
				currentY -= lineHeight * 1.5f; // Move down for next line
			}
		}
	}
}