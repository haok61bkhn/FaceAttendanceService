#pragma once

#include <atomic>
#include <cstddef>
#include <memory>

template <typename T>
class LockFreeLifoQueue {
 public:
  explicit LockFreeLifoQueue(std::size_t capacity = 1024)
      : _capacity(capacity), _size(0) {
    _indexMask = capacity;
    for (std::size_t i = 1; i <= sizeof(void*) * 4; i <<= 1)
      _indexMask |= _indexMask >> i;
    _abaOffset = _indexMask + 1;

    _queue =
        reinterpret_cast<Node*>(::operator new(sizeof(Node) * (capacity + 1)));
    for (std::size_t i = 1; i < capacity;) {
      Node& node = _queue[i];
      node.abaNextFree = ++i;
    }
    _queue[capacity].abaNextFree = 0;

    _abaFree = 1;
    _abaPushed = 0;
  }

  ~LockFreeLifoQueue() {
    for (std::size_t abaPushed = _abaPushed;;) {
      std::size_t nodeIndex = abaPushed & _indexMask;
      if (!nodeIndex) break;
      Node& node = _queue[nodeIndex];
      abaPushed = node.abaNextPushed;
      (&node.data)->~T();
    }

    ::operator delete(_queue);
  }

  std::size_t capacity() const { return _capacity; }

  std::size_t size() const { return _size.load(); }

  bool push(const T& data) {
    Node* node;
    std::size_t abaFree;
    for (;;) {
      abaFree = _abaFree.load();
      std::size_t nodeIndex = abaFree & _indexMask;
      if (!nodeIndex) return false;
      node = &_queue[nodeIndex];
      if (std::atomic_compare_exchange_strong(&_abaFree, &abaFree,
                                              node->abaNextFree + _abaOffset)) {
        break;
      }
    }

    new (&node->data) T(data);

    // Increment the size after successful push
    _size.fetch_add(1, std::memory_order_relaxed);

    for (;;) {
      std::size_t abaPushed = _abaPushed.load();
      node->abaNextPushed = abaPushed;
      if (std::atomic_compare_exchange_strong(&_abaPushed, &abaPushed,
                                              abaFree)) {
        return true;
      }
    }
  }

  bool pop(T& result) {
    Node* node;
    std::size_t abaPushed;
    for (;;) {
      abaPushed = _abaPushed.load();
      std::size_t nodeIndex = abaPushed & _indexMask;
      if (!nodeIndex) return false;
      node = &_queue[nodeIndex];
      if (std::atomic_compare_exchange_strong(
              &_abaPushed, &abaPushed, node->abaNextPushed + _abaOffset)) {
        break;
      }
    }

    result = node->data;
    (&node->data)->~T();

    // Decrement the size after successful pop
    _size.fetch_sub(1, std::memory_order_relaxed);

    abaPushed += _abaOffset;
    for (;;) {
      std::size_t abaFree = _abaFree.load();
      node->abaNextFree = abaFree;
      if (std::atomic_compare_exchange_strong(&_abaFree, &abaFree, abaPushed)) {
        return true;
      }
    }
  }

 private:
  struct Node {
    T data;
    std::size_t abaNextFree;
    std::size_t abaNextPushed;
  };

 private:
  std::size_t _indexMask;
  Node* _queue;
  std::size_t _abaOffset;
  std::size_t _capacity;
  alignas(64) std::atomic<std::size_t> _abaFree;
  alignas(64) std::atomic<std::size_t> _abaPushed;
  std::atomic<std::size_t> _size;
};
