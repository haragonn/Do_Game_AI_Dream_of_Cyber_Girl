/*==============================================================================
	[Scene.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SCENE_H
#define INCLUDE_IDEA_SCENE_H

#include "GameObject.h"
#include "../Framework/GameManager.h"
#include "../Framework/Fade.h"
#include "../Storage/Memory.h"
#include "../Storage/Storage.h"

#include <list>
#include <vector>
#include <typeinfo>

namespace idea{
	enum class GameObjectLayer{
		LAYER0,
		LAYER1,
		LAYER2,
		LAYER3,
		LAYER4,
		LAYER5,
		LAYER6,
		LAYER7
	};

	//------------------------------------------------------------------------------
	// クラス名　：Scene
	// クラス概要：ゲームシーンクラスの基底となるクラス
	//------------------------------------------------------------------------------
	class Scene{
	public:
		Scene() :
			bAsync_(false){}

		virtual ~Scene() = default;

		// 初期化
		virtual void Init() = 0;

		// 終了処理
		virtual void UnInit()
		{
			for(int i = 0; i < 8; ++i){
				for(GameObject* object : vecGameObject_[i]){
					if(object == nullptr){
						continue;
					}

					object->UnInit();

					SafeDelete(object);
				}

				vecGameObject_[i].clear();
			}
		}

		// 更新
		virtual void Update()
		{
			for(int i = 0; i < 8; ++i){
				for(GameObject* object : vecGameObject_[i]){
					if(object == nullptr){
						continue;
					}

					object->Update();
				}

				vecGameObject_[i].remove_if(
					[](GameObject* object)
				{
					return object->Destroy(); 
				});
			}
		}

		// 描画
		virtual void Draw()
		{
			for(int i = 0; i < 8; ++i){
				for(GameObject* object : vecGameObject_[i]){
					if(object == nullptr){
						continue;
					}

					object->Draw();
				}
			}
		}

		// ゲームオブジェクトの追加
		template<typename T>
		T* AddGameObject(GameObjectLayer layer)
		{
			T* gameObject = new T();
			Assert(gameObject);

			int layerNum = 0;

			switch(layer){
				case GameObjectLayer::LAYER0:
					layerNum = 0;
					break;

				case GameObjectLayer::LAYER1:
					layerNum = 1;
					break;

				case GameObjectLayer::LAYER2:
					layerNum = 2;
					break;

				case GameObjectLayer::LAYER3:
					layerNum = 3;
					break;

				case GameObjectLayer::LAYER4:
					layerNum = 4;
					break;

				case GameObjectLayer::LAYER5:
					layerNum = 5;
					break;

				case GameObjectLayer::LAYER6:
					layerNum = 6;
					break;

				case GameObjectLayer::LAYER7:
					layerNum = 7;
					break;
			}

			vecGameObject_[layerNum].push_back(gameObject);

			gameObject->Init();

			return gameObject;
		}

		// ゲームオブジェクトの取得
		template<typename T>
		T* GetGameObject(GameObjectLayer layer)
		{
			int layerNum = 0;

			switch(layer){
				case GameObjectLayer::LAYER0:
					layerNum = 0;
					break;

				case GameObjectLayer::LAYER1:
					layerNum = 1;
					break;

				case GameObjectLayer::LAYER2:
					layerNum = 2;
					break;

				case GameObjectLayer::LAYER3:
					layerNum = 3;
					break;

				case GameObjectLayer::LAYER4:
					layerNum = 4;
					break;

				case GameObjectLayer::LAYER5:
					layerNum = 5;
					break;

				case GameObjectLayer::LAYER6:
					layerNum = 6;
					break;

				case GameObjectLayer::LAYER7:
					layerNum = 7;
					break;
			}

			for(GameObject* object : vecGameObject_[layerNum]){
				if(typeid(*object) == typeid(T)){
					return (T*)object;
				}
			}

			return nullptr;
		}

		// ゲームオブジェクトリストの取得
		template<typename T>
		std::vector<T*> GetGameObjects(GameObjectLayer layer)
		{
			int layerNum = 0;

			switch(layer){
				case GameObjectLayer::LAYER0:
					layerNum = 0;
					break;

				case GameObjectLayer::LAYER1:
					layerNum = 1;
					break;

				case GameObjectLayer::LAYER2:
					layerNum = 2;
					break;

				case GameObjectLayer::LAYER3:
					layerNum = 3;
					break;

				case GameObjectLayer::LAYER4:
					layerNum = 4;
					break;

				case GameObjectLayer::LAYER5:
					layerNum = 5;
					break;

				case GameObjectLayer::LAYER6:
					layerNum = 6;
					break;

				case GameObjectLayer::LAYER7:
					layerNum = 7;
					break;
			}

			std::vector<T*> objects;
			objects.reserve(vecGameObject_[layerNum].size());

			for(GameObject* object : vecGameObject_[layerNum]){
				if(typeid(*object) == typeid(T)){
					objects.push_back((T*)object);
				}
			}

			return objects;
		}

		// ゲームオブジェクトが生存しているか
		template<typename T>
		bool IsAliveGameObject(T*& pGameObject, GameObjectLayer layer)
		{
			if(!pGameObject){
				return false;
			}

			int layerNum = 0;

			switch(layer){
				case GameObjectLayer::LAYER0:
					layerNum = 0;
					break;

				case GameObjectLayer::LAYER1:
					layerNum = 1;
					break;

				case GameObjectLayer::LAYER2:
					layerNum = 2;
					break;

				case GameObjectLayer::LAYER3:
					layerNum = 3;
					break;

				case GameObjectLayer::LAYER4:
					layerNum = 4;
					break;

				case GameObjectLayer::LAYER5:
					layerNum = 5;
					break;

				case GameObjectLayer::LAYER6:
					layerNum = 6;
					break;

				case GameObjectLayer::LAYER7:
					layerNum = 7;
					break;
			}

			for(GameObject* object : vecGameObject_[layerNum]){
				if(typeid(*object) == typeid(T)){
					if(object == pGameObject && !object->bDestroy_){
						return true;
					}
				}
			}

			pGameObject = nullptr;

			return false;
		}

		// ゲームオブジェクトの数の取得
		template<typename T>
		int GetGameObjectsNum(GameObjectLayer layer)
		{
			int layerNum = 0;

			switch(layer){
				case GameObjectLayer::LAYER0:
					layerNum = 0;
					break;

				case GameObjectLayer::LAYER1:
					layerNum = 1;
					break;

				case GameObjectLayer::LAYER2:
					layerNum = 2;
					break;

				case GameObjectLayer::LAYER3:
					layerNum = 3;
					break;

				case GameObjectLayer::LAYER4:
					layerNum = 4;
					break;

				case GameObjectLayer::LAYER5:
					layerNum = 5;
					break;

				case GameObjectLayer::LAYER6:
					layerNum = 6;
					break;

				case GameObjectLayer::LAYER7:
					layerNum = 7;
					break;
			}

			int ret = 0;

			for(GameObject* object : vecGameObject_[layerNum]){
				if(typeid(*object) == typeid(T)){
					++ret;
				}
			}

			return ret;
		}

	protected:
		// ゲームオブジェクトリスト
		std::list<GameObject*> vecGameObject_[8];

		// ゲームマネージャー
		GameManager gameManager_;

		// フェード
		Fade fade_;

		// 数値保管庫
		Memory memory_;

		// リソース保管庫
		Storage storage_;

	private:
		friend class SequenceManager;
		friend class AsyncLoadScene;

		// 非同期読み込みフラグ
		bool bAsync_ = false;
	};
}
#endif	// #ifndef INCLUDE_IDEA_SCENE_H