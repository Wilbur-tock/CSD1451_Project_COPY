#include "LevelManager.hpp"

#include <filesystem>
#include <cctype>

namespace LevelManager {
	namespace fs = std::filesystem;

	namespace {
		constexpr char LEVEL_FOLDER[] = "Assets/DataMap";
		int selectedLevelIndex = -1;

		bool TryExtractLevelId(fs::path const& path, int& outLevelId) {
			if (!fs::is_regular_file(path) || path.extension() != ".txt") {
				return false;
			}

			std::string stem = path.stem().string(); // e.g. "level3"
			const std::string prefix = "level";

			if (stem.rfind(prefix, 0) != 0) {
				return false;
			}

			std::string suffix = stem.substr(prefix.size());
			if (suffix.empty()) {
				return false; // skips "level.txt"
			}

			bool allDigits = std::all_of(
				suffix.begin(),
				suffix.end(),
				[](unsigned char c) { return std::isdigit(c) != 0; }
			);

			if (!allDigits) {
				return false;
			}

			outLevelId = std::stoi(suffix);
			return outLevelId > 0;
		}
	}

	Level* selectedLevel = nullptr;
	std::vector<Level> levelDatas;

	int LevelCount() {
		return static_cast<int>(levelDatas.size());
	}

	bool IsValidLevelIndex(int levelIndex) {
		return levelIndex >= 0 && levelIndex < static_cast<int>(levelDatas.size());
	}

	Level const* GetLevel(int levelIndex) {
		if (!IsValidLevelIndex(levelIndex)) {
			return nullptr;
		}

		return &levelDatas[levelIndex];
	}

	std::string GetLevelName(int levelIndex) {
		Level const* level = GetLevel(levelIndex);
		return level ? level->Name() : "";
	}

	void LoadAllLevels() {
		selectedLevel = nullptr;
		selectedLevelIndex = -1;
		levelDatas.clear();

		std::vector<int> discoveredLevelIds;

		try {
			for (auto const& entry : fs::directory_iterator(LEVEL_FOLDER)) {
				int levelId = 0;
				if (TryExtractLevelId(entry.path(), levelId)) {
					discoveredLevelIds.push_back(levelId);
				}
			}
		}
		catch (fs::filesystem_error const& e) {
			std::cerr << "[LevelManager] Failed to scan " << LEVEL_FOLDER
				<< ": " << e.what() << '\n';
			return;
		}

		std::sort(discoveredLevelIds.begin(), discoveredLevelIds.end());
		discoveredLevelIds.erase(
			std::unique(discoveredLevelIds.begin(), discoveredLevelIds.end()),
			discoveredLevelIds.end()
		);

		levelDatas.reserve(discoveredLevelIds.size());

		for (int levelId : discoveredLevelIds) {
			levelDatas.emplace_back(levelId);
			levelDatas.back().Load();
		}

		std::cout << "[LevelManager] Loaded " << levelDatas.size() << " level(s).\n";
	}

	void LoadLevel(int levelIndex) {
		if (!IsValidLevelIndex(levelIndex)) {
			std::cerr << "[LevelManager] Invalid level index in LoadLevel(): "
				<< levelIndex << '\n';
			return;
		}

		levelDatas[levelIndex].Load();
	}

	void SelectLevel(int levelIndex) {
		if (!IsValidLevelIndex(levelIndex)) {
			std::cerr << "[LevelManager] Invalid level index in SelectLevel(): "
				<< levelIndex << '\n';
			selectedLevel = nullptr;
			selectedLevelIndex = -1;
			return;
		}

		selectedLevelIndex = levelIndex;
		selectedLevel = &levelDatas[levelIndex];
	}

	Level* CurrentLevel() {
		return selectedLevel;
	}

	void InitializeLevel() {
		if (!selectedLevel) {
			std::cerr << "[LevelManager] Cannot initialize level: no selected level.\n";
			return;
		}

		selectedLevel->Initalize();
	}

	void UpdateLevel() {
		if (!selectedLevel) {
			return;
		}

		selectedLevel->Update();
	}

	void FreeLevel() {
		if (!selectedLevel) {
			return;
		}

		selectedLevel->Free();
	}

	void UnloadLevel() {
		levelDatas.clear();
		selectedLevel = nullptr;
		selectedLevelIndex = -1;
	}
}