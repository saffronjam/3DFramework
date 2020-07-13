#pragma once

#include <string>
#include <assert.h>
#include <memory>
#include <algorithm>
#include <sstream>

#include "VeWin.h"
#include "VeException.h"
#include "Color.h"


class Surface
{
public:
	Surface(unsigned int width, unsigned int height) noexcept;
	Surface(Surface&& source) noexcept;
	Surface(Surface&) = delete;
	Surface& operator=(Surface&& donor) noexcept;
	Surface& operator=(const Surface&) = delete;
	~Surface();
	void Clear(Color fillValue) noexcept;
	void PutPixel(unsigned int x, unsigned int y, Color c) noexcept(!IS_DEBUG);
	Color GetPixel(unsigned int x, unsigned int y) const noexcept(!IS_DEBUG);
	unsigned int GetWidth() const noexcept;
	unsigned int GetHeight() const noexcept;
	Color* GetBufferPtr() noexcept;
	const Color* GetBufferPtr() const noexcept;
	const Color* GetBufferPtrConst() const noexcept;
	static Surface FromFile(const std::string& name);
	void Save(const std::string& filename) const;
	void Copy(const Surface& src) noexcept(!IS_DEBUG);
private:
	Surface(unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBufferParam) noexcept;
private:
	std::unique_ptr<Color[]> pBuffer;
	unsigned int width;
	unsigned int height;


public:
	class Exception : public VeException
	{
	public:
		Exception(int line, const char* file, std::string note) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		const std::string& GetNote() const noexcept;
	private:
		std::string note;
	};
};
