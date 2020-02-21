#include "export.hpp"

#include <LibCore/common.h>
#include <LibCore/api/Node.hpp>

#include <Lib3D/io/fbx.hpp>
#include <Lib3D/interface/i3dmodel.hpp>

namespace lib3d {

void install()
{
	assert(px::PackageInstaller::spInstance);
	px::PackageInstaller& installer = *px::PackageInstaller::spInstance;
#ifdef _WIN32
	installer.registerSerializer(std::make_unique<FbxIo>());
#endif
	installer.registerObject(Scene::TypeInfo);
	installer.registerParent<Scene, px::IDestructable>();
}

} // namespace lib3d
