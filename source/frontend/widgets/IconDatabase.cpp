#include "IconManager.hpp"
#include <array>          // for std::array
#include <core/3d/gl.hpp> // for glGenTextures
#include <imgui/imgui.h>  // for ImGui::Image
#include <librii/image/ImagePlatform.hpp>

namespace riistudio {

IconDatabase::IconDatabase(u32 iconDimension) : mIconDim(iconDimension) {
  mIcons.reserve(256);
}
IconDatabase::~IconDatabase() {}
IconDatabase::Key IconDatabase::addIcon(lib3d::Texture& texture) {
  Key id = mIcons.size();
  mIcons.emplace_back(texture, mIconDim);
  return id;
}
void IconDatabase::drawIcon(Key id, int wd, int ht) const {
  ImGui::Image((void*)(intptr_t)mIcons[id].glId,
               ImVec2((wd > 0 ? wd : mIconDim), (ht > 0 ? ht : mIconDim)));
}

// TODO: Not threadsafe
static std::array<u8, 128 * 128 * 4> scratch;
static std::vector<u8> tmp;

IconDatabase::Icon::Icon(lib3d::Texture& texture, u32 dimension) {
  glGenTextures(1, &glId);

  glBindTexture(GL_TEXTURE_2D, glId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  assert(dimension <= 128);
  texture.decode(tmp, false);
  librii::image::resize(scratch.data(), dimension, dimension, tmp.data(),
                        texture.getWidth(), texture.getHeight(),
                        librii::image::Lanczos);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimension, dimension, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, (void*)scratch.data());
}
IconDatabase::Icon::~Icon() {
  if (glId != (u32)-1)
    glDeleteTextures(1, &glId);
}

} // namespace riistudio
