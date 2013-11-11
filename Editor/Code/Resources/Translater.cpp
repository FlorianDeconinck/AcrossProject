//-----------------------------------------------------------------------------
//Project
#include "Translater.h"
//Tools
#include <Vec3D.h>
//STD
#include <assert.h>
#include <minmax.h>
//--
#pragma warning( disable : 4996 )
//-----------------------------------------------------------------------------
namespace AE{
	//------------------------------------------------------------
	// TRANSLATER_3D_SCENE
	//------------------------------------------------------------
	void assimpToInternal_BoudingBoxComputation_Internal(const aiScene& scene, const aiNode* node, aiVector3D& min, aiVector3D& max, aiMatrix4x4& trf_model){
		aiMultiplyMatrix4(&trf_model, &node->mTransformation);
		//--
		for(AT::U32 iMesh = 0 ; iMesh < scene.mNumMeshes ; ++iMesh){
			const aiMesh& Mesh = *scene.mMeshes[iMesh];
			for(AT::U32 iVert = 0 ; iVert < Mesh.mNumVertices ; ++iVert){
				aiVector3D vertex = Mesh.mVertices[iVert];
				aiTransformVecByMatrix4(&vertex, &trf_model);
				//--
				min.x = min(min.x, vertex.x);
				min.y = min(min.y, vertex.y);
				min.z = min(min.z, vertex.z);
				//--
				max.x = max(max.x, vertex.x);
				max.y = max(max.y, vertex.y);
				max.z = max(max.z, vertex.z);
			}
		}
		//--
// 		for(AT::U32 iChildNode = 0 ; iChildNode < node->mNumChildren ; ++iChildNode)
// 			assimpToInternal_BoudingBoxComputation_Internal(scene, node->mChildren[iChildNode], min, max, trf_model);
	}
	//------------------------------------------------------------
	//Compute recursively bounding boxes of meshes contained within the DAE to get
	//the global bounding box
	void assimpToInternal_BoudingBoxComputation(FILE* pFileEngine, const aiScene& scene){
		aiMatrix4x4 trf_model;
		aiIdentityMatrix4(&trf_model);
		//--
		aiVector3D min, max;
		min.x = min.y = min.z = std::numeric_limits<AT::I32F>::infinity();
		max.x = max.y = max.z = -std::numeric_limits<AT::I32F>::infinity();
		//--
		assimpToInternal_BoudingBoxComputation_Internal(scene, scene.mRootNode, min, max, trf_model);
		//--
		//Write down bounding box
		fwrite(&min, sizeof(min.x), 3, pFileEngine);
		fwrite(&max, sizeof(max.x), 3, pFileEngine);
	}
	//------------------------------------------------------------
	void assimpToInternal_MeshVertices(FILE* pFileEngine, const aiMesh& Mesh){
		//--
		AT::I32 nUV = Mesh.GetNumUVChannels();
		fwrite((void*)&nUV, sizeof(nUV), 1, pFileEngine);
		//--
		fwrite((void*)&Mesh.mNumVertices, sizeof(Mesh.mNumVertices), 1, pFileEngine);
		//--
		if(nUV==1){
			for(AT::U32 iVert = 0 ; iVert < Mesh.mNumVertices ; ++iVert){
				fwrite((void*)&Mesh.mVertices[iVert], 3*sizeof(Mesh.mVertices[0].x), 1, pFileEngine);
				fwrite((void*)&Mesh.mTextureCoords[nUV-1][iVert].x, sizeof(Mesh.mTextureCoords[nUV-1][0].x), 1, pFileEngine);
				AT::I32F y = 1-Mesh.mTextureCoords[nUV-1][iVert].y;
				fwrite((void*)&y, sizeof(Mesh.mTextureCoords[nUV-1][0].x), 1, pFileEngine);
			}
		}else if(nUV!=0){
			assert(false);
		}else{
			//--
			AT::I32F Color[]={0.5f, 0.5f, 0.5f, 0.8f};
			for(AT::U32 iVert = 0 ; iVert < Mesh.mNumVertices ; ++iVert){
				fwrite((void*)&Mesh.mVertices[iVert], 3*sizeof(Mesh.mVertices[0].x), 1, pFileEngine);
				fwrite(Color, 4*sizeof(Color[0]), 1, pFileEngine);
			}
		}
	}
	//------------------------------------------------------------
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
			assimpToInternal_BoudingBoxComputation(pFileEngine, *scene);
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