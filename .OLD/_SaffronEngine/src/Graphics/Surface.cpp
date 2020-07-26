#include "Surface.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

#include "Log.h"

Surface::Surface(unsigned int width, unsigned int height)
        : m_width(width),
          m_height(height),
          m_pixels(m_width * m_height)
{
}

Surface Surface::FromFile(const std::string &name)
{
    int width, height, nChannels;
    unsigned char *imageRawData = stbi_load(name.c_str(), &width, &height, &nChannels, 0);

    if (imageRawData == nullptr)
    {
        LogWarningUser("Failed to load %s into surface: File not found", name.c_str());
        return Surface(0u, 0u);
    }

    if (nChannels != 3 && nChannels != 4)
    {
        LogWarningUser("Failed to load %s into surface: Only 3- and 4-channels are supported", name.c_str());
        return Surface(0u, 0u);
    }

    Surface surf(width, height);
    surf.m_filepath = name;
    const unsigned char *currDataPointer = imageRawData;
    const unsigned int nPixels = surf.GetWidth() * surf.GetHeight();
    if (nChannels == 3)
    {
        for (size_t i = 0; i < nPixels; i++, currDataPointer += nChannels)
        {
            float r = static_cast<float>(*currDataPointer) / 255.0f;
            float g = static_cast<float>(*(currDataPointer + 1)) / 255.0f;
            float b = static_cast<float>(*(currDataPointer + 2)) / 255.0f;
            surf.m_pixels[i] = Color{r, g, b};
        }
    }
    else if (nChannels == 4)
    {
        for (size_t i = 0; i < nPixels; i++, currDataPointer += nChannels)
        {
            float r = static_cast<float>(*currDataPointer) / 255.0f;
            float g = static_cast<float>(*(currDataPointer + 1)) / 255.0f;
            float b = static_cast<float>(*(currDataPointer + 2)) / 255.0f;
            float a = static_cast<float>(*(currDataPointer + 3)) / 255.0f;
            surf.m_pixels[i] = Color{r, g, b, a};
        }
    }
    stbi_image_free(imageRawData);
    return surf;
}

void Surface::Clear(const Color &fillValue) noexcept
{
    m_pixels.clear();
}

const Color &Surface::GetPixel(unsigned int x, unsigned int y) const noxnd
{
    return m_pixels[x + y * m_width];
}

unsigned int Surface::GetWidth() const noexcept
{
    return m_width;
}

unsigned int Surface::GetHeight() const noexcept
{
    return m_height;
}

const std::vector<Color> &Surface::GetPixels() noexcept
{
    return m_pixels;
}

Color *Surface::GetBufferPtr() noexcept
{
    return m_pixels.data();
}

const Color *Surface::GetBufferPtr() const noexcept
{
    return m_pixels.data();
}

const std::string &Surface::GetFilepath() const noexcept
{
    return m_filepath;
}

void Surface::SetPixel(unsigned int x, unsigned int y, const Color &c) noxnd
{
    m_pixels[x + y * m_width] = c;
}

Surface::Exception::Exception(int line, const char *file, const char *errorString) noexcept
        : IException(line, file),
          errorString(errorString)
{
}

const char *Surface::Exception::what() const noexcept
{
    std::ostringstream oss;
    oss << "[Type] " << GetType() << std::endl
        << "[Description] " << GetErrorString() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char *Surface::Exception::GetType() const noexcept
{
    return "Saffron Window Exception";
}

const char *Surface::Exception::GetErrorString() const noexcept
{
    return errorString.c_str();
}