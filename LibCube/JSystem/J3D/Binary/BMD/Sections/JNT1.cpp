#define _USE_MATH_DEFINES
#include <cmath>

#include <LibCube/JSystem/J3D/Binary/BMD/Sections.hpp>

namespace libcube::jsystem {

void readJNT1(BMDOutputContext& ctx)
{
    auto& reader = ctx.reader;
	if (!enterSection(ctx, 'JNT1')) return;
	
	ScopedSection g(ctx.reader, "Joints");

    u16 size = reader.read<u16>();
    ctx.mdl.mJoints.resize(size);
    ctx.jointIdLut.resize(size);
    reader.read<u16>();

    const auto [ofsJointData, ofsRemapTable, ofsStringTable] = reader.readX<s32, 3>();
    reader.seekSet(g.start);

    // Compressible resources in J3D have a relocation table (necessary for interop with other animations that access by index)
    // FIXME: Note and support saving identically
    reader.seekSet(g.start + ofsRemapTable);

    bool sorted = true;
    for (int i = 0; i < size; ++i)
    {
        ctx.jointIdLut[i] = reader.read<u16>();

        if (ctx.jointIdLut[i] != i)
            sorted = false;
    }

    if (!sorted)
        DebugReport("Joint IDS are remapped.\n");


    // FIXME: unnecessary allocation of a vector.
    reader.seekSet(ofsStringTable + g.start);
    const auto nameTable = readNameTable(reader);

    for (int i = 0; i < size; ++i)
    {
        auto& joint = ctx.mdl.mJoints[i];
        reader.seekSet(g.start + ofsJointData + ctx.jointIdLut[i] * 0x40);
        joint.id = ctx.jointIdLut[i]; // TODO
        joint.name = nameTable[i];
        const u16 flag = reader.read<u16>();
        joint.flag = flag & 0xf;
        joint.bbMtxType = static_cast<Joint::MatrixType>(flag >> 4);
        const u8 mayaSSC = reader.read<u8>();
        joint.mayaSSC = mayaSSC == 0xff ? false : mayaSSC;
        reader.read<u8>();
        joint.scale << reader;
        joint.rotate.x = static_cast<f32>(reader.read<s16>()) / f32(0x7ff) * M_PI;
        joint.rotate.y = static_cast<f32>(reader.read<s16>()) / f32(0x7ff) * M_PI;
        joint.rotate.z = static_cast<f32>(reader.read<s16>()) / f32(0x7ff) * M_PI;
        reader.read<u16>();
        joint.translate << reader;
        joint.boundingSphereRadius = reader.read<f32>();
        joint.boundingBox << reader;
    }
}

struct JNT1Node final : public oishii::v2::Node
{
	JNT1Node(const J3DModel& model)
		: mModel(model)
	{
		mId = "JNT1";
		mLinkingRestriction.alignment = 32;
	}
	Result gatherChildren(NodeDelegate& d)
	{
		//d.addNode();
		return {};
	}

	Result write(oishii::v2::Writer& writer) const noexcept
	{
		writer.write<u32, oishii::EndianSelect::Big>('JNT1');
		writer.writeLink<s32>({ *this }, { "DRW1" });

		writer.write<u16>(mModel.mJoints.size());
		writer.write<u16>(-1);

		//	writer.writeLink<s32>(
		//		oishii::v2::Hook(*this),
		//		oishii::v2::Hook("MatrixSizeTable"));
		//	writer.writeLink<s32>(
		//		oishii::v2::Hook(getSelf()),
		//		oishii::v2::Hook("MatrixIndexTable"));
		//	writer.writeLink<s32>(
		//		oishii::v2::Hook(getSelf()),
		//		oishii::v2::Hook("MatrixWeightTable"));
		//	writer.writeLink<s32>(
		//		oishii::v2::Hook(getSelf()),
		//		oishii::v2::Hook("MatrixInvBindTable"));

		return eResult::Success;
	}

private:
	const J3DModel& mModel;
};

std::unique_ptr<oishii::v2::Node> makeJNT1Node(BMDExportContext& ctx)
{
	return std::make_unique<JNT1Node>(ctx.mdl);
}


}