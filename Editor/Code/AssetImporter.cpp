//-----------------------------------------------------------------------------
//Project
//-----------------------------------------------------------------------------
//Project
#include "AssetImporter.h"
#include "./Resources/Translater.h"
//Tools
#include <imgui.h>
//Windows
#include <minwinbase.h>
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	ASSET_IMPORTER::ASSET_IMPORTER():m_bImport(true),m_ImportStep(NONE),m_AssetType(ASSET_UNKNOWN_TYPE),m_pImportedObject(NULL){
		m_ID = IMPORT_ASSET_MODE;
		sprintf_s(m_ModuleName, "Asset Importer");
		memset(m_CurrentModelFilename, 0, sizeof(m_CurrentModelFilename));
		memset(m_CurrentModelTextureFilename, 0, sizeof(m_CurrentModelTextureFilename));
	}
	//-----------------------------------------------------------------------------
	ASSET_IMPORTER::~ASSET_IMPORTER(){

	}
	//-----------------------------------------------------------------------------
	ENGINE_API_ENTRYPOINTS::API_MSG	ASSET_IMPORTER::Update(AE::ENGINE& Engine, AE::WORLD& World){ 
		if(m_ImportStep== NONE)
			return ENGINE_API_ENTRYPOINTS::NO_MSG;
		switch(m_ImportStep){
		case LOADING:
			break;
		case TRANSLATING:{
			if(!strlen(m_CurrentModelFilename)){
				m_ImportStep=LOADING;
				break;
			}
			m_ImportStep = TRANSLATING;
			char FilenameToWrite[128];
			memset(FilenameToWrite, 0, 128*sizeof(char));
			char* pPoint = strstr(m_CurrentModelFilename, ".");
			strncpy(FilenameToWrite, m_CurrentModelFilename, size_t(pPoint-m_CurrentModelFilename));
			strcat(FilenameToWrite, ".aeasset");
			TRANSLATER_3D_SCENE::TEXTURE_PARAMETERS TParameters;
			TParameters.FilenameToImport =  m_CurrentModelTextureFilename;
			TParameters.UVOffset.Set(0, 0);
			TRANSLATER_3D_SCENE::TranslateWithAssImp(m_AssetType, m_CurrentModelFilename, TParameters, FilenameToWrite);
			Engine.AddResourceToDB(FilenameToWrite);
			m_pImportedObject = World.SpawnNPC(FilenameToWrite, AT::VEC2Di(64, 64));
			if(!m_pImportedObject)//problem with the translation
				m_ImportStep = IMPORT_FAILED;
			//--
			m_ImportStep = PARAMETERS_TWEAKING;
			break;
		}
		case PARAMETERS_TWEAKING:
			break;
		case FINAL_SAVING:{
			//--
			char FilenameToWrite[128];
			memset(FilenameToWrite, 0, 128*sizeof(char));
			char* pPoint = strstr(m_CurrentModelFilename, ".");
			strncpy(FilenameToWrite, m_CurrentModelFilename, size_t(pPoint-m_CurrentModelFilename));
			strcat(FilenameToWrite, ".aeasset");
			TRANSLATER_3D_SCENE::TEXTURE_PARAMETERS TParameters;
			TParameters.FilenameToImport =  m_CurrentModelTextureFilename;
			AT::VEC2Df uvOffset = m_pImportedObject->GetUVOffset(0);
			TParameters.UVOffset.Set(uvOffset.x, uvOffset.y);
			TRANSLATER_3D_SCENE::TranslateWithAssImp(m_AssetType, m_CurrentModelFilename, TParameters, FilenameToWrite);
			//--
			World.ClearNPC();
			m_CurrentModelFilename[0]='\0';
			m_CurrentModelTextureFilename[0]='\0';
			m_pImportedObject = NULL;
			m_AssetType=ASSET_UNKNOWN_TYPE;
			m_ImportStep=NONE;
			break;
		}
		case IMPORT_FAILED:
		case NONE:
		default:
			break;
		}
		return ENGINE_API_ENTRYPOINTS::NO_MSG; 
	}
	//-----------------------------------------------------------------------------
	void ASSET_IMPORTER::UpdateGUI(const AE::ENGINE& Engine){
		if(m_ImportStep== NONE)
			return;
		switch(m_ImportStep){
			case LOADING:
				LoadMesh_Wdw(Engine);
				break;
			case TRANSLATING:{
				AT::I32 NavBarScrollArea=1;
				imguiBeginScrollArea("Translating data into engine format",  RENDERER_ABC::WIDTH/2 - 50, RENDERER_ABC::HEIGHT/2 - 25 , 100, 50 , &NavBarScrollArea);
				imguiEndScrollArea();
				break;
			}
			case IMPORT_FAILED:{
				AT::I32 NavBarScrollArea=1;
				imguiBeginScrollArea("Translation failed",  RENDERER_ABC::WIDTH/2 - 50, RENDERER_ABC::HEIGHT/2 - 50 , 100, 100 , &NavBarScrollArea);
				bool toggle = imguiButton("Ok");
				if(toggle)
					m_ImportStep=NONE;
				imguiEndScrollArea();
				break;
			}
			case PARAMETERS_TWEAKING:
			case FINAL_SAVING:
			case NONE:
			default:
				break;
		}
	}
	//-----------------------------------------------------------------------------
	void ASSET_IMPORTER::UpdateNavBarGUI(){
		imguiIndent();
		m_bImport = imguiButton("Load/Translate model", m_ImportStep==NONE);
		if(m_bImport || m_ImportStep!=NONE){
			if(m_ImportStep==NONE)
				m_ImportStep = LOADING;
			switch(m_ImportStep){
				case PARAMETERS_TWEAKING:{
					if(!m_pImportedObject)
						break;
					AT::VEC2Df uvOffset = m_pImportedObject->GetUVOffset(0);
					imguiSlider("Offset UV : x", &uvOffset.x, -1.f, 1.f, 0.f);
					imguiSlider("Offset UV : y", &uvOffset.y, -1.f, 1.f, 0.f);
					m_pImportedObject->SetUVOffset(0, uvOffset.x, uvOffset.y);
					bool toggle = imguiButton("Save final model");
					if(toggle)
						m_ImportStep=FINAL_SAVING;
					break;
				}
				case NONE:
				case LOADING:
				case TRANSLATING:
				case FINAL_SAVING:
				default:
					break;
			}
		}
		imguiUnindent();
	}
	//-----------------------------------------------------------------------------
	inline void PopLoadDialog(char* szFileOut){
		char szFile[256];
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrTitle = "Open";
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
		ofn.nFilterIndex =1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0 ;
		ofn.lpstrInitialDir=NULL;
		ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
		GetOpenFileName(&ofn);
		strcpy(szFileOut, ofn.lpstrFile);
	}
	//-----------------------------------------------------------------------------
	void ASSET_IMPORTER::LoadMesh_Wdw(const AE::ENGINE& Engine){
		AT::I32 NavBarScrollArea=1;
		imguiBeginScrollArea("Load & translate model",  RENDERER_ABC::WIDTH/2 - 300, RENDERER_ABC::HEIGHT/2 - 125 , 600, 250 , &NavBarScrollArea);
		imguiSeparatorLine();
		//--
		char msg[256];
		bool toggle = imguiButton("Model path");
		if(toggle)
			PopLoadDialog(m_CurrentModelFilename);
		sprintf_s(msg, "Model : %s", m_CurrentModelFilename);
		imguiLabel(msg);
		toggle = imguiButton("Texture path");
		if(toggle)
			PopLoadDialog(m_CurrentModelTextureFilename);		
		sprintf_s(msg, "Texture : %s", m_CurrentModelTextureFilename);
		imguiLabel(msg);
		//--
		imguiSeparatorLine();
		m_AssetType = imguiCheck("3D Mesh", m_AssetType==ASSET_TYPE::ASSET_3D_MESH) ? ASSET_TYPE::ASSET_3D_MESH : m_AssetType;
		m_AssetType = imguiCheck("2D Billboard", m_AssetType==ASSET_TYPE::ASSET_2D_BILLBOARD) ? ASSET_TYPE::ASSET_2D_BILLBOARD : m_AssetType;
		//--
		imguiSeparatorLine();
		toggle = imguiButton("Translate model to engine format (write into \"ModelName\".aeasset)");
		if(toggle)
			m_ImportStep = TRANSLATING;
		//--
		imguiEndScrollArea();
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------