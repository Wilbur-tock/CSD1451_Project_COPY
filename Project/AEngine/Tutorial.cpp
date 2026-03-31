/*
Authors: Dayrius
Description: Display the tutorial messages

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "Tutorial.hpp"

#include "UIManager.hpp"

namespace Tutorial {
	UIManager::PanelText panel;
	bool isActive = false;
	int currentSteps = 0;

	void Initialize() {
		panel = UIManager::PanelText{
			UIManager::Panel{{500,150}, Color{0,0,0}, Utils::AnchorMode::TOP_RIGHT},
			TextObject{"", 0.75f, Color{1,1,1}, {{0,0}, {450,150}}}
		};

		isActive = true;
		currentSteps = 0;

		SetTutorialMsg(1);
	}

	void SetTutorialMsg(int steps) {
		if (steps <= currentSteps || !isActive) return;

		currentSteps = steps;

		std::string newMsg = "";
		switch (steps) {
		case 1:
			newMsg = "Welcome to Fortress Fight! Before the battle begins, choose one of three abilities to add to your inventory. Tip: These abilities provide a powerful advantage!";
			break;
		case 2:
			newMsg = "Choose any ability from your inventory! Tip: If you use your default ability this round, you'll be able to pick a stat boost for the next round.";
			break;
		case 3:
			newMsg = "After selecting an ability, use the Mouse Wheel to zoom in or out. Use WASD to pan the map and find your target.";
			break;
		case 4:
			newMsg = "Use the trajectory to help you aim. To shoot, hold Left-Click and release. If you want to cancel your shot while holding Left-Click, simply Right-Click.";
			break;
		case 5:
			newMsg = "You and your opponent will take turns firing projectiles at each other. To win, eliminate all of your opponent's units or defeat their BOSS unit. Good luck!";
			break;
		case 6:
			isActive = false;
			break;
		}
		panel.Update(newMsg);
	}

	void Draw() {
		if (!isActive) return;
		panel.Draw();
	}
}