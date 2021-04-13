#ifndef INCLUDE_IDEA_STATEMACHINE_H
#define INCLUDE_IDEA_STATEMACHINE_H

#include "../Framework/GameObject.h"

#include <memory>
#include <unordered_map>

namespace idea{
	template<typename T>
	class State{
	public:
		State() = delete;
		explicit State(T id) :
			id_(id)
		{}

		virtual ~State() = default;

		// 初期化
		virtual void Init()
		{}

		// 終了処理
		virtual void UnInit()
		{}

		// 更新
		virtual void Update()
		{
			if(childState_){
				childState_->Update();
			}
		}

		// 状態IDを取得する
		const T& GetId()
		{
			return id_;
		}

		// 状態を追加する
		void AddState(const std::shared_ptr<State<T>>& state)
		{
			// 既に登録済みかチェック
			if(childStateList_.find(state->GetId()) == childStateList_.end()){
				childStateList_[state->GetId()] = state;
			}
		}

		// 指定の状態へ移行する
		void SetState(T nextStateId)
		{
			Assert(childStateList_.find(nextStateId) != childStateList_.end());	// 存在しない状態に移行しようとした

			if(childState_){
				childState_->UnInit();
				childState_ = childStateList_[nextStateId];
				childState_->Init();
			} else{
				childState_ = childStateList_[nextStateId];
				childState_->Init();
			}
		}

		// 現在の状態のIDを返す
		const T& GetState() const
		{
			Assert(childState_);	// NULLチェック

			return childState_->GetId();
		}

	private:
		// 状態ID
		T id_;

		/// 現在の状態
		std::shared_ptr<State<T>> childState_;

		// 状態リスト
		std::unordered_map<T, std::shared_ptr<State<T>>> childStateList_;
	};

	template<typename T>
	class StateMachine : public GameObject{
	public:
		StateMachine() :
			state_(nullptr)
		{}

		virtual ~StateMachine() = default;

		// 更新
		virtual void Update()override
		{
			if(state_){
				state_->Update();
			}
		}

		// 状態を追加する
		void AddState(const std::shared_ptr<State<T>>& state)
		{
			// 既に登録済みかチェック
			if(stateList_.find(state->GetId()) == stateList_.end()){
				stateList_[state->GetId()] = state;
			}
		}

		// 指定の状態へ移行する
		void SetState(T nextStateId)
		{
			Assert(stateList_.find(nextStateId) != stateList_.end());	// 存在しない状態に移行しようとした

			if(state_ != nullptr){
				state_->UnInit();
				state_ = stateList_[nextStateId];
				state_->Init();
			} else{
				state_ = stateList_[nextStateId];
				state_->Init();
			}
		}

		// 現在の状態のIDを返す
		const T& GetState() const
		{
			Assert(state_);	// NULLチェック

			return state_->GetId();
		}

		// 子を指定の状態へ移行する
		void SetChildState(T nextChildStateId)
		{
			Assert(stateList_.find(nextChildStateId) != stateList_.end());	// 存在しない状態に移行しようとした

			state_->SetState(nextChildStateId);
		}

		// 子の現在の状態のIDを返す
		const T& GetChildState() const
		{
			Assert(state_);	// NULLチェック

			return state_->GetState();
		}

	private:
		/// 現在の状態
		std::shared_ptr<State<T>> state_;

		// 状態リスト
		std::unordered_map<T, std::shared_ptr<State<T>>> stateList_;
	};

	/*----------------------------------------------
	// サンプル
	//----------------------------------------------
	// サンプルステート軍
	enum class SampleStates{
		State1,
		State2
	};

	// サンプルステートマシン
	class SampleMachine : public StateMachine<SampleStates>{
	public:
		// 初期化
		void Init() override
		{
			// TODO: 初期化処理

			// 状態を追加する
			AddState(std::make_shared<State1>(*this));
			AddState(std::make_shared<State2>(*this));

			// 初期状態を設定
			SetState(SampleStates::State1);
		}

		void Update()override
		{
			// TODO: 共通の前処理

			StateMachine::Update();

			// TODO: 共通の後処理
		}
		void Draw()override
		{
			// TODO: 描画処理
		}

		//----------------------------------------------
		// 状態をインナークラスで定義する
		//----------------------------------------------
		class State1 : public State<SampleStates>{
		public:
			State1() = delete;
			State1(SampleMachine& main) :
				State<SampleStates>(SampleStates::State1),
				main_(main)
			{}

			void Init() override
			{
				// TODO: 初期化処理
			}

			void Update() override
			{
				// TODO: 更新処理

				// 指定の状態へ移行する
				main_.SetState(SampleStates::State2);
			}

			void UnInit() override
			{
				// TODO: 終了処理
			}

		private:
			//StateMachineを取得しておくとアクセスできる
			SampleMachine& main_;
		};

		class State2 : public State<SampleStates>{
		public:
			State2() = delete;
			State2(SampleMachine& main) :
				State<SampleStates>(SampleStates::State2)
				, main_(main)
			{}

			void Init() override
			{
				// TODO: 初期化処理
			}

			void Update() override
			{
				// TODO: 更新処理

				// 指定の状態へ移行する
				main_.SetState(SampleStates::State1);
			}

			void UnInit() override
			{
				// TODO: 終了処理
			}

		private:
			//StateMachineを取得しておくとアクセスできる
			SampleMachine& main_;
		};
	};
	----------------------------------------------*/
}

#endif	// #ifndef INCLUDE_IDEA_STATEMACHINE_H