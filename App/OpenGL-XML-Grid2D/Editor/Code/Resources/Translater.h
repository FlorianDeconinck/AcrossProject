#pragma once
//---------------------------------------------------------------------------
//AssImp
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
//Tool
#include <AcrossTypes.h>
#include <Vec2D.h>
//Engine
#include <./ResourceManager/Asset_Types.h>
//---------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------
	class TRANSLATER_3D_SCENE{
	public :
		struct TEXTURE_PARAMETERS{
			AT::I8*			FilenameToImport;
			AT::VEC2Df		UVOffset;
		};
		static AT::I8 TranslateWithAssImp(ASSET_TYPE AssetType, const AT::I8* ModelFilenameToImport, const TEXTURE_PARAMETERS& TextureParameters, const AT::I8* FilenameToWrite);
	};
	//-----------------------------------------------------------------------
	class TRANSLATER_2D_SPRITE{
	public :
		static AT::I8 Translate2DSprite(ASSET_TYPE AssetType, AT::I32F Scale/*Meters per 100 pixel*/, const AT::I8* SpriteFilename, const AT::I8* FilenameToWrite);
	};
	//-----------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------