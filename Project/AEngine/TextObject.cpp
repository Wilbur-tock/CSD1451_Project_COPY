#include "TextObject.hpp"

#include "AssetsManager.hpp"

TextObject::TextObject(std::string _textContent, float _scale, Color _color, AEVec2 _pos, Utils::RectMode _rectMode)
	: textContent{ _textContent }, textScale{ _scale }, color{ _color }, pos{ _pos }, rectMode{ _rectMode } {}

void TextObject::Draw() {
	if (textContent.empty()) return;

	// Use a stringstream to handle newline parsing automatically
	std::stringstream ss(textContent);
	std::string line;

	// Initial setup
	f32 charW, charH;
	AEGfxGetPrintSize(AssetsManager::textFont, "A", textScale, &charW, &charH);

	// Set position y
	f32 currentY = pos.y;
	const f32 lineSpacing = 1.5f; // Adjust vertical gap between lines

	// Iterate through each line delimited by '\n'
	while (std::getline(ss, line)) {
		// Only render if the line isn't just whitespace or empty
		if (!line.empty()) {
			f32 lineW, lineH;
			AEGfxGetPrintSize(AssetsManager::textFont, line.c_str(), textScale, &lineW, &lineH);

			// currentY - lineH ensures the text is drawn relative to its baseline
			AEGfxPrint(AssetsManager::textFont,
				line.c_str(),
				pos.x,
				currentY - lineH,
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

void TextObject::SetPos(AEVec2 panelPos, AEVec2 panelSize) {
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
	switch (rectMode) {
	case Utils::RectMode::RECT_TOP_LEFT:
		break;
	case Utils::RectMode::RECT_TOP_CENTER:
		pos.x = x + halfPanelW - halfLineW;
		break;
	case Utils::RectMode::RECT_TOP_RIGHT:
		pos.x = x + (halfPanelW * 2.0f) - lineW;
		break;
	case Utils::RectMode::RECT_CENTER_LEFT:
		pos.y = y - halfPanelH + halfLineH;
		break;
	case Utils::RectMode::RECT_CENTER:
		pos.y = y - halfPanelH + halfLineH;
		pos.x = x + halfPanelW - halfLineW;
		break;
	case Utils::RectMode::RECT_CENTER_RIGHT:
		pos.y = y - halfPanelH + halfLineH;
		pos.x = x + (halfPanelW * 2.0f) - lineW;
		break;
	case Utils::RectMode::RECT_BOTTOM_LEFT:
		pos.y = y - (halfPanelH * 2.0f) + lineH;
		break;
	case Utils::RectMode::RECT_BOTTOM_CENTER:
		pos.y = y - (halfPanelH * 2.0f) + lineH;
		pos.x = x + halfPanelW - halfLineW;
		break;
	case Utils::RectMode::RECT_BOTTOM_RIGHT:
		pos.y = y - (halfPanelH * 2.0f) + lineH;
		pos.x = x + (halfPanelW * 2.0f) - lineW;
		break;
	}
}