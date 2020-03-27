#pragma once

#include <commons/math/vec2.hh>
#include <cstdint>
#include "global.hh"

template <typename Q = uint32_t> struct BSPLayout
{
	BSPLayout(Q initWidth, Q initHeight)
	{
		this->root = new BSPNode(false, initWidth, initHeight);
	}
	
	BSPLayout() : BSPLayout(0, 0) {}
	
	~BSPLayout()
	{
		delete this->root;
	}
	
	[[nodiscard]] vec2<Q> pack(Q width, Q height)
	{
		if(width == 0 || height == 0)
		{
			logger << Sev::ERR << "Attempted to pack a tile with 0 width or height" << logger.endl();
			return {};
		}
		vec2<Q> out;
		bool ok;
		this->root->packIter(ok, width, height, out);
		if(!ok)
		{
			if(this->root->width + width > this->root->height + height)
			{
				BSPNode *newRoot = new BSPNode(false, std::max(this->root->width, width), this->root->height + height);
				newRoot->childA = this->root;
				newRoot->childB = new BSPNode(false, std::max(this->root->width, width), height, 0, this->root->height);
				newRoot->packIter(ok, width, height, out);
				this->root = newRoot;
			}
			else
			{
				BSPNode *newRoot = new BSPNode(false, this->root->width + width, std::max(this->root->height, height));
				newRoot->childA = this->root;
				newRoot->childB = new BSPNode(false, width, std::max(this->root->height, height), this->root->width, 0);
				newRoot->packIter(ok, width, height, out);
				this->root = newRoot;
			}
		}
		return out;
	}
	
	[[nodiscard]] inline Q width() const
	{
		return this->root->width;
	}
	
	[[nodiscard]] inline Q height() const
	{
		return this->root->height;
	}

private:
	struct BSPNode
	{
		BSPNode(bool isEndpoint, Q width, Q height, Q x, Q y) : width(width), height(height), coords(x, y), isEndpoint(isEndpoint) {}
		BSPNode(bool isEndpoint, Q width, Q height) : BSPNode(isEndpoint, width, height, 0, 0) {}
		
		~BSPNode()
		{
			if(this->childA && this->childB)
			{
				delete this->childA;
				delete this->childB;
			}
		}
		
		Q const width, height;
		vec2<Q> coords;
		bool isEndpoint;
		
		BSPNode *childA = nullptr, *childB = nullptr;
		
		void packIter(bool &ok, Q const &width, Q const &height, vec2<Q> &pos)
		{
			if(this->isEndpoint || (width > this->width) || (height > this->height))
			{
				ok = false;
				return;
			}
			
			if(this->childA && this->childB)
			{
				this->childA->packIter(ok, width, height, pos);
				if(ok) return;
				this->childB->packIter(ok, width, height, pos);
				return;
			}
			
			if(width == this->width && height == this->height)
			{
				this->isEndpoint = true;
				ok = true;
				pos = this->coords;
				return;
			}
			
			if(width != this->width && height != this->height)
			{
				this->childA = new BSPNode(false, width, this->height, this->coords.x(), this->coords.y());
				this->childB = new BSPNode(false, this->width - width, this->height, this->coords.x() + width, this->coords.y());
				this->childA->packIter(ok, width, height, pos);
				return;
			}
			
			if(width == this->width)
			{
				this->childA = new BSPNode(true, width, height, this->coords.x(), this->coords.y());
				this->childB = new BSPNode(false, width, this->height - height, this->coords.x(), this->coords.y() + height);
				ok = true;
				pos = this->childA->coords;
				return;
			}
			
			if(height == this->height)
			{
				this->childA = new BSPNode(true, width, height, this->coords.x(), this->coords.y());
				this->childB = new BSPNode(false, this->width - width, this->height, this->coords.x() + width, this->coords.y());
				ok = true;
				pos = this->childA->coords;
				return;
			}
		}
	};
	
	BSPNode *root = nullptr;
};
