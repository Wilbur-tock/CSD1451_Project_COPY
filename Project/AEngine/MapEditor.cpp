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

using namespace MapManager;

namespace MapEditor {
	bool isDrawingNewMap = true, isShowingTileList = true;
	int totalLevelMap = 5;
	std::string const defaultMapLabel = "Editing Custom Map";
	std::string currentMapLabel = defaultMapLabel;
	std::string const defaultSavePath = "Assets/Map/level.txt", defaultLoadPath = "Assets/Map/default.txt";
	std::string loadPath = "", savePath = "";
	std::vector<std::vector<Tile>> newMap(Config::GRID_HEIGHT, std::vector<Tile>(Config::GRID_WIDTH));

	TileType defaultTile = TileType::TILE_BLOCK_GROUND_0;
	TileType drawingTileType = defaultTile;
	// Current selected tileId
	int tileId = static_cast<int>(defaultTile);

	// UI Objects
	UIManager::Panel mapLabelPanel{ {250,50}, {1,1,1}, Utils::AnchorMode::TOP_CENTER };
	TextObject helpText, debugText, mapLabelText;

	// If the Project/Assets/Map does not contain default.txt file it will create a new one if not just return
	static void CheckForDefault();
	static void LoadNewFile(std::string filePath);
	static void SaveNewFile(std::string filePath);
	static void ResetEditor();
	static void ResetMap(); // Reset the map 2D vector to be all of type 0

	// Draw out the graphics for MapEditor
	static void Draw();

	// Ensure that the file path is going back to my project folder NOT THE BIN FOLDER
	static std::string DebugFilePath(std::string filePath) { return "../../" + filePath; }
	// Change the drawing tile type
	static void ChangeTile(int &newTileId) {
		Utils::Wrap(newTileId, 1, static_cast<int>(TileType::TILE_COUNT) - 1);
		drawingTileType = static_cast<TileType>(newTileId);
	}

