/*****************************************************************************************
 *                                                                                       *
 * OpenSpace                                                                             *
 *                                                                                       *
 * Copyright (c) 2014-2024                                                               *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this  *
 * software and associated documentation files (the "Software"), to deal in the Software *
 * without restriction, including without limitation the rights to use, copy, modify,    *
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    *
 * permit persons to whom the Software is furnished to do so, subject to the following   *
 * conditions:                                                                           *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all copies *
 * or substantial portions of the Software.                                              *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   *
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         *
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    *
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  *
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE  *
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                         *
 ****************************************************************************************/

#include <modules/base/rendering/screenspaceimageonline.h>

#include <openspace/documentation/documentation.h>
#include <openspace/documentation/verifier.h>
#include <openspace/engine/globals.h>
#include <openspace/engine/downloadmanager.h>
#include <ghoul/filesystem/filesystem.h>
#include <ghoul/io/texture/texturereader.h>
#include <ghoul/logging/logmanager.h>
#include <ghoul/opengl/texture.h>
#include <ghoul/opengl/programobject.h>
#include <optional>

namespace {
    constexpr openspace::properties::Property::PropertyInfo TextureInfo = {
        "URL",
        "Image URL",
        "Sets the URL of the texture that is displayed on this screen space plane. If "
        "this value is changed, the image at the new path will automatically be loaded "
        "and displayed. The size of the image will also automatically set the default "
        "size of this plane",
        // @VISIBILITY(2.4)
        openspace::properties::Property::Visibility::User
    };

    struct [[codegen::Dictionary(ScreenSpaceImageOnline)]] Parameters {
        // Specifies the GUI name of the ScreenspaceImage
        std::optional<std::string> name;

        // [[codegen::verbatim(TextureInfo.description)]]
        std::optional<std::string> url [[codegen::key("URL")]];
    };
#include "screenspaceimageonline_codegen.cpp"
} // namespace

namespace openspace {

documentation::Documentation ScreenSpaceImageOnline::Documentation() {
    return codegen::doc<Parameters>("base_screenspace_image_online");
}

ScreenSpaceImageOnline::ScreenSpaceImageOnline(const ghoul::Dictionary& dictionary)
    : ScreenSpaceRenderable(dictionary)
    , _textureIsDirty(false)
    , _texturePath(TextureInfo)
{
    const Parameters p = codegen::bake<Parameters>(dictionary);

    std::string identifier;
    if (dictionary.hasValue<std::string>(KeyIdentifier)) {
        identifier = dictionary.value<std::string>(KeyIdentifier);
    }
    else {
        identifier = "ScreenSpaceImageOnline";
    }
    identifier = makeUniqueIdentifier(identifier);
    setIdentifier(std::move(identifier));

    _texturePath.onChange([this]() { _textureIsDirty = true; });
    _texturePath = p.url.value_or(_texturePath);
    addProperty(_texturePath);
}

ScreenSpaceImageOnline::~ScreenSpaceImageOnline() {}

bool ScreenSpaceImageOnline::deinitializeGL() {
    _texture = nullptr;

    return ScreenSpaceRenderable::deinitializeGL();
}

void ScreenSpaceImageOnline::update() {
    if (_textureIsDirty) {
        if (!_imageFuture.valid()) {
            std::future<DownloadManager::MemoryFile> future = downloadImageToMemory(
                _texturePath
            );
            if (future.valid()) {
                _imageFuture = std::move(future);
            }
        }

        if (_imageFuture.valid() && DownloadManager::futureReady(_imageFuture)) {
            DownloadManager::MemoryFile imageFile = _imageFuture.get();

            if (imageFile.corrupted) {
                LERRORC(
                    "ScreenSpaceImageOnline",
                    fmt::format("Error loading image from URL '{}'", _texturePath.value())
                );
                return;
            }

            try {
                std::unique_ptr<ghoul::opengl::Texture> texture =
                    ghoul::io::TextureReader::ref().loadTexture(
                        reinterpret_cast<void*>(imageFile.buffer),
                        imageFile.size,
                        2,
                        imageFile.format
                    );

                if (texture) {
                    // Images don't need to start on 4-byte boundaries, for example if the
                    // image is only RGB
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

                    if (texture->format() == ghoul::opengl::Texture::Format::Red) {
                        texture->setSwizzleMask({ GL_RED, GL_RED, GL_RED, GL_ONE });
                    }

                    texture->uploadTexture();
                    texture->setFilter(ghoul::opengl::Texture::FilterMode::LinearMipMap);
                    texture->purgeFromRAM();

                    _texture = std::move(texture);
                    _objectSize = _texture->dimensions();
                    _textureIsDirty = false;
                }
            }
            catch (const ghoul::io::TextureReader::InvalidLoadException& e) {
                _textureIsDirty = false;
                LERRORC(e.component, e.message);
            }
        }
    }
}

std::future<DownloadManager::MemoryFile> ScreenSpaceImageOnline::downloadImageToMemory(
                                                                   const std::string& url)
{
    return global::downloadManager->fetchFile(
        url,
        [url](const DownloadManager::MemoryFile&) {
            LDEBUGC(
                "ScreenSpaceImageOnline",
                "Download to memory finished for screen space image"
            );
        },
        [url](const std::string& err) {
            LDEBUGC(
                "ScreenSpaceImageOnline",
                "Download to memory failed for screen space image: " + err
            );
        }
    );
}

void ScreenSpaceImageOnline::bindTexture() {
    if (_texture) {
        _texture->bind();
    }
}

} // namespace openspace
