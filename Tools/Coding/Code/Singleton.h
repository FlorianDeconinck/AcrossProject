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
			if (!_singleton)
				_singleton = new T;
			return (static_cast<T*> (_singleton));
		}
		//------------------------------------------------------------------------
		static void kill(){
			if (_singleton){
				delete _singleton;
				_singleton = 0;
			}
		}
		//------------------------------------------------------------------------
	private:
		static T *_singleton;
	};
	//-----------------------------------------------------------------------------
	template <typename T> T *SINGLETON<T>::_singleton = 0;
	//-----------------------------------------------------------------------------
}//namespace AT
//-----------------------------------------------------------------------------