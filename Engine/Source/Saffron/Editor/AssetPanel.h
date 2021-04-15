#pragma once

#include "Saffron/Core/FileIOManager.h"

namespace  Se
{
class AssetPanel : public Managed
{
public:
	struct AssetStat
	{
		String Full;
		String Stem;
		String Type;
		Filepath Path;

		AssetStat(String stem, String type, Filepath path)
			:Full(stem + type),
			Stem(Move(stem)),
			Type(Move(type)),
			Path(Move(path))
		{
		}
		AssetStat(const AssetStat &stat)
			: AssetStat(stat.Stem, stat.Type, stat.Path)
		{
		}

	};

	struct Drop
	{
		size_t StorageIndex;
		AssetStat *Stat;
	};

public:
	AssetPanel() = default;
	AssetPanel(Filepath path);

	void OnGuiRender();
	void SyncAssetPaths();

	void SetAssetFolderpath(Filepath folderpath) { m_AssetFolderPath = Move(folderpath); }
	const ArrayList<AssetStat> &GetAssetStats() const { return m_AssetStats; }

private:
	Filepath m_AssetFolderPath;
	ArrayList<AssetStat> m_AssetStats;
	Mutex m_FilepathMutex;
};
}

