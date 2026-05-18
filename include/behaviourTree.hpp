#ifndef BEHAVIOURTREE_HPP
#define BEHAVIOURTREE_HPP

#include "GunningUp.h"

//pre declare classes
class Node;
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

#endif

//Things to Add

//Node

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
	float m_timeSinceEntered{0.0f};
};

//Selectors

class Selector : Node {
public:
	Selector(Node* parent) : Node(parent) {

	}
	virtual void OnEnter() override {}
	virtual void OnUpdate(float dt) override {}
	virtual void OnExit() override {}
};

//Sequennce

class Sequence : Node {
public:
	Sequence(Node* parent) : Node(parent) {

	}
	virtual void OnEnter() override {}
	virtual void OnUpdate(float dt) override {}
	virtual void OnExit() override {}
};

//Actions {multiple}

class Action : Node {
	Action(Node* parent) : Node(parent) {

	}
	virtual void OnEnter() override {}
	virtual void OnUpdate(float dt) override {}
	virtual void OnExit() override {}
};