#include "TextObject.hpp"

#include "AssetsManager.hpp"

TextObject::TextObject(std::string _textContent, float _scale, Color _color, AEVec2 _pos)
	: textContent{ _textContent }, textScale{ _scale }, color{ _color }, pos{ _pos } {}

void TextObject::Draw() {
	if (textContent.empty()) return;

	// Use a stringstream to handle newline parsing automatically
	std::stringstream ss(textContent);
	std::string line;

	// Initial setup
	f32 charW, charH;
	AEGfxGetPrintSize(AssetsManager::textFont, "A", textScale, &charW, &charH);

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