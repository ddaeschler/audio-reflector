/*
 * ObjectPool.h
 *
 *  Created on: Dec 6, 2011
 *      Author: ddaeschler
 */

#ifndef OBJECTPOOL_H_
#define OBJECTPOOL_H_


#pragma once

#include <set>
#include <boost/function.hpp>

namespace audioreflector
{
	template <typename T, typename Pool>
	class ObjectPoolLease;

	/**
	 * Stores live instances of objects for later reuse
	 */
	template <typename T, typename Cmp = std::less<T> >
	class ObjectPool
	{
	private:
		boost::function<T()> _creatorFunctor;
		std::set<T, Cmp> _leasedObjects;
		std::vector<T> _freeObjects;

	public:
		ObjectPool(boost::function<T()> creator)
			: _creatorFunctor(creator)
		{

		}

		virtual ~ObjectPool()
		{
		}

		/**
		 * Lease an object from the pool
		 */
		T lease()
		{
			if (_freeObjects.size() > 0) {
				T obj = _freeObjects.back();
				_freeObjects.pop_back();
				_leasedObjects.insert(obj);

				return obj;

			} else {
				T newObj = _creatorFunctor();
				_leasedObjects.insert(newObj);

				return newObj;
			}
		}

		/**
		 * Return an object to the pool
		 */
		void unlease(T obj)
		{
			_leasedObjects.erase(obj);
			_freeObjects.push_back(obj);
		}
	};
}

#endif /* OBJECTPOOL_H_ */
