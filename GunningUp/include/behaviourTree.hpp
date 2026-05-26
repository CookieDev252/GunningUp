#ifndef BEHAVIOURTREE_HPP
#define BEHAVIOURTREE_HPP

#include "GunningUp.h"

class Node {
public:
	Node(Node* parent) { this->m_parent = parent; }
	void  AddChild(Node* child);
	const bool IsThereChildren();
	const Node* GetParent();
	const Node* GetChild(unsigned int index);

	virtual void OnEnter() { 5; }
	virtual void OnUpdate(float dt) { 4; }
	virtual void OnExit() { 3; }
private:
	Node* m_parent{ nullptr };
	std::vector<Node*> m_children;
	float m_timeSinceEntered{ 0.0f };
};


//pre declare classes
class Selector;
class Sequence;
class Action;


class BehaviourTree {
public:
	BehaviourTree();
private:
	Node* root{nullptr};
	Node* currentNode{ root };
};


//Things to Add



//Selectors

class Selector : public Node {
public:
	Selector(Node* parent) : Node(parent) {

	}
	virtual void OnEnter() override {}
	virtual void OnUpdate(float dt) override {}
	virtual void OnExit() override {}
};

//Sequennce

class Sequence : public Node {
public:
	Sequence(Node* parent) : Node(parent) {

	}
	virtual void OnEnter() override {}
	virtual void OnUpdate(float dt) override {}
	virtual void OnExit() override {}
};

//Actions {multiple}

class Action : public Node {
	Action(Node* parent) : Node(parent) {

	}
	virtual void OnEnter() override {}
	virtual void OnUpdate(float dt) override {}
	virtual void OnExit() override {}
};

#endif


////adding an enemy behaviour tree
//#ifndef ENEMYBEHAVIOUR
//#define ENEMYBEHAVIOUR
//
//class EnemyFollowPlayer : public Action {
//public:
//	EnemyFollowPlayer(Node* parent) : Action(parent) {}
//};
//
//
//class EnemyBehaviour : public BehaviourTree {
//public:
//	EnemyBehaviour() : BehaviourTree() {}
//private:
//	Enemy* enemyLink{nullptr}; // holds a link to the enemy using it
//
//};
//
//#endif