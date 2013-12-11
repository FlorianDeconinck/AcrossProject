//-----------------------------------------------------------------------------
//Project
#include "Translater.h"
//Tools
#include <Vec3D.h>
//DevIL
#include <IL/il.h>
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
		fwrite((void*)&FaceCount, sizeof(FaceCount), 1, pFileEngine);
		for(AT::U32 iFace = 0 ; iFace < Mesh.mNumFaces ; ++iFace){
			//nNumIndices must be 3 or triangulation is OFF !
			assert(Mesh.mFaces[iFace].mNumIndices == 3);
			fwrite(Mesh.mFaces[iFace].mIndices, Mesh.mFaces[iFace].mNumIndices*sizeof(Mesh.mFaces[iFace].mIndices[0]), 1, pFileEngine);
		}
	}
	//------------------------------------------------------------
	AT::I8 TRANSLATER_3D_SCENE::TranslateWithAssImp(ASSET_TYPE AssetType, const AT::I8* FilenameToImport, const TEXTURE_PARAMETERS& TextureParameters, const AT::I8* FilenameToWrite){
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
		//--
		//Asset type
		fwrite((void*)&AssetType, sizeof(ASSET_TYPE), 1, pFileEngine);
		//--
		//Write down vertex & indices
		if(scene->HasMeshes()){
			assimpToInternal_BoudingBoxComputation(pFileEngine, *scene);
			fwrite((void*)&scene->mNumMeshes, sizeof(scene->mNumMeshes), 1, pFileEngine);
			for(AT::U32 iMesh = 0 ; iMesh < scene->mNumMeshes ; ++iMesh){
				aiMesh& Mesh = *scene->mMeshes[iMesh];
				if(!Mesh.HasFaces())
					continue;
				assimpToInternal_MeshVertices(pFileEngine, Mesh);
				assimpToInternal_MeshIndices(pFileEngine, Mesh);
			}
		}
		//--
		//Write down texture
		if(TextureParameters.FilenameToImport){
			size_t len = strlen(TextureParameters.FilenameToImport);
			fwrite((void*)&len, sizeof(size_t), 1, pFileEngine);
			fwrite((void*)TextureParameters.FilenameToImport, sizeof(AT::I8), len, pFileEngine);
			fwrite((void*)&TextureParameters.UVOffset.x, sizeof(AT::I32F), 1, pFileEngine);
			fwrite((void*)&TextureParameters.UVOffset.y, sizeof(AT::I32F), 1, pFileEngine);
		}else{
			size_t len = -1;
			fwrite((void*)&len, sizeof(size_t), 1, pFileEngine);
		}
		//--
		//Write last zero
		AT::I8 Zero = 0;
		fwrite((void*)&Zero, sizeof(AT::I8), 1, pFileEngine);
		//--
		aiReleaseImport(scene);
		fclose(pFileEngine);
		return true;
	}
	//-----------------------------------------------------------------------------
	// TRANSLATER 2D SPRITE
	//-----------------------------------------------------------------------------
	AT::I8 TRANSLATER_2D_SPRITE::Translate2DSprite(ASSET_TYPE AssetType, AT::I32F Scale/*Meters per 100 pixel*/, const AT::I8* SpriteFilename, const AT::I8* FilenameToWrite){
		FILE* pFileEngine = fopen(FilenameToWrite, "wb");
		if(!pFileEngine)
			return false;
		//--
		AT::U32 W=0;
		AT::U32	H=0;
		ILuint ilTexid;
		ilGenImages(1, &ilTexid); 
		ilBindImage(ilTexid); 
		if(ilLoadImage(SpriteFilename) && ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)){
			W = ilGetInteger(IL_IMAGE_WIDTH);
			H = ilGetInteger(IL_IMAGE_HEIGHT);
		}
		ilDeleteImage(ilTexid);
		AT::I32F fHalfWidth = ((W/2)/100.f)*Scale;
		AT::I32F fHeight = (H/100.f)*Scale;
		//--
		//Asset type
		fwrite((void*)&AssetType, sizeof(ASSET_TYPE), 1, pFileEngine);
		//--
		//Write down BBox
		aiVector3D min, max;
		min.x = -fHalfWidth; min.y = 0.f;		min.z = 0.f;
		max.x =  fHalfWidth; max.y = fHeight;	max.z = 0.f;
		fwrite(&min, sizeof(min.x), 3, pFileEngine);
		fwrite(&max, sizeof(max.x), 3, pFileEngine);
		//--
		//Write down vertex & indices
		AT::I32 MeshCount = 1;
		fwrite((void*)&MeshCount, sizeof(MeshCount), 1, pFileEngine);
		//--
		AT::I32 nUV = 1;
		fwrite((void*)&nUV, sizeof(nUV), 1, pFileEngine);
		//--
		AT::U32 mNumVertices = 4;
		fwrite((void*)&mNumVertices, sizeof(mNumVertices), 1, pFileEngine);
		//--
		AT::I32F vertex[4][5];
		vertex[0][0] = -fHalfWidth; vertex[0][1] = 0.f;		vertex[0][2] = 0.f; vertex[0][3] = 1.f; vertex[0][4] = 1.f;
		vertex[1][0] = -fHalfWidth; vertex[1][1] = fHeight; vertex[1][2] = 0.f; vertex[1][3] = 1.f; vertex[1][4] = 0.f;
		vertex[2][0] =  fHalfWidth; vertex[2][1] = fHeight; vertex[2][2] = 0.f; vertex[2][3] = 0.f; vertex[2][4] = 0.f;
		vertex[3][0] =  fHalfWidth; vertex[3][1] = 0.f;		vertex[3][2] = 0.f; vertex[3][3] = 0.f; vertex[3][4] = 1.f;
		fwrite((void*)&vertex, sizeof(vertex), 1, pFileEngine);
		//--
		AT::I32 IndicesCount = 6;
		fwrite((void*)&IndicesCount, sizeof(IndicesCount), 1, pFileEngine);
		//--
		AT::U32 Indices[6];
		Indices[0] = 0; Indices[1] = 1; Indices[2] = 2;
		Indices[3] = 2; Indices[4] = 3; Indices[5] = 0;
		fwrite((void*)&Indices, sizeof(Indices), 1, pFileEngine);
		//--
		//Write down texture
		size_t len = strlen(SpriteFilename);
		fwrite((void*)&len, sizeof(size_t), 1, pFileEngine);
		fwrite((void*)SpriteFilename, sizeof(AT::I8), len, pFileEngine);
		AT::VEC2Df UVOffset(0,0);
		fwrite((void*)&UVOffset.x, sizeof(AT::I32F), 1, pFileEngine);
		fwrite((void*)&UVOffset.y, sizeof(AT::I32F), 1, pFileEngine);
		//--
		//Write last zero
		AT::I8 Zero = 0;
		fwrite((void*)&Zero, sizeof(AT::I8), 1, pFileEngine);
		//--
		fclose(pFileEngine);
		return true;
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------