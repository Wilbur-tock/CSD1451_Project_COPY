#include "MapEditor.hpp"

#include "MapManager.hpp"
#include "CameraManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "Utils.hpp"
#include "Tile.hpp"
#include "GameStateManager.hpp"
#include "LevelManager.hpp"
#include "UIManager.hpp"

struct MapConstraint {
	TileType type;
	int requiredCount = 0;
	int currentCount = 0;
	bool isMet = false;
};

std::vector<MapConstraint> constraints;

void SetUpConstraints() {
	// Reset
	constraints = {
		{TileType::TILE_ENEMY_BOSS, 1},
		{TileType::TILE_PLAYER_CHARACTER, 1},
		{TileType::TILE_ENEMY_CANNON, 1},
		{TileType::TILE_PLAYER_CANNON, 1},
	};
}

void UpdateCount(TileType type, int valueToModify) {
	for (MapConstraint& constraint : constraints) {
		if (type == constraint.type) {
			constraint.currentCount += valueToModify;
			constraint.isMet = constraint.currentCount >= constraint.requiredCount;
			return;
		}
	}
}

using namespace MapManager;

namespace MapEditor {
	bool isDrawingNewMap = true, isShowingTileList = true;
	int totalLevelMap = 5;

	std::string const defaultMapLabel = "Editing Custom Map";
	std::string currentMapLabel = defaultMapLabel;

	// Loading and saving file path
	std::string const defaultLoadName = "default";
	std::string const defaultSaveName = "level"; // custom level
	std::string const defaultLoadPath = MapManager::MapFilePath() + defaultLoadName + ".txt";
	std::string const defaultSavePath = MapManager::MapFilePath() + defaultSaveName + ".txt";
	// Current File paths
	std::string loadPath = "", savePath = "";

	// 2D Vector to store the tile map data 
	std::vector<std::vector<Tile>> newMap(Config::GRID_HEIGHT, std::vector<Tile>(Config::GRID_WIDTH));

	TileType defaultTile = TileType::TILE_BLOCK_GROUND_0;
	TileType drawingTileType = defaultTile;
	// Current selected tileId
	int tileId = static_cast<int>(defaultTile);

	bool prevIsHoveringOverUI = false;
	bool isHoveringOverUI = false;
	// UI Objects
	UIManager::Panel mapLabelPanel{ { 250, 50 }, { 1,1,1 }, Utils::AnchorMode::TOP_CENTER };
	UIManager::PopupPanel constraintsPopup;
	TextObject helpText, debugText, mapLabelText;

	// If the project file does not contain default.txt file it will create a new one if not just return
	static void CheckForDefault();
	static void LoadNewFile(std::string fileName);
	static void SaveNewFile(std::string fileName);

	static void Save();
	static void ResetEditor();
	static void ResetMap(); // Reset the map 2D vector to be all of type 0

	static void ChangeBlock(int idX, int idY, bool isDrawing = true);
	// Draw out the graphics for MapEditor
	static void Draw();
	// Check whatever or not the level satisfy the requirements for editing level
	static bool ConstraintChecks();
	void CheckInputs();

	// Ensure that the file path is going back to my project folder NOT THE BIN FOLDER if the debugger is on
	static std::string DebugFilePath(std::string filePath) { return "../../" + filePath; }

	// Change the drawing tile type
	static void ChangeTile(int &newTileId) {
		Utils::Wrap(newTileId, 1, static_cast<int>(TileType::TILE_COUNT) - 1);
		drawingTileType = static_cast<TileType>(newTileId);
	}

	static void CheckForDefault() {
		std::string filePath = defaultLoadPath;
		std::ifstream ifs{ filePath }; // return if containt default.txt file if not create one
		if (ifs) {
			//std::cout << "Contain default.txt " << fileName << std::endl;
			return;
		}

		std::ofstream output{ filePath };
		if (!output.is_open()) {
			std::cerr << "Error: Could not open file at " << filePath << std::endl;
			return;
		}

		// Write to the text file
		for (int y = 0; y < Config::GRID_HEIGHT; ++y) {
			for (int x = 0; x < Config::GRID_WIDTH; ++x) {
				int val = 0;
				newMap[y][x].Type(static_cast<TileType>(val));
				output << val << " ";
			}
			output << std::endl;
		}

		output.close();
	}

	static void LoadNewFile(std::string fileName) {
		// Read map data from txt file
		MapManager::ReadMapData(fileName, newMap); // Reset the map to render
		// Set save path
		savePath = MapManager::mapFilePath + fileName + ".txt";;
		if (fileName == defaultLoadName) savePath = defaultSavePath; // if fileName is same as default loadName 
		// Set load path
		loadPath = MapManager::mapFilePath + fileName + ".txt";

		// Reset and Check constraints for loaded map again
		SetUpConstraints();
		for (std::vector<Tile> const& vec : newMap) {
			for (Tile const& tile : vec) {
				for (MapConstraint& constraint : constraints) {
					if (tile.Type() != constraint.type) continue;

					UpdateCount(tile.Type(), 1);
				}
			}
		}
	}

