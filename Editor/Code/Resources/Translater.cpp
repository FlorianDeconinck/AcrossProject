//-----------------------------------------------------------------------------
//Project
#include "Translater.h"
//Tools
#include <Vec3D.h>
#include <assert.h>
#pragma warning( disable : 4996 )
//-----------------------------------------------------------------------------
namespace AE{
	//------------------------------------------------------------
	// TRANSLATER_3D_SCENE
	//------------------------------------------------------------
	void assimpToInternal_MeshVertices(FILE* pFileEngine, const aiMesh& Mesh){
		fwrite((void*)&Mesh.mNumVertices, sizeof(Mesh.mNumVertices), 1, pFileEngine);
		//--
		AT::I32F Color[]={0.5f, 0.5f, 0.5f, 0.8f};
		for(AT::U32 iVert = 0 ; iVert < Mesh.mNumVertices ; ++iVert){
			fwrite((void*)&Mesh.mVertices[iVert], 3*sizeof(Mesh.mVertices[0].x), 1, pFileEngine);
			fwrite(Color, 4*sizeof(Color[0]), 1, pFileEngine);
		}
	}
	void assimpToInternal_MeshIndices(FILE* pFileEngine, const aiMesh& Mesh){
		AT::I32 FaceCount = Mesh.mNumFaces*Mesh.mFaces[0].mNumIndices;
		fwrite((void*)&FaceCount, sizeof(Mesh.mNumFaces), 1, pFileEngine);
		for(AT::U32 iFace = 0 ; iFace < Mesh.mNumFaces ; ++iFace){
			//nNumIndices must be 3 or triangulation is OFF !
			assert(Mesh.mFaces[iFace].mNumIndices == 3);
			fwrite(Mesh.mFaces[iFace].mIndices, Mesh.mFaces[iFace].mNumIndices*sizeof(Mesh.mFaces[iFace].mIndices[0]), 1, pFileEngine);
		}
	}
	//------------------------------------------------------------
	AT::I8 TRANSLATER_3D_SCENE::TranslateWithAssImp(const AT::I8* FilenameToImport, const AT::I8* FilenameToWrite){
		const aiScene* scene = aiImportFile(FilenameToImport, 
			aiProcess_CalcTangentSpace       | 
			aiProcess_Triangulate            |
			aiProcess_JoinIdenticalVertices  |
			aiProcess_SortByPType);
		if(!scene)
			return false;
		FILE* pFileEngine = fopen(FilenameToWrite, "wb");
		if(!pFileEngine){
			aiReleaseImport(scene);
			return false;
		}
		//Write down Engine asset format
		if(scene->HasMeshes()){
			for(AT::U32 iMesh = 0 ; iMesh < scene->mNumMeshes ; ++iMesh){
				aiMesh& Mesh = *scene->mMeshes[iMesh];
				if(!Mesh.HasFaces())
					continue;
				assimpToInternal_MeshVertices(pFileEngine, Mesh);
				assimpToInternal_MeshIndices(pFileEngine, Mesh);
			}
		}
		//--
		aiReleaseImport(scene);
		fclose(pFileEngine);
		return true;
	}
}//namespace AE
//-----------------------------------------------------------------------------