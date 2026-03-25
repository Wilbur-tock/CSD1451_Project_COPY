#include "TextObject.hpp"

#include "AssetsManager.hpp"

TextObject::TextObject()
	: textContent{ "" }, textScale{ 1.0f }, transform{ { 0,0 }, {0,0} }, color{ 1,1,1 }, font{ AssetsManager::textFont },
	anchorMode{ Utils::AnchorMode::TOP_LEFT } {}

TextObject::TextObject(std::string _textContent, float _scale, Color _color, Transform _transform, Utils::AnchorMode _anchorMode)
	: textContent{ _textContent }, textScale{ _scale }, transform{ _transform.pos, _transform.size }, color{ _color }, font{ AssetsManager::textFont },
	anchorMode{ _anchorMode } {}

void TextObject::TextContent(std::string newText) {
	prevTextContent = textContent;
	textContent = newText;

	// Wrap text if there is a width stated for the text object
	if (transform.size.x > 0) {
		Utils::WrapText(textContent, font, textScale, transform.size.x);
		//std::cout << textContent << '\n';
	}
	hasUpdatePos = false;
}

void TextObject::Draw() {
	if (textContent.empty()) return;

	// If textContent is changed or position has not been set yet
	if (isSettingAnchorPos && prevTextContent != textContent && !hasUpdatePos) {
		SetAnchorPosition(parent);
		//std::cout << prevTextContent << " Setting Text Print Anchor Position " << textContent << ' ' << pos.x << ' ' << pos.y << '\n';
		hasUpdatePos = true;
	}

	// Update position if there is a offset need to constantly updated if textContent is updated
	float x = transform.pos.x, y = transform.pos.y;
	if (offsetPos.x != 0 || offsetPos.y != 0) {
		x += offsetPos.x;
		y += offsetPos.y;
	}

	// Use a stringstream to handle newline parsing automatically
	std::stringstream ss(textContent);
	std::string line;

	// Use default font if no font found
	if (font == 0) {
		font = AssetsManager::textFont;
	}

	// Initial setup
	f32 charW, charH;
	AEGfxGetPrintSize(font, "A", textScale, &charW, &charH);

	// Set position y
	f32 currentY = y - (charH * 1.0f); // minus charH because AEGfxPrint is BOTTOM-LEFT RECT MODE
	const f32 lineSpacing = 1.5f; // Adjust vertical gap between lines

	// Iterate through each line delimited by '\n'
	while (std::getline(ss, line)) {
		// Only render if the line isn't just whitespace or empty
		if (!line.empty()) {
			AEGfxPrint(AssetsManager::textFont,
				line.c_str(),
				x,
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

void TextObject::SetPosition(Utils::AnchorMode newMode, Transform const& parentTransform) {
	anchorMode = newMode;
	parent = parentTransform;
	hasUpdatePos = false;
	isSettingAnchorPos = true;

	SetAnchorPosition(parent);
}

void TextObject::SetPosition(Transform const& parentTransform) {
	SetPosition(anchorMode, parentTransform);
	//std::cout << "TextObject: " << textContent << " Prev:" << prevTextContent << ' ' << isSettingAnchorPos << hasUpdatePos <<  '\n';
}

void TextObject::SetOffsetPosition(float x, float y) {
	Utils::NormalizeCoordinates(x, y);
	offsetPos = { x,y };
}

void TextObject::SetAnchorPosition(Transform const& parentTransform) {
	SetTextAnchorPosition();
	f32 lineW, lineH;
	AEGfxGetPrintSize(AssetsManager::textFont, textContent.c_str(), textScale, &lineW, &lineH);

	// Get the normalize size of the text object
	float halfLineW = static_cast<float>(lineW * 0.5f);
	float halfLineH = static_cast<float>(lineH * 0.5f);

	// Need to normalize parent pos and size
	float x = parentTransform.pos.x;
	float y = parentTransform.pos.y;
	Utils::NormalizeCoordinates(x, y);

	float halfPanelW = parentTransform.size.x * 0.5f;
	float halfPanelH = parentTransform.size.y * 0.5f;
	Utils::NormalizeCoordinates(halfPanelW, halfPanelH);

	// Set position x
	switch (anchorMode) {
	case Utils::AnchorMode::TOP_LEFT:
		break;
	case Utils::AnchorMode::TOP_CENTER:
		transform.pos.x = x + halfPanelW - halfLineW;
		break;
	case Utils::AnchorMode::TOP_RIGHT:
		transform.pos.x = x + (halfPanelW * 2.0f) - lineW;
		break;
	case Utils::AnchorMode::CENTER_LEFT:
		transform.pos.y = y - halfPanelH + halfLineH;
		break;
	case Utils::AnchorMode::CENTER:
		transform.pos.y = y - halfPanelH + halfLineH;
		transform.pos.x = x + halfPanelW - halfLineW;
		break;
	case Utils::AnchorMode::CENTER_RIGHT:
		transform.pos.y = y - halfPanelH + halfLineH;
		transform.pos.x = x + (halfPanelW * 2.0f) - lineW;
		break;
	case Utils::AnchorMode::BOTTOM_LEFT:
		transform.pos.y = y - (halfPanelH * 2.0f) + lineH;
		break;
	case Utils::AnchorMode::BOTTOM_CENTER:
		transform.pos.y = y - (halfPanelH * 2.0f) + lineH;
		transform.pos.x = x + halfPanelW - halfLineW;
		break;
	case Utils::AnchorMode::BOTTOM_RIGHT:
		transform.pos.y = y - (halfPanelH * 2.0f) + lineH;
		transform.pos.x = x + (halfPanelW * 2.0f) - lineW;
		break;
	}
}

void TextObject::SetTextAnchorPosition() {
	// Get the normalized text width and height
	f32 textW, textH;
	AEGfxGetPrintSize(font, TextContent().c_str(), textScale, &textW, &textH);

	float x = parent.pos.x, y = parent.pos.y, borderGap = 0.01f;
	float halfW = parent.size.x * 0.5f, halfH = parent.size.y * 0.5f;

	// Position text object anchor mode according to the button pos and size
	switch (anchorMode) {
	case Utils::AnchorMode::TOP_LEFT: {
		Utils::NormalizeCoordinates(x, y);
		transform.pos.x = x + borderGap;
		transform.pos.y = y - borderGap;
		break;
	}
	case Utils::AnchorMode::TOP_CENTER: {
		x += halfW;
		Utils::NormalizeCoordinates(x, y);
		transform.pos.x = x - (textW / 2.0f);
		transform.pos.y = y;
		break;
	}
	case Utils::AnchorMode::TOP_RIGHT: {
		x += halfW;
		Utils::NormalizeCoordinates(x, y);
		transform.pos.x = x - (textW) - borderGap;
		transform.pos.y = y - borderGap;
		break;
	}
	case Utils::AnchorMode::CENTER_LEFT: {
		y -= halfH;
		Utils::NormalizeCoordinates(x, y);
		transform.pos.x = x + borderGap;
		transform.pos.y = y + (textH / 2.0f);
		break;
	}
	case Utils::AnchorMode::CENTER: {
		// Convert button position from corner rectmode to middle rectmode (world pos)
		// then convert to normalized position 
		// then minus half of the text width and height
		x += halfW;
		y -= halfH;
		Utils::NormalizeCoordinates(x, y);
		transform.pos.x = x - (textW / 2.0f);
		transform.pos.y = y + (textH / 2.0f);
		break;
	}
	case Utils::AnchorMode::CENTER_RIGHT: {
		x += x;
		y -= halfH;
		Utils::NormalizeCoordinates(x, y);
		transform.pos.x = x - (textW)-borderGap;
		transform.pos.y = y + (textH / 2.0f);
		break;
	}
	case Utils::AnchorMode::BOTTOM_LEFT: {
		y -= y;
		Utils::NormalizeCoordinates(x, y);
		transform.pos.x = x + borderGap;
		transform.pos.y = y + textH + borderGap;
		break;
	}
	case Utils::AnchorMode::BOTTOM_CENTER: {
		x += halfW;
		y -= y;
		Utils::NormalizeCoordinates(x, y);
		transform.pos.x = x - (textW / 2.0f);
		transform.pos.y = y + textH + borderGap;
		break;
	}
	case Utils::AnchorMode::BOTTOM_RIGHT: {
		x += x;
		y -= y;
		Utils::NormalizeCoordinates(x, y);
		transform.pos.x = x - textW - borderGap;
		transform.pos.y = y + textH + borderGap;
		break;
	}
	}
}