	static void SaveNewFile(std::string filePath) {
		std::ofstream output{ filePath };

		if (!output.is_open()) {
			std::cerr << "Error: Could not open file at " << filePath << std::endl;
			return;
		}

		// Write into the text file and save the file
		for (int y = 0; y < Config::GRID_HEIGHT; ++y) {
			for (int x = 0; x < Config::GRID_WIDTH; ++x) {
				output << static_cast<int>(newMap[y][x].Type()) << " ";
			}
			output << std::endl;
		}

		output.close();
	}

	static void Save() {
		if (!ConstraintChecks()) return;

		SaveNewFile(savePath);
		if (Utils::isDebuggerOn) {
			SaveNewFile(DebugFilePath(savePath));
		}

		LevelManager::LoadAllLevels(); // Reload every level map
		isDrawingNewMap = true;
	}

	static void ResetEditor() {
		SetUpConstraints();

		// Reset settings
		drawingTileType = defaultTile;
		tileId = static_cast<int>(defaultTile);

		CheckForDefault();

		loadPath = defaultLoadPath;
		savePath = defaultSavePath;
		LoadNewFile(defaultLoadName);
		currentMapLabel = defaultMapLabel;
	}

	static void ResetMap() {
		for (std::vector<Tile> &list : newMap) {
			for (Tile& tile : list) {
				ChangeBlock(tile.tileId.x, tile.tileId.y, false);
			}
		}
	}

	static void ChangeBlock(int idX, int idY, bool isDrawing) {
		Tile& tile = newMap[idY][idX];
		if ((tile.Type() == drawingTileType && isDrawing) 
			|| (!isDrawing && tile.Type() == TileType::TILE_EMPTY) ) 
			return; // Don't draw the same block type as the tile or erase air

		if (isDrawing) {
			UpdateCount(tile.Type(), -1);
			tile.Type(drawingTileType);
			UpdateCount(tile.Type(), 1);
		}
		else {
			UpdateCount(tile.Type(), -1);
			tile.Type(TileType::TILE_EMPTY);
		}
	}

	static void Draw() {
		// Render the map drawn
		MapManager::DrawMap(newMap);

		// Draw UI
		// Display the label on which map is being editing 
		mapLabelPanel.Draw();
		mapLabelText.TextContent(currentMapLabel);
		mapLabelText.Draw();

		if (constraintsPopup.IsActive()) {
			constraintsPopup.Draw();
		}

		// Print out the information on how to use the editor
		std::string helpStr =
			"Current File: " + loadPath +
			"\nSave File: " + savePath +
			"\n\nMap Constraints:";

		for (MapConstraint const& constraint : constraints) {
			helpStr += "\n" + GetTileName(constraint.type) + ' ' + std::to_string(constraint.currentCount) + '/' + std::to_string(constraint.requiredCount);
		}

		helpStr +="\n\nShortcuts:"
			"\n[M] Back to MAIN MENU"
			"\n[CTRL + S] Save file"
			"\n[R] Reset Map"
			"\n[LEFT CLICK] Place Tile"
			"\n[RIGHT CLICK] Erase Tile"
			"\n\n[T] Toggle Tile List ";

		if (isShowingTileList) {
			helpStr += "[SHOWING]\n[Q] Scroll UP\n[E] Scroll DOWN\nSelected Tile: "
				+ GetTileName(static_cast<TileType>(tileId)) + " (" + std::to_string(tileId) + ")";
			// List out the type of tiles able to be drawn
			for (int i = 0; i < static_cast<int>(TileType::TILE_COUNT) - 1; ++i) {
				int id = i + 1;
				std::string str = "[ ] ";
				if (tileId == id) {
					str = "[O] ";
				}
				helpStr += "\n" + str + GetTileName(static_cast<TileType>(id));
			}
		}
		else {
			helpStr += "[HIDING]";
		}

		helpText.TextContent(helpStr);
		helpText.Draw();

		// Print out debugger information about the map editor
		if (Utils::isDebuggerOn) {
			f32 x, y;
			AEGfxGetCamPosition(&x, &y);

			s32 mouseX, mouseY;
			AEInputGetCursorPosition(&mouseX, &mouseY);
			float mX = 0, mY = 0;
			Utils::GetWorldMousePos(mX, mY);

			int gridX = 0, gridY = 0;
			Utils::GetMouseGridIndex(gridX, gridY);

			//std::cout << (*currentMap).size() << ' ' << (*currentMap)[0].size() << '\n';
			Tile& tile = (*currentMap)[gridY][gridX];
			debugText.TextContent("Grid\nIndex X: " + std::to_string(gridX) + " Y: " + std::to_string(gridY)
				+ "\nTile: " + GetTileName(tile.Type())
				+ "\nPos X: " + Utils::FormatFloat(newMap[gridY][gridX].transform.pos.x, 2) + " Y: " + Utils::FormatFloat(newMap[gridY][gridX].transform.pos.y, 2)
				+ "\nCamera " + std::to_string(CameraManager::camera.GetState()) +
				+"\nPos X:" + Utils::FormatFloat(x, 2) + " Y: " + Utils::FormatFloat(y, 2)
				+ "\nMouse\nScreen Pos X: " + Utils::FormatFloat(mouseX, 2) + " Y: " + Utils::FormatFloat(mouseY, 2)
				+ "\nWorld Pos X: " + Utils::FormatFloat(mX, 2) + " Y: " + Utils::FormatFloat(mY, 2) );

			debugText.Draw();
		}
	}

