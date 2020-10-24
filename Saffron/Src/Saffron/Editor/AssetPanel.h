#pragma once

#include "Saffron/Core/FileIOManager.h"

namespace  Se
{
class AssetPanel : public ReferenceCounted
{
public:
	struct AssetStat
	{
		std::string Full;
		std::string Stem;
		std::string Type;
		fs::path Path;

		AssetStat(std::string stem, std::string type, fs::path path)
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
	AssetPanel(fs::path path);

	void OnGuiRender();
	void SyncAssetPaths();

	const std::vector<AssetStat> &GetAssetStats() const { return m_AssetStats; }

private:
	fs::path m_AssetFolderPath;
	std::vector<AssetStat> m_AssetStats;
};
}

