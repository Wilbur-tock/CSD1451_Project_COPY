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
			UIManager::Panel{{500,150}, Color{1.0f,0.5f,0.5f}, Utils::AnchorMode::TOP_RIGHT},
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
			newMsg = "Before the start of the game, you will have the choice to choose 1 out of 3 abilities to add to your inventory.";
			break;
		case 2:
			newMsg = "You can choose any of your abilities to use in your inventory! Tip: If you use the default ability this round, for the next round you can choose a stats boost to help you next round";
			break;
		case 3:
			newMsg = "After selecting your ability to use, you can use MOUSE SCROLL to zoom in and out of the map. se WASD to move the map around";
			break;
		case 4:
			newMsg = "You should use the trajectory to help you aim and to shoot hold LEFT MOUSE CLICK and relase to shoot. \nTip: if you are holding LEFT MOUSE CLICK and wants to cancel, you can by clicking RIGHT MOUSE CLICK";
			break;
		case 5:
			newMsg = "You and your opponenet will take turn shooting projectiiles at each other. In order to win, you have to kill all your opponent units or just the BOSS unit. Good luck!";
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