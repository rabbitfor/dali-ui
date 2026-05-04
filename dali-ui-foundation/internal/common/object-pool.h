#pragma once

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// EXTERNAL INCLUDES
#include <vector>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Small internal object pool for reusable handle/value objects.
 *
 * The pool only stores idle objects. Callers provide creation and reset
 * policies so pooled objects can be normalized for each domain before reuse.
 */
template<typename ObjectType>
class ObjectPool
{
public:
  ObjectPool() = default;

  ObjectPool(const ObjectPool&)            = delete;
  ObjectPool& operator=(const ObjectPool&) = delete;

  ObjectPool(ObjectPool&&)            = default;
  ObjectPool& operator=(ObjectPool&&) = default;

  /**
   * @brief Acquires an object from the pool or creates a new one.
   *
   * @param[in] create Callable returning ObjectType when the pool is empty
   * @return An object ready for caller-owned active use
   */
  template<typename CreateFunction>
  ObjectType Acquire(CreateFunction&& create)
  {
    if(mObjects.empty())
    {
      return create();
    }

    ObjectType object = std::move(mObjects.back());
    mObjects.pop_back();
    return object;
  }

  /**
   * @brief Returns an object to the pool after applying caller-provided reset.
   *
   * @param[in] object The object to store for later reuse
   * @param[in] reset Callable that restores the object to an idle state
   */
  template<typename ResetFunction>
  void Release(ObjectType object, ResetFunction&& reset)
  {
    reset(object);
    mObjects.emplace_back(std::move(object));
  }

  /**
   * @brief Returns the number of idle objects currently held by the pool.
   */
  std::size_t GetIdleCount() const
  {
    return mObjects.size();
  }

  /**
   * @brief Drops all idle objects from the pool.
   */
  void Clear()
  {
    mObjects.clear();
  }

private:
  std::vector<ObjectType> mObjects;
};

} // namespace Internal
} // namespace Ui
} // namespace Dali
