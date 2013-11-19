#pragma once
//-----------------------------------------------------------------------------
namespace AT{
	template <typename T> class SINGLETON{
	protected:
		SINGLETON(){}
		~SINGLETON(){}
	public:
		//------------------------------------------------------------------------
		static T *getInstance(){
			if (NULL == _singleton)
				_singleton = new T;
			return (static_cast<T*> (_singleton));
		}
		//------------------------------------------------------------------------
		static void kill(){
			if (NULL != _singleton){
				delete _singleton;
				_singleton = NULL;
			}
		}
		//------------------------------------------------------------------------
	private:
		static T *_singleton;
	};
	//-----------------------------------------------------------------------------
	template <typename T> T *SINGLETON<T>::_singleton = NULL;
	//-----------------------------------------------------------------------------
}//namespace AT
//-----------------------------------------------------------------------------