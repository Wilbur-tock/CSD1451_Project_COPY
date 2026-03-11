#include "TextObject.hpp"

#include "AssetsManager.hpp"

TextObject::TextObject()
	: textContent{ "" }, textScale{ 1.0f }, color{ 1,1,1 }, pos{ 0,0 }, anchorMode{ Utils::AnchorMode::TOP_LEFT } {}

TextObject::TextObject(std::string _textContent, float _scale, Color _color, AEVec2 _pos, Utils::AnchorMode _anchorMode)
	: textContent{ _textContent }, textScale{ _scale }, color{ _color }, pos{ _pos }, anchorMode{ _anchorMode } {}

void TextObject::Draw() {
	if (textContent.empty()) return;

	// Use a stringstream to handle newline parsing automatically
	std::stringstream ss(textContent);
	std::string line;

	// Initial setup
	f32 charW, charH;
	AEGfxGetPrintSize(AssetsManager::textFont, "A", textScale, &charW, &charH);

	// Set position y
	f32 currentY = pos.y - (charH * 1.0f); // minus charH because AEGfxPrint is BOTTOM-LEFT RECT MODE
	const f32 lineSpacing = 1.5f; // Adjust vertical gap between lines

	// Iterate through each line delimited by '\n'
	while (std::getline(ss, line)) {
		// Only render if the line isn't just whitespace or empty
		if (!line.empty()) {
			AEGfxPrint(AssetsManager::textFont,
				line.c_str(),
				pos.x,
				currentY,
				textScale,
				color.Red(),
				color.Green(),
				color.Blue(),
				color.Alpha());
		}

		// Move the cursor down for the next line regardless of whether this line was empty
		currentY -= (charH * lineSpacing);
	}
}

void TextObject::SetPos(AEVec2 const& panelPos, AEVec2 const& panelSize) {
	f32 lineW, lineH;
	AEGfxGetPrintSize(AssetsManager::textFont, textContent.c_str(), textScale, &lineW, &lineH);

	float halfLineW = static_cast<float>(lineW * 0.5f);
	float halfLineH = static_cast<float>(lineH * 0.5f);

	float halfPanelW = panelSize.x * 0.5f;
	float halfPanelH = panelSize.y * 0.5f;
	Utils::NormalizeCoordinates(halfPanelW, halfPanelH);

	float x = panelPos.x;
	float y = panelPos.y;
	Utils::NormalizeCoordinates(x, y);

	// Set position x
	switch (anchorMode) {
	case Utils::AnchorMode::TOP_LEFT:
		break;
	case Utils::AnchorMode::TOP_CENTER:
		pos.x = x + halfPanelW - halfLineW;
		break;
	case Utils::AnchorMode::TOP_RIGHT:
		pos.x = x + (halfPanelW * 2.0f) - lineW;
		break;
	case Utils::AnchorMode::CENTER_LEFT:
		pos.y = y - halfPanelH + halfLineH;
		break;
	case Utils::AnchorMode::CENTER:
		pos.y = y - halfPanelH + halfLineH;
		pos.x = x + halfPanelW - halfLineW;
		break;
	case Utils::AnchorMode::CENTER_RIGHT:
		pos.y = y - halfPanelH + halfLineH;
		pos.x = x + (halfPanelW * 2.0f) - lineW;
		break;
	case Utils::AnchorMode::BOTTOM_LEFT:
		pos.y = y - (halfPanelH * 2.0f) + lineH;
		break;
	case Utils::AnchorMode::BOTTOM_CENTER:
		pos.y = y - (halfPanelH * 2.0f) + lineH;
		pos.x = x + halfPanelW - halfLineW;
		break;
	case Utils::AnchorMode::BOTTOM_RIGHT:
		pos.y = y - (halfPanelH * 2.0f) + lineH;
		pos.x = x + (halfPanelW * 2.0f) - lineW;
		break;
	}
}