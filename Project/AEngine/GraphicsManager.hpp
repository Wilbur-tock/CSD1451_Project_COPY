#ifndef GRAPHICSMANAGER_HPP
#define GRAPHICSMANAGER_HPP

#include "pch.hpp"

namespace GraphicsManager {

	extern AEGfxVertexList *pMeshMiddle, *pMeshCorner;

	void Load();

	/* Generate the mesh to draw on e.g.paper to draw on. There are TWO mesh, pMeshMiddle(RectMode Center) for game objects to draw on,
	pMeshCorner (RectMode Corner) for UI to draw on */
	void GenerateMesh();

	/* Create the transform matrix to be used to draw out the texture on the mesh,
		x,y - position of where the texture is going to be render at
		rot - rotation, 
		scaleX, scaleY, scale of the texture no ranges */
	AEMtx33 CreateTransformMatrix(float x, float y, float rot = 0.f, float scaleX = 100.f, float scaleY = 100.f);

	/* Use either pMeshMiddle or pMeshCorner based on which rectMode to use center or corner rectMode, texture to render (image/sprite),
	3x3 matrix created using CreateTransformMatrix
	r,g,b,a - colour of the text values of 0.0f - 1.0f */
	void DrawTexture(AEGfxVertexList* pMesh, AEGfxTexture* pTex, AEMtx33 transform, Color color);

	/* Draw game object on the map using pMeshMiddle rect mode middle */
	void DrawGameObject(AEGfxTexture* pTex, AEMtx33 transform, Color color);

	/* Draw UI texture using pMeshCorner rect mode corner */
	void DrawUI(AEGfxTexture* pTex, AEMtx33 transform, Color color);

	void DrawUIFollowCam(AEGfxTexture* pTex, AEMtx33 transform, Color color);

	/*	Draw text UI using noramlized coordinates 
		x,y, use range from -1.0f to 1.0f
		textSize, use range from -1.0 to 1.0f
	*/
	void DrawTextUI(std::string text, f32 x, f32 y, Color color, float textScale = Config::FONT_SCALE);

	/* Button class, use textOffset to position the text inside the button (use range 0.0 to 1.0f) */
	void DrawButton(Button btn, Color textColor = Color{0,0,0}, float textScale = Config::FONT_SCALE);
}

#endif // !GRAPHICSMANAGER_HPP
