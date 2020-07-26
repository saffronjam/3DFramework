#pragma once

#include "Bindable.h"
#include "Surface.h"

namespace Bind
{
    class Texture : public Bindable
    {
    public:
        using Ptr = std::shared_ptr<Texture>;

    public:
        explicit Texture(const Surface &surface);
        explicit Texture(const std::string &filepath);

        void BindTo(Graphics &gfx) override;
        static Texture::Ptr Resolve(const Surface &surface);
        static Texture::Ptr Resolve(const std::string &filepath);

        template<typename... Ignore>
        [[nodiscard]] static std::string GenerateUID(const Surface &surface, Ignore &&...ignore);
        template<typename... Ignore>
        [[nodiscard]] static std::string GenerateUID(const std::string &filepath, Ignore &&...ignore);
        [[nodiscard]] std::string GetUID() const noexcept override;

    private:
        std::string m_filepath;
        unsigned int m_textureID;
    };

    template<typename... Ignore>
    std::string Bind::Texture::GenerateUID(const Surface &surface, Ignore &&... ignore)
    {
        static size_t UIDNoFilepathSurfaces = 0;
        using namespace std::string_literals;
        return typeid(Texture).name() + "#"s + surface.GetFilepath();
    }

    template<typename... Ignore>
    std::string Bind::Texture::GenerateUID(const std::string &filepath, Ignore &&... ignore)
    {
        using namespace std::string_literals;
        return typeid(Texture).name() + "#"s + filepath;
    }
}