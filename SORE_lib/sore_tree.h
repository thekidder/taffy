/***************************************************************************
 *   Copyright (C) 2008 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
// $Id$

#ifndef  __SORE_TREE_H__
#define  __SORE_TREE_H__

#include <map>

namespace SORE_Utility
{
	template<class T>
			class TreeNode
	{
		public:
			TreeNode(TreeNode<T>* _parent, T _node) : node(_node), parent(_parent)
			{
			}
			TreeNode(T _node) : node(_node), parent(NULL)
			{
			}
			
			bool TopLevel() {return parent==NULL;}
			
		protected:
			T node;
			std::vector<TreeNode<T>* > children;
			TreeNode<T>* parent;
	};
	
	template<class T>
			class NamedTree
	{
		public:
			//constructors
			NamedTree<T>(T root_node, std::string name) : nameList("a")
			{
				nodes.push_back(std::pair<std::string, TreeNode<T> >(name, TreeNode<T>(root_node)));
			}
			NamedTree(TreeNode<T> root_node, std::string name) : nameList("a")
			{
				nodes.push_back(std::pair<std::string, TreeNode<T> >(name, root_node));
			}
			NamedTree(T root_node) : nameList("a")
			{
				std::string name = GetNextUnused();
				nodes.push_back(std::pair<std::string, TreeNode<T> >(name,TreeNode<T>(root_node)));
			}
			NamedTree(TreeNode<T> root_node) : nameList("a")
			{
				std::string name = GetNextUnused();
				nodes.push_back(std::pair<std::string, TreeNode<T> >(name, root_node));
			}
			
		protected:
			std::string GetNextUnused()
			{
				std::string toReturn = nameList;
				
				if(nameList[nameList.size()-1]<='z')
					nameList[nameList.size()-1]+=1;
				else
					nameList += "a";
				
				return toReturn;
			}
			
			std::map<std::string, TreeNode<T> > nodes;
			std::string nameList;
	};
}

#endif /*__SORE_TREE_H__*/
