#pragma once
//---------------------------------------------------------------------------
//AssImp
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
//Tool
#include <AcrossTypes.h>
//---------------------------------------------------------------------------
namespace AE{
	class TRANSLATER_3D_SCENE{
	public :
		static AT::I8 TranslateWithAssImp(const AT::I8* FilenameToImport, const AT::I8* FilenameToWrite);
	private :
	};
}//namespace AE
//---------------------------------------------------------------------------