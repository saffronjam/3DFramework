#pragma once

#include "Saffron/Core/FileIOManager.h"

namespace Se
{
class AssetPanel : public Managed
{
public:
	struct AssetStat
	{
		String Full;
		String Stem;
		String Type;
		Path Filepath;

		AssetStat(String stem, String type, Path path) :
			Full(stem + type),
			Stem(Move(stem)),
			Type(Move(type)),
			Filepath(Move(path))
		{
		}

		AssetStat(const AssetStat& stat) :
			AssetStat(stat.Stem, stat.Type, stat.Filepath)
		{
		}
	};

	struct Drop
	{
		size_t StorageIndex;
		AssetStat* Stat;
	};

public:
	AssetPanel() = default;
	AssetPanel(Path path);

	void OnGuiRender();
	void SyncAssetPaths();

	void SetAssetFolderpath(Path folderpath) { _assetFolderPath = Move(folderpath); }

	const List<AssetStat>& GetAssetStats() const { return _assetStats; }

private:
	Path _assetFolderPath;
	List<AssetStat> _assetStats;
	Mutex _filepathMutex;
};
}
