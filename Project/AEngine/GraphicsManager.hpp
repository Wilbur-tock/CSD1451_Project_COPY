#ifndef GRAPHICSMANAGER_HPP
#define GRAPHICSMANAGER_HPP

#include "AEEngine.h"
#include <iostream>

namespace GraphicsManager {

	AEGfxVertexList* pMeshMiddle = nullptr, * pMeshCorner = nullptr;

	/* Generate the mesh to draw on e.g.paper to draw on. There are TWO mesh, pMeshMiddle(RectMode Center) for game objects to draw on,
	pMeshCorner (RectMode Corner) for UI to draw on */
	void GenerateMesh();

	/* Create the transform matrix to be used to draw out the texture on the mesh, x,y - position of where the texture is going to be render at
	 rot - rotation, scaleX, scaleY, how big/small the texture is going to render */
	AEMtx33 CreateTransformMatrix(float x, float y, float rot, float scaleX, float scaleY);

	/* Use either pMeshMiddle or pMeshCorner based on which rectMode to use center or corner rectMode, texture to render (image/sprite),
	3x3 matrix created using CreateTransformMatrix
	r,g,b,a - colour of the text values of 0.0f - 1.0f */
	void DrawSprite(AEGfxVertexList* pMesh, AEGfxTexture* pTex, AEMtx33 transform, float r, float g, float b, float a);
}

#endif // !GRAPHICSMANAGER_HPP
