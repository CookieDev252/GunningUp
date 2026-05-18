#include "behaviourTree.hpp"

BehaviourTree::BehaviourTree()
{

}

//Add a reference of a Node or a child of Node
void Node::AddChild(Node* child)
{
	m_children.push_back(child);
}

//checks if the list of children has anything, returns true if there is
const bool Node::IsThereChildren()
{
	return (m_children.size() > 0);
}

//returns the parent, unless there isn't one, then returns a nullptr
const Node* Node::GetParent()
{
	return m_parent;
}

//returns the selected child unless the index is too hight, then returns a nullptr
const Node* Node::GetChild(unsigned int index)
{
	if (index < m_children.size()) {
		return m_children[index];
	}
	else return nullptr;
};