	static bool ConstraintChecks() {
		for (MapConstraint const& constraint : constraints) {
			if (!constraint.isMet) {
				std::string str = "You must have at least " + std::to_string(constraint.requiredCount)
					+ ' ' + GetTileName(constraint.type) + " in your map!";
				std::cout << "[Error] " + str << '\n';
				constraintsPopup.Popup(str);
				return false;
			}
		}

		return true;
	}

	void CheckInputs() {
		if (isHoveringOverUI || prevIsHoveringOverUI) return;

		int gridX = 0, gridY = 0;
		Utils::GetMouseGridIndex(gridX, gridY);

		// Check for input to draw the map editor
		if (!Utils::IsTileIndexOutOfBounds(gridX, gridY)) {
			bool leftHold = AEInputCheckCurr(AEVK_LBUTTON);
			bool rightHold = AEInputCheckCurr(AEVK_RBUTTON);
			bool ctrlHold = AEInputCheckCurr(AEVK_LCTRL) || AEInputCheckCurr(AEVK_RCTRL);
			bool shiftHold = AEInputCheckCurr(AEVK_LSHIFT) || AEInputCheckCurr(AEVK_RSHIFT);

			if (leftHold || rightHold) {
				bool isDrawing = leftHold; // left click for drawing right click for erasing
				ChangeBlock(gridX, gridY, isDrawing);

				// Input to draw/erase a column
				if (ctrlHold) {
					for (int i = 0; i < Config::GRID_WIDTH; ++i) {
						ChangeBlock(i, gridY, isDrawing);
					}
				}

				// Input to draw/erase a column
				if (shiftHold) {
					for (int i = 0; i < Config::GRID_HEIGHT; ++i) {
						ChangeBlock(gridX, i, isDrawing);
					}
				}
			}
		}

		// Input to reset map layout
		if (AEInputCheckTriggered(AEVK_R)) {
			ResetMap();
		}

		if (AEInputCheckTriggered(AEVK_Q)) {
			ChangeTile(--tileId);
		}
		if (AEInputCheckTriggered(AEVK_E)) {
			ChangeTile(++tileId);
		}

		// Input to toggle drawing tile list
		if (AEInputCheckTriggered(AEVK_T)) {
			isShowingTileList = !isShowingTileList;
		}

		if (AEInputCheckCurr(AEVK_LCTRL) || AEInputCheckCurr(AEVK_RCTRL)) {
			// Input CTRL + {NUMBER} to load a new LEVEL MAP to edit
			for (int i = 1; i <= totalLevelMap; ++i) {
				if (AEInputCheckTriggered(static_cast<u8>(AEVK_0 + i))) {
					// Load file from bin folder only
					std::string newLoadFile = "level" + std::to_string(i);
					LoadNewFile(newLoadFile);

					currentMapLabel = "Editing Level " + std::to_string(i) + " Map";
				}
			}

			// Input CTRL + S
			// Save file on both BIN folder and my project folder if debugger is on
			if (AEInputCheckTriggered(AEVK_S)) {
				Save();
			}
		}
	}

	void Initialize() {
		helpText = { "",  Utils::debuggerInfo.first * 0.9f, Utils::debuggerInfo.second,{-1.0f,.99f} };
		debugText = { "",  Utils::debuggerInfo.first * 0.8f, Utils::debuggerInfo.second, { 0.5f, 0.99f} };

		constraintsPopup = UIManager::PopupPanel{ false, UIManager::Panel{{500,250}}, TextObject{"", 0.5f, Color{1.0f,0.0f,0.0f}} };
		constraintsPopup.AddOffsetPosition({ 0,150 });

		mapLabelPanel = UIManager::Panel{ { 250, 50 }, { 1,1,1 }, Utils::AnchorMode::TOP_CENTER };

		// mapLabelText is a child object of the mapLabelPanel
		mapLabelText = TextObject{ currentMapLabel, 0.75f, {0,0,0}, { {0,0}, {0,0} }, Utils::AnchorMode::CENTER};
		mapLabelText.SetPosition(mapLabelPanel.GetTransform());

		CameraManager::camera.SetPosition({ 0, 0 });
		CameraManager::camera.SetState(CameraManager::CameraController::INPUT);
		CameraManager::camera.SetZoomAllow(true);
	}

	void Update() {
		if (isDrawingNewMap) {
			ResetEditor();
			isDrawingNewMap = false;
		}

		constraintsPopup.Update();

		prevIsHoveringOverUI = isHoveringOverUI;
		isHoveringOverUI = constraintsPopup.IsHovered() && constraintsPopup.IsActive();

		CheckInputs();

		Draw();
	}

	void Exit() {
		isDrawingNewMap = true;
	}
}