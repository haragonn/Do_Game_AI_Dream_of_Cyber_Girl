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

		// ������
		virtual void Init()
		{}

		// �I������
		virtual void UnInit()
		{}

		// �X�V
		virtual void Update()
		{
			if(childState_){
				childState_->Update();
			}
		}

		// ���ID���擾����
		const T& GetId()
		{
			return id_;
		}

		// ��Ԃ�ǉ�����
		void AddState(const std::shared_ptr<State<T>>& state)
		{
			// ���ɓo�^�ς݂��`�F�b�N
			if(childStateList_.find(state->GetId()) == childStateList_.end()){
				childStateList_[state->GetId()] = state;
			}
		}

		// �w��̏�Ԃֈڍs����
		void SetState(T nextStateId)
		{
			Assert(childStateList_.find(nextStateId) != childStateList_.end());	// ���݂��Ȃ���ԂɈڍs���悤�Ƃ���

			if(childState_){
				childState_->UnInit();
				childState_ = childStateList_[nextStateId];
				childState_->Init();
			} else{
				childState_ = childStateList_[nextStateId];
				childState_->Init();
			}
		}

		// ���݂̏�Ԃ�ID��Ԃ�
		const T& GetState() const
		{
			Assert(childState_);	// NULL�`�F�b�N

			return childState_->GetId();
		}

	private:
		// ���ID
		T id_;

		/// ���݂̏��
		std::shared_ptr<State<T>> childState_;

		// ��ԃ��X�g
		std::unordered_map<T, std::shared_ptr<State<T>>> childStateList_;
	};

	template<typename T>
	class StateMachine : public GameObject{
	public:
		StateMachine() :
			state_(nullptr)
		{}

		virtual ~StateMachine() = default;

		// �X�V
		virtual void Update()override
		{
			if(state_){
				state_->Update();
			}
		}

		// ��Ԃ�ǉ�����
		void AddState(const std::shared_ptr<State<T>>& state)
		{
			// ���ɓo�^�ς݂��`�F�b�N
			if(stateList_.find(state->GetId()) == stateList_.end()){
				stateList_[state->GetId()] = state;
			}
		}

		// �w��̏�Ԃֈڍs����
		void SetState(T nextStateId)
		{
			Assert(stateList_.find(nextStateId) != stateList_.end());	// ���݂��Ȃ���ԂɈڍs���悤�Ƃ���

			if(state_ != nullptr){
				state_->UnInit();
				state_ = stateList_[nextStateId];
				state_->Init();
			} else{
				state_ = stateList_[nextStateId];
				state_->Init();
			}
		}

		// ���݂̏�Ԃ�ID��Ԃ�
		const T& GetState() const
		{
			Assert(state_);	// NULL�`�F�b�N

			return state_->GetId();
		}

		// �q���w��̏�Ԃֈڍs����
		void SetChildState(T nextChildStateId)
		{
			Assert(stateList_.find(nextChildStateId) != stateList_.end());	// ���݂��Ȃ���ԂɈڍs���悤�Ƃ���

			state_->SetState(nextChildStateId);
		}

		// �q�̌��݂̏�Ԃ�ID��Ԃ�
		const T& GetChildState() const
		{
			Assert(state_);	// NULL�`�F�b�N

			return state_->GetState();
		}

	private:
		/// ���݂̏��
		std::shared_ptr<State<T>> state_;

		// ��ԃ��X�g
		std::unordered_map<T, std::shared_ptr<State<T>>> stateList_;
	};

	/*----------------------------------------------
	// �T���v��
	//----------------------------------------------
	// �T���v���X�e�[�g�R
	enum class SampleStates{
		State1,
		State2
	};

	// �T���v���X�e�[�g�}�V��
	class SampleMachine : public StateMachine<SampleStates>{
	public:
		// ������
		void Init() override
		{
			// TODO: ����������

			// ��Ԃ�ǉ�����
			AddState(std::make_shared<State1>(*this));
			AddState(std::make_shared<State2>(*this));

			// ������Ԃ�ݒ�
			SetState(SampleStates::State1);
		}

		void Update()override
		{
			// TODO: ���ʂ̑O����

			StateMachine::Update();

			// TODO: ���ʂ̌㏈��
		}
		void Draw()override
		{
			// TODO: �`�揈��
		}

		//----------------------------------------------
		// ��Ԃ��C���i�[�N���X�Œ�`����
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
				// TODO: ����������
			}

			void Update() override
			{
				// TODO: �X�V����

				// �w��̏�Ԃֈڍs����
				main_.SetState(SampleStates::State2);
			}

			void UnInit() override
			{
				// TODO: �I������
			}

		private:
			//StateMachine���擾���Ă����ƃA�N�Z�X�ł���
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
				// TODO: ����������
			}

			void Update() override
			{
				// TODO: �X�V����

				// �w��̏�Ԃֈڍs����
				main_.SetState(SampleStates::State1);
			}

			void UnInit() override
			{
				// TODO: �I������
			}

		private:
			//StateMachine���擾���Ă����ƃA�N�Z�X�ł���
			SampleMachine& main_;
		};
	};
	----------------------------------------------*/
}

#endif	// #ifndef INCLUDE_IDEA_STATEMACHINE_H