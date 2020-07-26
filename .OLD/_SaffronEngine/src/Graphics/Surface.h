#pragma once

#include <string>
#include <optional>
#include <vector>
#include <string>

#include "Config.h"
#include "IException.h"
#include "Color.h"

class Surface
{
public:
    Surface(unsigned int width, unsigned int height);
    Surface(Surface &&source) noexcept = default;
    Surface(Surface &) = delete;
    Surface &operator=(Surface &&donor) noexcept = default;
    Surface &operator=(const Surface &) = delete;
    ~Surface() = default;

    static Surface FromFile(const std::string &name);
    void Clear(const Color &fillValue) noexcept;

    [[nodiscard]] const Color &GetPixel(unsigned int x, unsigned int y) const noxnd;
    [[nodiscard]] unsigned int GetWidth() const noexcept;
    [[nodiscard]] unsigned int GetHeight() const noexcept;
    const std::vector<Color> &GetPixels() noexcept;
    Color *GetBufferPtr() noexcept;
    [[nodiscard]] const Color *GetBufferPtr() const noexcept;
    [[nodiscard]] const std::string &GetFilepath() const noexcept;

    void SetPixel(unsigned int x, unsigned int y, const Color &c) noxnd;

private:
    unsigned int m_width, m_height;
    std::vector<Color> m_pixels;
    std::string m_filepath;

public:
    class Exception : public IException
    {
    public:
        Exception(int line, const char *file, const char *errorString) noexcept;
        const char *what() const noexcept override;
        const char *GetType() const noexcept override;
        const char *GetErrorString() const noexcept;

    private:
        std::string errorString;
    };
};