	static void CheckForDefault() {
		std::string filePath = defaultLoadPath;
		std::ifstream ifs{ filePath };
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

	static void LoadNewFile(std::string filePath) {
		MapManager::ReadMapData(filePath, newMap); // Reset the map to render
		savePath = filePath;
		if (filePath == defaultLoadPath) savePath = defaultSavePath;
		loadPath = filePath;
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

	static void ResetEditor() {
		// Reset settings
		drawingTileType = defaultTile;
		tileId = static_cast<int>(defaultTile);

		CheckForDefault();

		loadPath = defaultLoadPath;
		savePath = defaultSavePath;
		LoadNewFile(loadPath);
		currentMapLabel = defaultMapLabel;
	}

	static void ResetMap() {
		for (std::vector<Tile> &list : newMap) {
			for (Tile& tile : list) {
				tile.Type(static_cast<TileType>(0));
			}
		}
	}

	void Initialize() {
		helpText =  { "",  Utils::debuggerInfo.first * 0.9f, Utils::debuggerInfo.second,{-1.0f,.99f} };
		debugText = { "",  Utils::debuggerInfo.first * 0.8f, Utils::debuggerInfo.second, { 0.5f, 0.99f} };

		mapLabelText = { currentMapLabel, 0.75f, {0,0,0}, {0,0}, Utils::AnchorMode::CENTER };
		mapLabelText.SetPos(mapLabelPanel.Pos(), mapLabelPanel.Size());

		CameraManager::camera.SetPosition({ 0, 0 });
		CameraManager::camera.SetState(CameraManager::CameraController::INPUT);
		CameraManager::camera.SetZoomAllow(true);
	}

	void Update() {
		if (isDrawingNewMap) {
			ResetEditor();
			isDrawingNewMap = false;
		}

		int gridX = 0, gridY = 0;
		Utils::GetMouseGridIndex(gridX, gridY);

		// Check for input to draw the map editor
		if (gridX >= 0 && gridX < Config::GRID_WIDTH && gridY >= 0 && gridY < Config::GRID_HEIGHT) {
			if (AEInputCheckCurr(AEVK_LBUTTON)) {
				newMap[gridY][gridX].Type(drawingTileType);

				// Input to draw a column
				if (AEInputCheckCurr(AEVK_LCTRL) || AEInputCheckCurr(AEVK_RCTRL)) {
					for (int i = 0; i < Config::GRID_WIDTH; ++i) {
						newMap[gridY][i].Type(drawingTileType);
					}
				}

				// Input to draw a row
				if (AEInputCheckCurr(AEVK_LSHIFT) || AEInputCheckCurr(AEVK_RSHIFT)) {
					for (int i = 0; i < Config::GRID_HEIGHT; ++i) {
						newMap[i][gridX].Type(drawingTileType);
					}
				}
			}
			else if (AEInputCheckCurr(AEVK_RBUTTON)) { // Input to erase tile

				newMap[gridY][gridX].Type(TileType::TILE_EMPTY);
				// Input to erase a column
				if (AEInputCheckCurr(AEVK_LCTRL) || AEInputCheckCurr(AEVK_RCTRL)) {
					for (int i = 0; i < Config::GRID_WIDTH; ++i) {
						newMap[gridY][i].Type(TileType::TILE_EMPTY);
					}
				}

				// Input to erase a row
				if (AEInputCheckCurr(AEVK_LSHIFT) || AEInputCheckCurr(AEVK_RSHIFT)) {
					for (int i = 0; i < Config::GRID_HEIGHT; ++i) {
						newMap[i][gridX].Type(TileType::TILE_EMPTY);
					}
				}
			}
		}

		// Input to reset map layout
		if (AEInputCheckTriggered(AEVK_R)) {
			ResetMap();
		}

		// Change drawing tile category
		//s32 scroll = 0;
		//AEInputMouseWheelDelta(&scroll);
		//if (scroll != 0) {
		//	tileId -= scroll;
		//	Utils::Wrap(tileId, 1, TILE_COUNT - 1);
		//	drawingTileType = static_cast<TileType>(tileId);
		//}
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
					std::string newLoadPath = "Assets/Map/level" + std::to_string(i) + ".txt";
					LoadNewFile(newLoadPath);

					currentMapLabel = "Editing Level " + std::to_string(i) + " Map";
				}
			}

			// Input CTRL + S
			// Save file on both BIN folder and my project folder if debugger is on
			if (AEInputCheckTriggered(AEVK_S)) {
				SaveNewFile(savePath);
				if (Utils::isDebuggerOn) {
					SaveNewFile(DebugFilePath(savePath));
				}

				LevelManager::Load(); // Reload the map data for each level
				isDrawingNewMap = true;
			}
		}

		Draw();
	}

	static void Draw() {
		// Render the map drawn
		MapManager::DrawMap(newMap);

		// Display the label on which map is being editing 
		mapLabelPanel.Draw();
		mapLabelText.textContent = currentMapLabel;
		mapLabelText.Draw();

		// Print out the information on how to use the editor
		std::string helpStr =
			"Current File: " + loadPath +
			"\nSave File: " + savePath +
			"\n\n[M] Back to MAIN MENU"
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

		helpText.textContent = helpStr;
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
			debugText.textContent = "Grid\nIndex X: " + std::to_string(gridX) + " Y: " + std::to_string(gridY)
				+ "\nTile: " + GetTileName(tile.Type())
				+ "\nPos X: " + Utils::FormatFloat(newMap[gridY][gridX].transform.pos.x, 2) + " Y: " + Utils::FormatFloat(newMap[gridY][gridX].transform.pos.y, 2)
				+ "\nCamera " + std::to_string(CameraManager::camera.GetState()) +
				+"\nPos X:" + Utils::FormatFloat(x, 2) + " Y: " + Utils::FormatFloat(y, 2)
				+ "\nMouse\nScreen Pos X: " + Utils::FormatFloat(mouseX, 2) + " Y: " + Utils::FormatFloat(mouseY, 2)
				+ "\nWorld Pos X: " + Utils::FormatFloat(mX, 2) + " Y: " + Utils::FormatFloat(mY, 2);

			debugText.Draw();
		}
	}

	void Exit() {
		isDrawingNewMap = true;
		CameraManager::camera.SetZoomAllow(true);
	}
}