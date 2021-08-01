#pragma once

#include "Saffron/Base.h"
#include "Saffron/Common/Layer.h"

namespace Se
{
class LayerStack
{
public:
	void Attach(std::shared_ptr<Layer> layer);
	void Detach(std::shared_ptr<Layer> layer);
	void Clear();

	auto begin() const -> std::vector<std::shared_ptr<Layer>>::const_iterator;
	auto end() const -> std::vector<std::shared_ptr<Layer>>::const_iterator;

	auto begin() -> std::vector<std::shared_ptr<Layer>>::iterator;
	auto end() -> std::vector<std::shared_ptr<Layer>>::iterator;

private:
	std::vector<std::shared_ptr<Layer>> _layers;
};
}
