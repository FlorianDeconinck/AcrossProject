#pragma once
//-----------------------------------------------------------------------------
//Project
#include "EditorModule.h"
//Tools
#include <Singleton.h>
//Engine
#include <./ResourceManager/Asset_Types.h>
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	class GAME_OBJECT;
	class WORLD_3DGRID;
	//-----------------------------------------------------------------------------
	class ASSET_IMPORTER:public AT::SINGLETON<ASSET_IMPORTER>,public EDITOR_MODULE_ABC{
		friend class AT::SINGLETON<ASSET_IMPORTER>;
	private :
		ASSET_IMPORTER();
		~ASSET_IMPORTER();
	public :
		//--
		/*virtual*/ ENGINE_API_ENTRYPOINTS::API_MSG	Update(AE::ENGINE& Engine, AE::WORLD_3DGRID& World);
		/*virtual*/ void							UpdateGUI(const AE::ENGINE& Engine);
		/*virtual*/ void							UpdateNavBarGUI();
		//--
	private :
		void					LoadMesh_Wdw(const AE::ENGINE& Engine);
		//--
		AT::I8					m_bImport;
		enum IMPORT_STEP{
			NONE=0,
			LOADING,
			TRANSLATING,
			PARAMETERS_TWEAKING,
			FINAL_SAVING,
			IMPORT_FAILED,
			//--
			IMPORT_STEP_COUNT
		};
		IMPORT_STEP				m_ImportStep;
		char					m_CurrentModelFilename[128];
		char					m_CurrentModelTextureFilename[128];
		ASSET_TYPE				m_AssetType;
		GAME_OBJECT*			m_pImportedObject;
		AT::I8					m_CurrentDirectory[256];
		//--
		void Load3DMesh(AE::ENGINE& Engine, AE::WORLD_3DGRID& World);
	};
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------