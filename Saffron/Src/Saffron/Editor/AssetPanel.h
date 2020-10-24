#pragma once

#include "Saffron/Core/FileIOManager.h"

namespace  Se
{
class AssetPanel : public ReferenceCounted
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
			Stem(std::move(stem)),
			Type(std::move(type)),
			Path(std::move(path))
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
	AssetPanel(Filepath path);

	void OnGuiRender();
	void SyncAssetPaths();

	const ArrayList<AssetStat> &GetAssetStats() const { return m_AssetStats; }

private:
	Filepath m_AssetFolderPath;
	ArrayList<AssetStat> m_AssetStats;
};
}

